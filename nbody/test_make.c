/* return random int in range [a,b) */
int randomInt(int a, int b) {
  double r = ((double)rand()/((double)(RAND_MAX)+(double)(1))); /* [0,1) */
  double x = a+r*(b-a); /* [a,b) */
  int result = (int)x;

  return result;
}
 
/* This is an alternative to the initialization by reading config
 * file.  This function instead creates 500 random bodies within
 * certain parameters.
 */
 
 int main(int argc, char* argv[]) 
 {
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
  int numBodies = 500;
  int i;
  int x0 = (int)0.2*s, y0 = (int)s/2;
  FILE *file;
  double mass = 1;
  int size = 2;

  file = fopen("random.txt", "w");
  assert(file);
  fprintf(file, "%lf %lf %lf %lf\n", x_min, x_max, y_min, y_max);
  fprintf(file, "%d %d %lf %d %d %d\n", nx, ny, K, nsteps, period, numBodies);
  fprintf(file, "\n");
  
  for (i=0; i<numBodies; i++) 
  {
    double r = 1.0*i;
    double theta = i*3.1415926535/50;
    double x = x0+r*cos(theta);
    double y = y0+r*sin(theta);
    double vx = r*(-sin(theta))*.0002;
    double vy = r*cos(theta)*.0002;
    int color = (int)(i*253.0/numBodies);
    
    fprintf(file, "%lf %d %d %lf %lf %lf %lf\n", mass, color, size, x, y, vx, vy);
  }
  
  fclose(file);
  
  return 0;
}

// int i;

// x_max = 3000;
// y_min =-600;
// y_max = 1200;
// univ_x = x_max-x_min;
// univ_y = y_max-y_min;
// nx = 1000;
// ny = 600;
// K = .15;
// nsteps = 1500;
// period = 2;
// numBodies = 500;

// bodies = (Body*)my_malloc(numBodies*sizeof(Body));
// bodies_new = (Body*)my_malloc(numBodies*sizeof(Body));
// for (i=0; i<numBodies; i++) {
// bodies[i].mass = randomInt(1,8);
// bodies[i].color = randomInt(0,253);
// bodies[i].size = bodies[i].mass;
// bodies[i].x = randomInt(x_min, x_max);
// bodies[i].y = randomInt(y_min, y_max);
// bodies[i].vx = randomInt(-1,1)/4;
// bodies[i].vy = randomInt(-1,1)/4;
// assert(!(isnan(bodies[i].x) || isnan(bodies[i].y) ||
	 // isnan(bodies[i].vx) || isnan(bodies[i].vy)));
// }
// for (i=0; i<numBodies; i++) {
// bodies_new[i].mass = bodies[i].mass;
// bodies_new[i].color = bodies[i].color;
// bodies_new[i].size = bodies[i].size;
// }