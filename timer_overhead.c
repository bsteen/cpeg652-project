#include <time.h>
#include <stdio.h>
#include <limits.h>
#include <omp.h>

int main()
{
	struct timespec begin_time, end_time; 		// Used for timing
	double elapsed_time, total; 				// Used for timing

	clock_gettime(CLOCK_MONOTONIC, &begin_time);
	
	double arr[8];

	int i, a;
	#pragma omp parallel for num_threads(8)
	for (i = 0; i < 1000; i++)
	{

		struct timespec b, e; 		// Used for timing
		double el; 			// Used for timing
		
		a = i+i;
		
		clock_gettime(CLOCK_MONOTONIC, &b); // Start timer
		clock_gettime(CLOCK_MONOTONIC, &e);	// End timer
		el = e.tv_sec - b.tv_sec;
		el += (e.tv_nsec - b.tv_nsec) / 1000000000.0;
		arr[omp_get_thread_num()] += el;
	}
	
	clock_gettime(CLOCK_MONOTONIC, &end_time);	// End timer
	elapsed_time = end_time.tv_sec - begin_time.tv_sec;
	elapsed_time += (end_time.tv_nsec - begin_time.tv_nsec) / 1000000000.0;

	printf("Time for timed loop (seconds): %f\n", elapsed_time);
	
	clock_gettime(CLOCK_MONOTONIC, &begin_time);
	
	#pragma omp parallel for num_threads(8)
	for (i = 0; i < 1000; i++)
	{
		a = i+i;
	}
	
	clock_gettime(CLOCK_MONOTONIC, &end_time);	// End timer
	elapsed_time = end_time.tv_sec - begin_time.tv_sec;
	elapsed_time += (end_time.tv_nsec - begin_time.tv_nsec) / 1000000000.0;

	printf("Time for untimed loop (seconds): %f\n", elapsed_time);
	
	
}

