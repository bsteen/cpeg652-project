#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
// Create a test parameter file with random planets

// return random int in range [a,b)
int randomInt(int a, int b) {
  double r = ((double)rand()/((double)(RAND_MAX)+(double)(1)));	// [0,1)
  double x = a+r*(b-a); // [a,b)
  int result = (int)x;

  return result;
}

/* This is an alternative to the initialization by reading config
 * file.  This function instead creates 500 random bodies within
 * certain parameters.
 */

 int main(int argc, char* argv[])
 {
	unsigned int seed = 8086;
	int numBodies = 500;
	char *end;

	if (argc == 2)
	{
		seed = strtol(argv[1], &end, 10);
	}
	else if (argc == 3)
	{
		seed = strtol(argv[1], &end, 10);
		numBodies = strtol(argv[2], &end, 10);
	}
	else if(argc > 3)
	{
		printf("Usage: ./test_maker <seed> <numBodies>\n");
		exit(1);
	}

	printf("seed=%d, numBodies=%d\nCreating Tests/random.txt\n", seed, numBodies);

	int x_min = -1000;
	int x_max = 3000;
	int y_min = -600;
	int y_max = 1200;
	int nx = 1000;
	int ny = 600;
	double K = 0.15;
	int nsteps = 1500;
	int period = 2;

	srand(seed);
	FILE *file = fopen("Tests/random.txt", "w");
	assert(file);

	fprintf(file, "%d %d %d %d\n", x_min, x_max, y_min, y_max);
	fprintf(file, "%d %d %lf %d %d %d\n", nx, ny, K, nsteps, period, numBodies);
	fprintf(file, "\n");

	int mass, color, size, x, y, i;
	double vx, vy;

	for (i=0; i<numBodies; i++)
	{
		mass = randomInt(1,8);
		color = randomInt(0,253);
		size = mass;
		x = randomInt(x_min, x_max);
		y = randomInt(y_min, y_max);
		vx = randomInt(-2,2) / 4.0;
		vy = randomInt(-2,2) / 4.0;

		assert(!(isnan(vx) || isnan(vy)));
		fprintf(file, "%d %d %d %d %d %lf %lf\n", mass, color, size, x, y, vx, vy);
	}

	fclose(file);

	return 0;
}