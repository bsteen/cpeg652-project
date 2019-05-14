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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA.
 */

// nbody_omp.c: Parallel 2-d nbody simulation using OpenMP
// Parallelize inner loop of update, no barriers
// Original program by authors listed above
// Changes and additions done by: Benjamin Steenkamer, 2019
// CPEG 652 Semester Project

#include <assert.h>
#include <gd.h>
#include <math.h>
#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define MAXCOLORS 254
#define PWIDTH 1

/* There is one structure of this type for each "body" in the
 * simulation.	All of the attributes and the current state of the
 * body are recorded in this structure. */
typedef struct BodyStruct {
	double mass;	/* mass of body */
	int color;	   	/* color used to draw this body */
	int size;		/* diameter of body in pixels */
	double x;		/* x position */
	double y;		/* y position */
	double vx;		/* velocity, x-direction */
	double vy;		/* velocity, y-direction */
} Body;

/* Global variables */
double x_min;				/* coord of left edge of universe */
double x_max;				/* coord of right edge of universe */
double y_min;				/* coord of bottom edge of universe */
double y_max;				/* coord of top edge of universe */
double univ_x;				/* x_max-x_min */
double univ_y;				/* y_max-y_min */
int nx;						/* width of movie window (pixels) */
int ny;						/* height of movie window (pixels) */
int numBodies;				/* number of bodies */
double K;					/* single constant encoding G, grid spacing, etc. */
int nsteps;					/* number of time steps */
int period;			 		/* number of times steps beween movie frames */
FILE *gif;			 		/* file containing animated GIF */
gdImagePtr im,  previm;		/* pointers to consecutive GIF images */
int *colors;		 		/* colors we will use */
Body *bodies, *bodies_new;	/* two copies of main data structure: list of bodies */

int num_threads = 0;
double *step_time_sums;

void* my_malloc(int numBytes)
{
  void *result = malloc(numBytes);
  assert(result);

  return result;
}

/* Prepare for GIF creation: open file, allocate color array */
void prepgif(char *outfilename)
{
	gif = fopen(outfilename, "wb");
	assert(gif);
	colors = (int*)my_malloc(sizeof(int) * MAXCOLORS);

	return;
}

/* init: reads init file and initializes variables */
void init(char *infilename, char *outfilename)
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

	#ifndef NO_OUT
	printf("x_min = %lf\n", x_min);
	printf("x_max = %lf\n", x_max);
	printf("y_min = %lf\n", y_min);
	printf("y_max = %lf\n", y_max);
	printf("nx = %d\n", nx);
	printf("ny = %d\n", ny);
	printf("K = %f\n", K);
	printf("nsteps = %d\n", nsteps);
	printf("period = %d\n", period);
	printf("numBodies = %d\n", numBodies);
	fflush(stdout);
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
	}

	for (i=0; i<numBodies; i++)
	{
		bodies_new[i].mass = bodies[i].mass;
		bodies_new[i].color = bodies[i].color;
		bodies_new[i].size = bodies[i].size;
	}

	fclose(infile);

	#ifndef NO_OUT
	prepgif(outfilename);
	#endif

	return;
}

/* Write one frame of the GIF for given time */
void write_frame(int time)
{
	int i;

	im = gdImageCreate(nx,ny);
	if (time == 0)
	{
		gdImageColorAllocate(im, 0, 0, 0);	/* black background */
		for (i=0; i<MAXCOLORS; i++)
		{
			colors[i] = gdImageColorAllocate (im, i, 0, MAXCOLORS-i-1);		/* (im, i,i,i); gives gray-scale image */
		}
		gdImageGifAnimBegin(im, gif, 1, -1);
	}
	else
	{
		gdImagePaletteCopy(im, previm);
	}

	for (i=0; i<numBodies; i++)
	{
		Body *body = bodies + i;
		double x = body->x;

		if (x>=0 && x<nx)
		{
			double y = body->y;
			if (y>=0 && y<ny)
			{
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

	return;
}

/* Move forward one time step.	This is the "integration step".	 For
 * each body b, compute the total force acting on that body.  If you
 * divide this by the mass of b, you get b's acceleration.	So you
 * actually just calculate the b's acceleration directly, since this
 * is what you want to know.  Once you have the acceleration, proceed
 * as follows: update the position by adding the current velocity,
 * then update the velocity by adding to it the current acceleration.
 */
void update() {
	// Main N-body simulation loop
	int step;

	for (step = 1; step <= nsteps; step++)
	{
		struct timespec main_step_s, main_step_e;
		double main_thread_elapsed;
		clock_gettime(CLOCK_MONOTONIC, &main_step_s);
		
		// Loop through the bodies owned by this thread
		
		#pragma omp parallel num_threads(num_threads)
		{
			struct timespec thread_step_s, thread_step_e;
			double thread_elapsed;
			
			if(omp_get_thread_num() != 0)
			{
				// Main thread will already be timing before start of parallel section
				clock_gettime(CLOCK_MONOTONIC, &thread_step_s);
			}
			
			int i;
			#pragma omp for nowait schedule(runtime) 
			for (i = 0; i < numBodies; i++)
			{
				double x = bodies[i].x;
				double y = bodies[i].y;
				double vx = bodies[i].vx;
				double vy = bodies[i].vy;
				double ax = 0;
				double ay = 0;
				int j;

				// Apply effects of all other bodies onto this current body
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

			// Main thread still has work for this step, don't stop it's timer yet
			if(omp_get_thread_num() != 0)
			{
				clock_gettime(CLOCK_MONOTONIC, &thread_step_e);
				thread_elapsed = thread_step_e.tv_sec - thread_step_s.tv_sec;
				thread_elapsed += (thread_step_e.tv_nsec - thread_step_s.tv_nsec) / 1000000000.0;
				step_time_sums[omp_get_thread_num()] += thread_elapsed;
			}
		}

		// Main thread handles sequential operations:
		// Switch old and new arrays and write out frame if needed
		Body *tmp = bodies;
		bodies = bodies_new;
		bodies_new = tmp;

		#ifndef NO_OUT
		if (step % period == 0)
		{
			write_frame(step);
		}
		#endif
		
		// Now stop master thread step time
		clock_gettime(CLOCK_MONOTONIC, &main_step_e);	// End timer
		main_thread_elapsed = main_step_e.tv_sec - main_step_s.tv_sec;
		main_thread_elapsed += (main_step_e.tv_nsec - main_step_s.tv_nsec) / 1000000000.0;
		step_time_sums[0] += main_thread_elapsed;
	}

	return;
}

/* Close GIF file, free all allocated data structures */
void wrapup()
{
	#ifndef NO_OUT
	if (previm)
	{
		gdImageDestroy(previm);
	}

	gdImageGifAnimEnd(gif);
	fclose(gif);
	#endif

	free(colors);
	free(bodies);
	free(bodies_new);
}

/* Perform an n-body simulation and create a GIF movie.	 Usage: you
 * can either specify two arguments: the name of the configuration
 * file and the name of the GIF file you are going to create, or you
 * can specify one argument (just the name of the GIF file), in which
 * case random initialization is used. */
int main(int argc, char* argv[])
{
	struct timespec begin_time, end_time; 	// Used for timing
	double elapsed_time; 					// Used for timing

	if (argc != 4)
	{
		printf("Usage: nbody_omp_v1 <infilename> <outfilename> <number of threads>\n");
		fflush(stdout);
		exit(1);
	}

	num_threads = atoi(argv[3]);
	if(num_threads < 1)
	{
		printf("Need at least 1 thread\n");
		exit(1);
	}

	step_time_sums = (double*)my_malloc(sizeof(double) * num_threads);

	int i;
	for(i = 0; i < num_threads; i++)
	{
		// Initialize the times to 0.0
		step_time_sums[i] = 0.0;
	}

	clock_gettime(CLOCK_MONOTONIC, &begin_time); // Start main program timer

	#ifndef NO_OUT
	printf("Writing to gif: %s\n", argv[2]);
	fflush(stdout);
	#endif

	init(argv[1], argv[2]);

	#ifndef NO_OUT
	write_frame(0);
	#endif

	update();	// Calculate all the steps in the simulation

	wrapup();

	clock_gettime(CLOCK_MONOTONIC, &end_time);
	elapsed_time = end_time.tv_sec - begin_time.tv_sec;
	elapsed_time += (end_time.tv_nsec - begin_time.tv_nsec) / 1000000000.0;

	printf("\nTotal time (seconds): %f\n", elapsed_time);
	fflush(stdout);
	
	for(i = 0; i < num_threads; i++)
	{
		printf("Thread %d avg step time: %f, Total step time %f\n", i, step_time_sums[i] / (nsteps * 1.0), step_time_sums[i]);
	}
	fflush(stdout);

	free(step_time_sums);

	return 0;
}
