#include <time.h>
#include <stdio.h>
#include <limits.h>

int main()
{
	struct timespec begin_time, end_time; 		// Used for timing
	double elapsed_time, total; 				// Used for timing

	clock_gettime(CLOCK_MONOTONIC, &begin_time);

	int i, a;
	for (i = 0; i < 1000; i++)
	{

		struct timespec b, e; 		// Used for timing
		double el, total; 			// Used for timing
		
		a = i+i;
		
		clock_gettime(CLOCK_MONOTONIC, &b); // Start timer
		clock_gettime(CLOCK_MONOTONIC, &e);	// End timer
		el = e.tv_sec - b.tv_sec;
		el += (e.tv_nsec - b.tv_nsec) / 1000000000.0;

	}
	
	clock_gettime(CLOCK_MONOTONIC, &end_time);	// End timer
	elapsed_time = end_time.tv_sec - begin_time.tv_sec;
	elapsed_time += (end_time.tv_nsec - begin_time.tv_nsec) / 1000000000.0;

	printf("Time for timed loop (seconds): %f\n", elapsed_time);
	
	clock_gettime(CLOCK_MONOTONIC, &begin_time);

	for (i = 0; i < 1000; i++)
	{
		a = i+i;
	}
	
	clock_gettime(CLOCK_MONOTONIC, &end_time);	// End timer
	elapsed_time = end_time.tv_sec - begin_time.tv_sec;
	elapsed_time += (end_time.tv_nsec - begin_time.tv_nsec) / 1000000000.0;

	printf("Time for untimed loop (seconds): %f\n", elapsed_time);
	
	
}

