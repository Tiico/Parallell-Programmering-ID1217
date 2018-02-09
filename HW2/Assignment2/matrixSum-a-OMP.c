/* matrix summation using OpenMP

   usage with gcc (version 4.2 or higher required):
     gcc -O -fopenmp -o matrixSum-openmp matrixSum-openmp.c
     ./matrixSum-openmp size numWorkers

*/

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <limits.h>

double start_time, end_time;

#define MAXSIZE 20000  /* maximum matrix size */
#define MAXWORKERS 8   /* maximum number of workers */

typedef struct{
  long value;
  int i;
  int j;
} index;

/* Set global variables */
int numWorkers;
int size;
long sum = 0;
int matrix[MAXSIZE][MAXSIZE];
index Max;
index Min;

void *Worker(void *);

/* read command line, initialize, and create threads */
int main(int argc, char *argv[]) {
  index Max;
  index Min;
  int i, j;
  long total=0;

  /* read command line args if any */
  size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
  numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
  if (size > MAXSIZE) size = MAXSIZE;
  if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;

  omp_set_num_threads(numWorkers);

  /* initialize the matrix */
  srand(time(NULL));
  for (i = 0; i < size; i++) {
	  for (j = 0; j < size; j++) {
      matrix[i][j] = rand()%99;
	  }
  }

  #ifdef DEBUG
    for (i = 0; i < size; i++) {
  	  printf("[ ");
  	  for (j = 0; j < size; j++) {
  	    printf(" %d", matrix[i][j]);
  	  }
  	  printf(" ]\n");
    }
  #endif



  Max.value = matrix[0][0];
  Min.value = matrix[0][0];

  start_time = omp_get_wtime();
#pragma omp parallel for reduction (+:total) private(j)
  for (i = 0; i < size; i++)
    for (j = 0; j < size; j++){
      total += matrix[i][j];

    if (matrix[i][j] > Max.value) {
      #pragma omp critical(updateMax)
      {
      if (matrix[i][j] > Max.value) {
        Max.value = matrix[i][j];
        Max.i = i;
        Max.j = j;
      }
      }
    }
      if (matrix[i][j] < Min.value) {
        #pragma omp critical (updateMin)
        {
        if (matrix[i][j] < Min.value) {
        Min.value = matrix[i][j];
        Min.i = i;
        Min.j = j;
      }
    }
    }
  } // implicit barrier

  end_time = omp_get_wtime();

  printf("the total is %d\n", total);
  printf("Max value: %d (%d, %d)\n", Max.value, Max.j, Max.i);
  printf("Min value: %d (%d, %d)\n", Min.value, Min.j, Min.i);
  printf("it took %g seconds\n", end_time - start_time);
}
