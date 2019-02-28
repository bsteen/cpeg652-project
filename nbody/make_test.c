#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

int main(int argc, char* argv[]) {
  double s = 1000;
  double x_min = -s;
  double x_max = 2*s;
  double y_min = -s;
  double y_max = 2*s;
  int nx = s;
  int ny = s;
  double K = .0001;
  int nsteps = 5000;
  int period = 20;
  int numBodies = 1000;
  int i;
  int x0 = (int)0.2*s, y0 = (int)s/2;
  FILE *file;
  double mass = 1;
  int size = 2;

  file = fopen("bigtest1.txt", "w");
  assert(file);
  fprintf(file, "%lf %lf %lf %lf\n", x_min, x_max, y_min, y_max);
  fprintf(file, "%d %d %lf %d %d %d\n", nx, ny, K, nsteps,
	  period, numBodies);
  fprintf(file, "\n");
  for (i=0; i<numBodies; i++) {
    double r = 1.0*i;
    double theta = i*3.1415926535/50;
    double x = x0+r*cos(theta);
    double y = y0+r*sin(theta);
    double vx = r*(-sin(theta))*.0002;
    double vy = r*cos(theta)*.0002;
    int color = (int)(i*253.0/numBodies);
    
    fprintf(file, "%lf %d %d %lf %lf %lf %lf\n",
	    mass, color, size, x, y, vx, vy);
  }
  fclose(file);
}
