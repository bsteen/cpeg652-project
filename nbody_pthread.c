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

// nbody_pthread.c: Parallel 2-d nbody simulation using POSIX Threads
// Original program by authors listed above
// Changes and additions done by: Benjamin Steenkamer, 2019
// CPEG 652 Semester Project

#define _POSIX_C_SOURCE 199309L // Needed this define for timing functions
#include <assert.h>
#include "gd.h"
#include <math.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#define MAXCOLORS 254
#define PWIDTH 1

/* There is one structure of this type for each "body" in the
 * simulation.  All of the attributes and the current state of the
 * body are recorded in this structure. */
typedef struct BodyStruct
{
	double mass;     /* mass of body */
	int color;       /* color used to draw this body */
	int size;        /* diameter of body in pixels */
	double x;        /* x position */
	double y;        /* y position */
	double vx;       /* velocity, x-direction */
	double vy;       /* velocity, y-direction */
} Body;

// Global variables
double x_min;        /* coord of left edge of universe */
double x_max;        /* coord of right edge of universe */
double y_min;        /* coord of bottom edge of universe */
double y_max;        /* coord of top edge of universe */
double univ_x;       /* x_max-x_min */
double univ_y;       /* y_max-y_min */
int nx;              /* width of movie window (pixels) */
int ny;              /* height of movie window (pixels) */
int numBodies;       /* number of bodies */
double K;            /* single constant encoding G, grid spacing, etc. */
int nsteps;          /* number of time steps */
int period;          /* number of times steps beween movie frames */
FILE *gif;           /* file containing animated GIF */
gdImagePtr im, previm;      /* pointers to consecutive GIF images */
int *colors;         /* colors we will use */
Body *bodies, *bodies_new;	/* two copies of main data structure: list of bodies */
int byteCount = 0;

// Pthread global variables
int* start_idx_num_owned; // Array containing the startting "local" index in the bodies array and the number bodies a thread owned
//The array is orgranized as follows: [thread 0 start index, thread 0 # owned, thread 0 start index, thread 1 # owned .....]
int num_threads = 0;
int* threads_ids;
pthread_t* threads;

void* my_malloc(int numBytes) 
{
	void *result = malloc(numBytes);

	assert(result);
	byteCount += numBytes;
	return result;
}

// Print out in plain text the given body to the output stream
void printBody(FILE *out, Body *body)
{
	assert(body);
	assert(out);
	fprintf(out, "  mass = %lf\n", body->mass);
	fprintf(out, "  color = %d\n", body->color);
	fprintf(out, "  size = %d\n", body->size);
	fprintf(out, "  x= %lf\n", body->x);
	fprintf(out, "  y= %lf\n", body->y);
	fprintf(out, "  vx= %lf\n", body->vx);
	fprintf(out, "  vy= %lf\n", body->vy);
	fflush(out);
}

// Prepare for GIF creation: open file, allocate color array
void prepgif(char *outfilename) 
{
	gif = fopen(outfilename, "wb");
	assert(gif);
	colors = (int*)my_malloc(MAXCOLORS*sizeof(int));
}

// init: reads init file and initializes variables
void init(char* infilename, char* outfilename) 
{
	FILE *infile = fopen(infilename, "r");
	int i;

	assert(infile);
	fscanf(infile, "%lf", &x_min);
	fscanf(infile, "%lf", &x_max);
	assert(x_max > x_min);
	univ_x = x_max-x_min;
	fscanf(infile, "%lf", &y_min);
	fscanf(infile, "%lf", &y_max);
	assert(y_max > y_min);
	univ_y = y_max-y_min;
	fscanf(infile, "%d", &nx);
	assert(nx>=10);
	fscanf(infile, "%d", &ny);
	assert(ny>=10);
	fscanf(infile, "%lf", &K);
	assert(K>0);
	fscanf(infile, "%d", &nsteps);
	assert(nsteps>=1);
	fscanf(infile, "%d", &period);
	assert(period>0);
	fscanf(infile, "%d", &numBodies);
	assert(numBodies>0);

	#ifndef TIMING
	printf("x_min = %lf\n", x_min);
	printf("x_max = %lf\n", x_max);
	printf("y_max = %lf\n", y_max);
	printf("y_min = %lf\n", y_min);
	printf("nx = %d\n", nx);
	printf("ny = %d\n", ny);
	printf("K = %f\n", K);
	printf("nsteps = %d\n", nsteps);
	printf("period = %d\n", period);
	printf("numBodies = %d\n", numBodies);
	printf("num_threads = %d\n", num_threads);
	#endif

	bodies = (Body*)my_malloc(numBodies*sizeof(Body));
	bodies_new = (Body*)my_malloc(numBodies*sizeof(Body));
	
	for (i=0; i<numBodies; i++) 
	{
		fscanf(infile, "%lf", &bodies[i].mass);
		assert(bodies[i].mass > 0);
		fscanf(infile, "%d", &bodies[i].color);
		assert(bodies[i].color >=0 && bodies[i].color<MAXCOLORS);
		fscanf(infile, "%d", &bodies[i].size);
		assert(bodies[i].size > 0);
		fscanf(infile, "%lf", &bodies[i].x);
		assert(bodies[i].x >=x_min && bodies[i].x < x_max);
		fscanf(infile, "%lf", &bodies[i].y);
		assert(bodies[i].y >=y_min && bodies[i].y < y_max);
		fscanf(infile, "%lf", &bodies[i].vx);
		fscanf(infile, "%lf", &bodies[i].vy);
		#ifdef DEBUG
		printf("Body %d:\n", i);
		printBody(stdout, &bodies[i]);
		#endif
	}
	for (i=0; i<numBodies; i++) 
	{
		bodies_new[i].mass = bodies[i].mass;
		bodies_new[i].color = bodies[i].color;
		bodies_new[i].size = bodies[i].size;
	}
  
	fflush(stdout);
	fclose(infile);
	prepgif(outfilename);
}

 // Write time and then state of all bodies to stdout: use for debugging
void write_plain(int time) 
{
	int i;
	printf("\nTime = %d\n", time);
	
	for (i=0; i<numBodies; i++) {
		printf("Body %d:\n", i);
		printBody(stdout, &bodies[i]);
	}
}

// Write one frame of the GIF for given time
void write_frame(int time)
{
	int i;
	im = gdImageCreate(nx,ny);
	
	if (time == 0) {
		gdImageColorAllocate(im, 0, 0, 0);  /* black background */
		for (i=0; i<MAXCOLORS; i++){
			colors[i] = gdImageColorAllocate (im, i, 0, MAXCOLORS-i-1);	/* (im, i,i,i); gives gray-scale image */
		}
		
		gdImageGifAnimBegin(im, gif, 1, -1);
	} 
	else
	{
		gdImagePaletteCopy(im, previm);
	}
	
	for (i=0; i<numBodies; i++) {
		Body *body = bodies + i;
		double x = body->x;

		if (x>=0 && x<nx)
		{
			double y = body->y;

			if (y>=0 && y<ny) {
				int size = bodies[i].size;
				int color = bodies[i].color;

				gdImageFilledEllipse(im, (int)x, ny-(int)y, size, size, colors[color]);
			}
		}
	}
	
	if (time == 0) 
	{
		gdImageGifAnimAdd(im, gif, 0, 0, 0, 0, gdDisposalNone, NULL);
	} 
	else
	{
		gdImageGifAnimAdd(im, gif, 0, 0, 0, 5, gdDisposalNone, /* previm */ NULL);
		gdImageDestroy(previm);
	}
	
	previm=im;
	im=NULL;
	#ifdef DEBUG
	write_plain(time);
	#endif
}

// Move forward one time step.  This is the "integration step".  For
// each body b, compute the total force acting on that body.  If you
// divide this by the mass of b, you get b's acceleration.  So you
// actually just calculate the b's acceleration directly, since this
// is what you want to know.  Once you have the acceleration, proceed
// as follows: update the position by adding the current velocity,
// then update the velocity by adding to it the current acceleration.

void* update(void* arg)
{
	int i, j;

	int* ID_ptr = (int*)(arg);
	int ID = *ID_ptr;
	int first = start_idx_num_owned[ID * 2];
	int num_owned = start_idx_num_owned[(ID * 2) + 1];

	for (i = first; i < first + num_owned; i++)
	{
		double x = bodies[i].x;
		double y = bodies[i].y;
		double vx = bodies[i].vx;
		double vy = bodies[i].vy;
		double ax = 0;
		double ay = 0;

		for (j = 0; j < numBodies; j++)
		{
			double r, mass, dx, dy, r_squared, acceleration;

			if (j == i)
			{
				continue;
			}

			dx = bodies[j].x - x;
			dy = bodies[j].y - y;
			mass = bodies[j].mass;
			r_squared = dx*dx + dy*dy;

			if (r_squared != 0) {
				r = sqrt(r_squared);
				if (r != 0) 
				{
				  acceleration = K*mass/(r_squared);
				  ax += acceleration*dx/r;
				  ay += acceleration*dy/r;
				}
			}
		}

		x += vx;
		y += vy;
		
		if (x>=x_max || x<x_min)
		{
			x=x+(ceil((x_max-x)/univ_x)-1)*univ_x;
		} 
		if (y>=y_max || y<y_min) 
		{
			y=y+(ceil((y_max-y)/univ_y)-1)*univ_y;
		} 
		
		vx += ax;
		vy += ay;
		assert(!(isnan(x) || isnan(y)));
		assert(!(isnan(vx) || isnan(vy)));
		bodies_new[i].x = x;
		bodies_new[i].y = y;
		bodies_new[i].vx = vx;
		bodies_new[i].vy = vy;
	}

	return NULL;
}

/* Close GIF file, free all allocated data structures */
void wrapup() 
{
	if (previm) gdImageDestroy(previm);
	gdImageGifAnimEnd(gif);
	fclose(gif);
	free(colors);
	free(bodies);
	free(bodies_new);
}


/* Perform an n-body simulation and create a GIF movie.  Usage: you
 * can either specify two arguments: the name of the configuration
 * file and the name of the GIF file you are going to create, or you
 * can specify one argument (just the name of the GIF file), in which
 * case random initialization is used. */
int main(int argc, char* argv[])
{
	int i;
	int j;
	int first;
	Body *tmp;
	struct timespec begin_time, end_time; 	//Used for timing
	double elapsed_time; 					//Used for timing

	clock_gettime(CLOCK_MONOTONIC, &begin_time);//Start timer


	if (argc != 4) {
		printf("Usage: nbody_pthread <infilename> <outfilename> <number of threads>\n");
		exit(1);
	}
	
	num_threads = atoi(argv[3]);
	init(argv[1], argv[2]);

	start_idx_num_owned = (int*)my_malloc(sizeof(int) * num_threads * 2);
	threads_ids = (int*)my_malloc(sizeof(int) * num_threads);
	threads = (pthread_t*)my_malloc(sizeof(pthread_t) * num_threads);

	for(i = 0; i < num_threads; i++){
		threads_ids[i] = i; //i is the thread index or rank
		first = (i * numBodies) / num_threads; //calculate the local starting index
		start_idx_num_owned[i * 2] = first;
		[(i * 2) + 1] = ((i + 1) * numBodies) / num_threads - first; //Calculate the number of bodies owned
	}

	write_frame(0);

	for (i = 1; i <= nsteps; i++)
	{
		#ifdef DEBUG
		printf("Computing time %d...", i);
		fflush(stdout);
		#endif

		for(j = 0; j < num_threads; j++)
		{
			pthread_create(threads + j, NULL, update, threads_ids + j);
		}
		for(j = 0; j < num_threads; j++)
		{
			pthread_join(threads[j], NULL);
		}

		//Swap arrays after running update calculation
		tmp = bodies;
		bodies = bodies_new;
		bodies_new = tmp;

		#ifdef DEBUG
		printf("done.\n");
		fflush(stdout);
		#endif

		if (i%period == 0)
		{
			write_frame(i);
		}
	}

	wrapup();
	free(start_idx_num_owned);
	free(threads_ids);
	free(threads);

	clock_gettime(CLOCK_MONOTONIC, &end_time);//End timer
	elapsed_time = end_time.tv_sec - begin_time.tv_sec;
	elapsed_time += (end_time.tv_nsec - begin_time.tv_nsec) / 1000000000.0;

	printf("Total time (seconds) for pthread: %f\n", elapsed_time);
	
	#ifndef TIMING
	printf("Total memory allocated (bytes): %d\n", byteCount);
	#endif
	
	fflush(stdout);

	return 0;
}
