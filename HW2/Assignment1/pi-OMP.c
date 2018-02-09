#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define MAXSIZE 900000000
#define MAXWORKERS 10

long interval = 0;
int numWorkers;
long double step;
int main(int argc, char *argv[]){

  interval = (argc > 1)? atoi(argv[1]) : MAXSIZE;
  numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
  if (interval > MAXSIZE) interval = MAXSIZE;
  if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;

  long double x, sum = 0.0;
  volatile long double pi = 0.0;
  int i;
  step = 1.0/(double)interval;

  double start_time = omp_get_wtime();
    omp_set_num_threads(numWorkers);
    #pragma omp parallel for private(x) reduction(+:sum)
    for (i = 0; i < interval; i++) {
      x = (i+0.5)*step;
      sum += 4.0/(1.0+x*x);
    }
    pi = step *= sum;
double end_time = omp_get_wtime();
 printf("Estimation of pi is %32.30Lf\n", pi);
 printf("(Actual pi value is 3.141592653589793238462643383279...)\n");
 printf("Time: %f\n", end_time - start_time);
}
