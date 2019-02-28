/* FEVS: A Functional Equivalence Verification Suite for High-Performance
 * Scientific Computing
 *
 * Copyright (C) 2009-2010, Stephen F. Siegel, Timothy K. Zirkel,
 * University of Delaware
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA.
 */


/* Name: nbody_par.c: parallel 2-d nbody simulation
 *
 * 2d torus.
 */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <mpi.h>
#include "gd.h"
#define MAXCOLORS 254
#define PWIDTH 1
#define BLOCK_LOW(id,p,n)  ((id)*(n)/(p))
#define BLOCK_HIGH(id,p,n) (BLOCK_LOW((id)+1,p,n)-1)
#define BLOCK_SIZE(id,p,n) \
                     (BLOCK_HIGH(id,p,n)-BLOCK_LOW(id,p,n)+1)
#define BLOCK_OWNER(j,p,n) (((p)*((j)+1)-1)/(n))


typedef struct MassPositionVecStruct {
  double mass;
  double x;
  double y;
} MassPositionVec;

typedef struct VelocityVecStruct {
  double x;
  double y;
} VelocityVec;

typedef struct AttributeStruct {
  int color;
  int size;
} Attribute;

/* Global variables */
int nprocs;          /* number of MPI processes */
int rank;            /* my rank */
int root;            /* rank of root proc */
double x_min;        /* coord of left edge of universe */
double x_max;        /* coord of right edge of universe */
double y_min;        /* coord of bottom edge of universe */
double y_max;        /* coord of top edge of universe */
double univ_x;       /* x_max-x_min */
double univ_y;       /* y_max-y_min */
int nx;              /* width of window (in pixels) */
int ny;              /* height of window */
int numBodiesLocal;  /* number of bodies I control */
int arrayLength;     /* length of arrays == numBodiesLocal on non-root */
int numBodiesGlobal; /* total number of bodies */
double K;            /* constant encoding G and grid spacing */
int nsteps;          /* number of time steps */
int period;          /* number of times steps beween frames */
FILE *gif;           /* file containing animated GIF, root only */
gdImagePtr im, previm;  /* pointers to consecutive GIF images, root only */
int *colors;         /* colors we will use, root only */
int byteCount = 0;
int maxPeakMem = 0;

/* The main data structures. */
MassPositionVec *masspos1, *masspos2, *masspos3;
VelocityVec *v1, *v2;
Attribute *attributes1, *attributes2;

/* MPI Datatype corresponding to structs above */
MPI_Datatype VELOCITY, MASSPOS, ATTRIBUTE;


void* my_malloc(int numBytes) {
  void *result = malloc(numBytes);

  assert(result);
  byteCount += numBytes;
  return result;
}

void printBody(FILE *out, MassPositionVec *masspos,
	       VelocityVec *vel, Attribute *attribute) {
  assert(out); assert(masspos); assert(vel); assert(attribute);
  fprintf(out, "  mass = %lf\n", masspos->mass);
  fprintf(out, "  color = %d\n", attribute->color);
  fprintf(out, "  size = %d\n", attribute->size);
  fprintf(out, "  x= %lf\n", masspos->x);
  fprintf(out, "  y= %lf\n", masspos->y);
  fprintf(out, "  vx= %lf\n", vel->x);
  fprintf(out, "  vy= %lf\n", vel->y);
  fflush(out);
}

void prepgif(char *outfilename) {
  gif = fopen(outfilename, "wb");
  assert(gif);
  colors = (int*)my_malloc(MAXCOLORS*sizeof(int));
  assert(colors);
}

void readConfigFile1(FILE *infile) {
  assert(infile);
  fscanf(infile, "%lf", &x_min);
  printf("x_min = %lf\n", x_min);
  fscanf(infile, "%lf", &x_max);
  printf("x_max = %lf\n", x_max);
  assert(x_max > x_min);
  fscanf(infile, "%lf", &y_min);
  printf("y_min = %lf\n", y_min);
  fscanf(infile, "%lf", &y_max);
  printf("y_max = %lf\n", y_max);
  assert(y_max > y_min);
  fscanf(infile, "%d", &nx);
  printf("nx = %d\n", nx);
  assert(nx>=10);
  fscanf(infile, "%d", &ny);
  printf("ny = %d\n", ny);
  assert(ny>=10);
  fscanf(infile, "%lf", &K);
  printf("K = %f\n", K);
  assert(K>0);
  fscanf(infile, "%d", &nsteps);
  printf("nsteps = %d\n", nsteps);
  assert(nsteps>=1);
  fscanf(infile, "%d", &period);
  printf("period = %d\n", period);
  assert(period>0);
  fscanf(infile, "%d", &numBodiesGlobal);
  printf("numBodies = %d\n", numBodiesGlobal);
  assert(numBodiesGlobal>0);
}

void readConfigFile2(FILE *infile) {
  int bodyCount = 0, i, j;

  assert(infile);
  for (i=0; i<nprocs; i++) {
    int numBodies = BLOCK_SIZE(i, nprocs, numBodiesGlobal);
    
#ifdef DEBUG2
    printf("Rank %d: Preparing to read block for proc %d of %d bodies.\n",
	   rank, i, numBodies);
    fflush(stdout);
#endif
    for (j=0; j<numBodies; j++, bodyCount++) {
      double mass, x, y, vx, vy;
      int color, size;

#ifdef DEBUG2
      printf("Rank %d: reading j=%d, bodyCount=%d.\n", rank, j, bodyCount);
      fflush(stdout);
#endif
      fscanf(infile, "%lf", &mass);
      assert(mass > 0);
      fscanf(infile, "%d", &color);
      assert(color >= 0 && color<MAXCOLORS);
      fscanf(infile, "%d", &size);
      assert(size > 0);
      fscanf(infile, "%lf", &x);
      assert(x >= x_min && x < x_max);
      fscanf(infile, "%lf", &y);
      assert(y >= y_min && y < y_max);
      fscanf(infile, "%lf", &vx);
      fscanf(infile, "%lf", &vy);
#ifdef DEBUG2
      printf("Rank %d: read values.\n", rank);
      fflush(stdout);
#endif
      masspos1[j].mass = mass;
      masspos1[j].x = x;
      masspos1[j].y = y;
      v1[j].x = vx;
      v1[j].y = vy;
      attributes1[j].color = color;
      attributes1[j].size = size;
#ifdef DEBUG2
      printf("Body %d:\n", bodyCount);
      printBody(stdout, masspos1+j, v1+j, attributes1+j);
      fflush(stdout);
#endif
    }
    if (i != root) {
      MPI_Send(masspos1, numBodies, MASSPOS, i, 0, MPI_COMM_WORLD);
      MPI_Send(v1, numBodies, VELOCITY, i, 0, MPI_COMM_WORLD);
      MPI_Send(attributes1, numBodies, ATTRIBUTE, i, 0, MPI_COMM_WORLD);
    }
  }
}

void init(char* infilename, char* outfilename) {
  FILE *infile = NULL;
  int i;

  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  root = nprocs - 1;
  MPI_Type_contiguous(3, MPI_DOUBLE, &MASSPOS);
  MPI_Type_commit(&MASSPOS);
  MPI_Type_contiguous(2, MPI_DOUBLE, &VELOCITY);
  MPI_Type_commit(&VELOCITY);
  MPI_Type_contiguous(2, MPI_INT, &ATTRIBUTE);
  MPI_Type_commit(&ATTRIBUTE);
  if (rank == root) {
    assert(infilename);
    assert(outfilename);
    infile = fopen(infilename, "r");
    assert(infile);
    readConfigFile1(infile);
  }
  MPI_Bcast(&x_min, 1, MPI_DOUBLE, root, MPI_COMM_WORLD);
  MPI_Bcast(&x_max, 1, MPI_DOUBLE, root, MPI_COMM_WORLD);
  MPI_Bcast(&y_min, 1, MPI_DOUBLE, root, MPI_COMM_WORLD);
  MPI_Bcast(&y_max, 1, MPI_DOUBLE, root, MPI_COMM_WORLD);
  MPI_Bcast(&nx, 1, MPI_INT, root, MPI_COMM_WORLD);
  MPI_Bcast(&ny, 1, MPI_INT, root, MPI_COMM_WORLD);
  MPI_Bcast(&K, 1, MPI_DOUBLE, root, MPI_COMM_WORLD);
  MPI_Bcast(&nsteps, 1, MPI_INT, root, MPI_COMM_WORLD);
  MPI_Bcast(&period, 1, MPI_INT, root, MPI_COMM_WORLD);
  MPI_Bcast(&numBodiesGlobal, 1, MPI_INT, root, MPI_COMM_WORLD);
#ifdef DEBUG2
  printf("Rank %d: Bcast complete.\n", rank); fflush(stdout);
#endif
  univ_x = x_max-x_min;
  univ_y = y_max-y_min;
  numBodiesLocal = BLOCK_SIZE(rank, nprocs, numBodiesGlobal);
  /* array length must be big enough to handle incoming blocks
   * from any proc.  Also must be >0. */
  arrayLength = numBodiesGlobal/nprocs;
  if (numBodiesGlobal%nprocs != 0) arrayLength++;
  if (arrayLength == 0) arrayLength = 1;
#ifdef DEBUG2
  printf("Rank %d: arrayLength=%d.\n", rank, arrayLength); fflush(stdout);
#endif
  if (rank == root && numBodiesGlobal%nprocs != 0) arrayLength++;
  masspos1 = (MassPositionVec*)my_malloc(arrayLength*sizeof(MassPositionVec));
  assert(masspos1);
  masspos2 = (MassPositionVec*)my_malloc(arrayLength*sizeof(MassPositionVec));
  assert(masspos2);
  masspos3 = (MassPositionVec*)my_malloc(arrayLength*sizeof(MassPositionVec));
  assert(masspos3);
  v1 = (VelocityVec*)my_malloc(arrayLength*sizeof(VelocityVec));
  assert(v1);
  v2 = (VelocityVec*)my_malloc(arrayLength*sizeof(VelocityVec));
  assert(v2);
  attributes1 = (Attribute*)my_malloc(arrayLength*sizeof(Attribute));
  assert(attributes1);
  attributes2 = (Attribute*)my_malloc(arrayLength*sizeof(Attribute));
  assert(attributes2);
#ifdef DEBUG2
  printf("Rank %d: data structs allocated.\n", rank); fflush(stdout);
#endif
  if (rank == root) {
    readConfigFile2(infile);
    fclose(infile);
    prepgif(outfilename);
  } else {
    MPI_Recv(masspos1, numBodiesLocal, MASSPOS, root, 0, MPI_COMM_WORLD,
	     MPI_STATUS_IGNORE);
    MPI_Recv(v1, numBodiesLocal, VELOCITY, root, 0, MPI_COMM_WORLD,
	     MPI_STATUS_IGNORE);
    MPI_Recv(attributes1, numBodiesLocal, ATTRIBUTE, root, 0,
	     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }
#ifdef DEBUG2
  printf("Rank %d: initial data received.\n", rank); fflush(stdout);
#endif
  for (i=0; i<numBodiesLocal; i++) {
    masspos2[i] = masspos1[i];
    v2[i] = v1[i];
    attributes2[i] = attributes1[i];
  }
}

/* write_plain: write current state of all bodies to stdout.
 * Precondition: on all procs, masspos1[i]==masspos2[i],
 *   v1[i]==v2[i], attributes1[i]==attributes2[i]
 * Postcondition: all data structures are preserved, except masspos3, which
 *   is trashed.
 */
void write_plain(int time) {
  int i, bodyCount = 0;

  if (rank == root) {
    printf("\nTime = %d\n", time);
    for (i=0; i<nprocs; i++) {
      int numBodies = BLOCK_SIZE(i, nprocs, numBodiesGlobal);
      MassPositionVec *masspos;
      Attribute *attributes;
      VelocityVec *v;
      int j;
    
      if (i != root) {
	masspos = masspos3;
	v = v2;
	attributes = attributes2;
	MPI_Recv(masspos, numBodies, MASSPOS, i, 0, MPI_COMM_WORLD,
		 MPI_STATUS_IGNORE);
	MPI_Recv(v, numBodies, VELOCITY, i, 0, MPI_COMM_WORLD,
		 MPI_STATUS_IGNORE);
	MPI_Recv(attributes, numBodies, ATTRIBUTE, i, 0, MPI_COMM_WORLD,
		 MPI_STATUS_IGNORE);
      } else {
	masspos = masspos1;
	v = v1;
	attributes = attributes1;
      }
      for (j=0; j<numBodies; j++, bodyCount++) {
	printf("Body %d:\n", bodyCount);
	printBody(stdout, masspos+j, v+j, attributes+j);
      }
    }
    /* repair the damage... */
    for (i=0; i<numBodiesLocal; i++) {
      v2[i] = v1[i];
      attributes2[i] = attributes1[i];
    }
  } else { /* non-root */
    MPI_Send(masspos1, numBodiesLocal, MASSPOS, root, 0, MPI_COMM_WORLD);
    MPI_Send(v1, numBodiesLocal, VELOCITY, root, 0, MPI_COMM_WORLD);
    MPI_Send(attributes1, numBodiesLocal, ATTRIBUTE, root, 0, MPI_COMM_WORLD);
  }
}

/* write_frame: write a frame to the animated gif file.  Non-roots
 * send to root only those bodies that fit in frame.
 */
void write_frame(int time) {
  int i;
  
  if (rank == root) {
    im = gdImageCreate(nx,ny);
    if (time == 0) {
      gdImageColorAllocate(im, 0, 0, 0);  /* black background */
      for (i=0; i<MAXCOLORS; i++)
	colors[i] = gdImageColorAllocate (im, i, 0, MAXCOLORS-i-1); 
      gdImageGifAnimBegin(im, gif, 1, -1);
    } else {
      gdImagePaletteCopy(im, previm);
    }
    for (i=0; i<nprocs; i++) {
      int numBodies = BLOCK_SIZE(i, nprocs, numBodiesGlobal);
      MassPositionVec *masspos;
      Attribute *attributes;
      VelocityVec *v;
      int j, numBodiesReceived;
      MPI_Status status;
    
      if (i != root) {
	masspos = masspos2;
	v = v2;
	attributes = attributes2;
	MPI_Recv(masspos, numBodies, MASSPOS, i, 0, MPI_COMM_WORLD, &status);
	MPI_Get_count(&status, MASSPOS, &numBodiesReceived);
	MPI_Recv(attributes, numBodiesReceived, ATTRIBUTE, i, 0,
		 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      } else {
	masspos = masspos1;
	v = v1;
	attributes = attributes1;
	numBodiesReceived = numBodiesLocal;
      }
      for (j=0; j<numBodiesReceived; j++) {
	double x = masspos[j].x;
	double y = masspos[j].y;
	int size = attributes[j].size;
	int color = attributes[j].color;

	if (rank != root || (x>=0 && x<nx && y>=0 && y<ny)) {
	  gdImageFilledEllipse(im, (int)x, ny-(int)y, size, size,
			       colors[color]);
	}
      }
    }
    if (time == 0) {
      gdImageGifAnimAdd(im, gif, 0, 0, 0, 0, gdDisposalNone, NULL);
    } else {
      gdImageGifAnimAdd(im, gif, 0, 0, 0, 5, gdDisposalNone, /* previm */ NULL);
      gdImageDestroy(previm);
    }
    previm=im;
    im=NULL;
    for (i=0; i<numBodiesLocal; i++) {
      masspos2[i]=masspos1[i];
      attributes2[i] = attributes1[i];
    }
  } else { /* non-root */
    /* send only those bodies that fit in window...*/
    int numToSend = 0;

    for (i=0; i<numBodiesLocal; i++) {
      double x = masspos1[i].x;
      double y = masspos1[i].y;

      if (x>=0 && x<nx && y>=0 && y<ny) {
	masspos2[numToSend] = masspos1[i];
	attributes2[numToSend] = attributes1[i];
	numToSend++;
      }
    }
    MPI_Send(masspos2, numToSend, MASSPOS, root, 0, MPI_COMM_WORLD);
    MPI_Send(attributes2, numToSend, ATTRIBUTE, root, 0, MPI_COMM_WORLD);
    for (i=0; i<numToSend; i++) {
      masspos2[i] = masspos1[i];
      attributes2[i] = attributes1[i];
    }
  }
#ifdef DEBUG
  write_plain(time);
#endif
}

/* For i=1..nprocs-1: exchange by sending i to your right while
 * receiving i to your left (cyclically).
 *
 * Prereq: masspos1==masspos2, v1==v2
 * Post:   masspos1==masspos2, v1==v2, both with updated values
 *         (masspos3 is junked)
 */
void update() {
  int i;

#ifdef DEBUG2
  printf("Rank %d: in update\n", rank); fflush(stdout);
  for (i=0; i<numBodiesLocal; i++) {
    assert(masspos1[i].x == masspos2[i].x);
    assert(masspos1[i].y == masspos2[i].y);
    assert(masspos1[i].mass == masspos2[i].mass);
    assert(v1[i].x==v2[i].x);
    assert(v1[i].y==v2[i].y);
  }
#endif

  for (i=0; i<nprocs; i++) {
    MassPositionVec *masspos;
    int j;
    int leftNumBodies;
    MPI_Request sendRequest;

    if (i==0) { /* local contributions */
      masspos = masspos2;
      leftNumBodies = numBodiesLocal;
      sendRequest = MPI_REQUEST_NULL;
    } else {
      int right = (rank+i)%nprocs;
      int left = (rank+nprocs-i)%nprocs;
      int numReceived;
      MPI_Status status;

#ifdef DEBUG2
      if (numBodiesLocal>0) {
	printf("Rank %d: sending mass %lf to %d\n", rank, masspos1[0].mass, right);
	fflush(stdout);
      }
#endif

      leftNumBodies = BLOCK_SIZE(left, nprocs, numBodiesGlobal);
      masspos = masspos3;
      MPI_Isend(masspos1, numBodiesLocal, MASSPOS, right, 0,
		MPI_COMM_WORLD, &sendRequest);
      MPI_Recv(masspos3, leftNumBodies, MASSPOS, left, 0,
	       MPI_COMM_WORLD, &status);
      MPI_Get_count(&status, MASSPOS, &numReceived);

#ifdef DEBUG2
      printf("Rank %d: update received %d bodies from rank %d: ",
	     rank, numReceived, left);
      if (numReceived>0)
	printf("mass=%lf, x=%lf, y=%lf", masspos3[0].mass,
	       masspos3[0].x, masspos3[0].y);
      printf("\n");
      fflush(stdout);
#endif

      assert(leftNumBodies == numReceived);
    }
    for (j=0; j<numBodiesLocal; j++) {
      double x = masspos2[j].x;
      double y = masspos2[j].y;
      double ax = 0;
      double ay = 0;
      int k;
      
      for (k=0; k<leftNumBodies; k++) {
	double r, mass, dx, dy, r_squared, acceleration;

	dx = masspos[k].x - x;
	dy = masspos[k].y - y;
	mass = masspos[k].mass;

#ifdef DEBUG2
	printf("Rank %d: x3=%lf, i=%d, j=%d, k=%d, dx=%lf, dy=%lf, mass=%lf\n",
	       rank, masspos[k].x, i, j, k, dx, dy, mass);
	fflush(stdout);
#endif

	r_squared = dx*dx + dy*dy;
	if (r_squared != 0) {
	  r = sqrt(r_squared);
	  if (r != 0) {
	    acceleration = K*mass/(r_squared);

#ifdef DEBUG2
	    printf("Rank %d: acceleration = %lf\n", rank, acceleration);
	    fflush(stdout);
#endif

	    ax += acceleration*dx/r;
	    ay += acceleration*dy/r;
	  }
	}
      }

#ifdef DEBUG2
      printf("Rank %d: j=%d, ax=%lf, ay=%lf\n", rank, j, ax, ay);
      fflush(stdout);
#endif

      v2[j].x += ax;
      v2[j].y += ay;
    }
    if (i != 0) MPI_Wait(&sendRequest, MPI_STATUS_IGNORE);
  }
  for (i=0; i<numBodiesLocal; i++) {
    double x = masspos2[i].x + v1[i].x;
    double y = masspos2[i].y + v1[i].y;

    if (x>=x_max || x<x_min) x=x+(ceil((x_max-x)/univ_x)-1)*univ_x;
    if (y>=y_max || y<y_min) y=y+(ceil((y_max-y)/univ_y)-1)*univ_y;
    masspos1[i].x = x;
    masspos2[i].x = x;
    masspos1[i].y = y;
    masspos2[i].y = y;
    v1[i] = v2[i];
  }
}

void wrapup() {
  MPI_Allreduce(&byteCount, &maxPeakMem, 1, MPI_INT, MPI_MAX,
		MPI_COMM_WORLD);
  if (rank == root) {
    if (previm) gdImageDestroy(previm);
    gdImageGifAnimEnd(gif);
    fclose(gif);
    free(colors);
  }
  free(masspos1); free(masspos2); free(masspos3);
  free(v1); free(v2);
  free(attributes1); free(attributes2);
  MPI_Type_free(&MASSPOS);
  MPI_Type_free(&VELOCITY);
  MPI_Type_free(&ATTRIBUTE);
}

/* return random int in range [a,b) */
int randomInt(int a, int b) {
  double r = ((double)rand()/((double)(RAND_MAX)+(double)(1))); /* [0,1) */
  double x = a+r*(b-a); /* [a,b) */
  int result = (int)x;

  return result;
}

int main(int argc, char* argv[]) {
  int i;
  double elapsed_time;

  MPI_Init(&argc, &argv);
  MPI_Barrier(MPI_COMM_WORLD);
  elapsed_time = -MPI_Wtime();
  if (argc == 2) {
    printf("Randinit not supported yet.");
    exit(1);
  } else {
    if (argc != 3) {
      printf("Usage: nbody <infilename> <outfilename>\n");
      exit(1);
    }
    init(argv[1], argv[2]);
  }
  write_frame(0);
  for (i=1; i<=nsteps; i++) {
#ifdef DEBUG
    if (rank == root) {
      printf("Computing time %d...", i);
      fflush(stdout);
    }
#endif
    update();
#ifdef DEBUG
    if (rank == root) {
      printf("done.\n");
      fflush(stdout);
    }
#endif
    if (i%period == 0) write_frame(i);
  }
  wrapup();
  MPI_Barrier(MPI_COMM_WORLD);
  elapsed_time += MPI_Wtime();
  if (rank == root) {
    printf("Total time (seconds): %f\n", elapsed_time);
    printf("Max peak memory allocated (bytes): %d\n", maxPeakMem);
    fflush(stdout);	 
  }
  MPI_Finalize();
  return 0;
}
