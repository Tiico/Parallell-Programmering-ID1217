/* matrix summation using OpenMP

   usage with gcc (version 4.2 or higher required):
     gcc -O -fopenmp -o matrixSum-openmp matrixSum-openmp.c
     ./matrixSum-openmp size numWorkers

*/

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

double start_time, end_time;

#define MAXSIZE 10000  /* maximum matrix size */
#define MAXWORKERS 8   /* maximum number of workers */

typedef struct{
  long value;
  int i;
  int j;
} index;

/* Set global variables */
int numWorkers;
int size;
int sum = 0;
int matrix[MAXSIZE][MAXSIZE];
index maxIndices;
index minIndices;

void *Worker(void *);

/* read command line, initialize, and create threads */
int main(int argc, char *argv[]) {
  index localMax;
  index localMin;
  int i, j, total=0;

  /* read command line args if any */
  size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
  numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
  if (size > MAXSIZE) size = MAXSIZE;
  if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;

  omp_set_num_threads(numWorkers);

  /* initialize the matrix */
  for (i = 0; i < size; i++) {
    //  printf("[ ");
	  for (j = 0; j < size; j++) {
      matrix[i][j] = rand()%99;
      //	  printf(" %d", matrix[i][j]);
	  }
	  //	  printf(" ]\n");
  }
  maxIndices.value = matrix[0][0];
  minIndices.value = matrix[0][0];


  start_time = omp_get_wtime();
#pragma omp parallel for reduction (+:total) private(j)
  for (i = 0; i < size; i++)
    for (j = 0; j < size; j++){
      total += matrix[i][j];

      if (matrix[i][j] > localMax.value) {
          localMax.value = matrix[i][j];
          localMax.i = i;
          localMax.j = j;
      }

      if (matrix[i][j] < localMin.value) {
        localMin.value = matrix[i][j];
        localMin.i = i;
        localMin.j = j;
      }
    }

    #pragma omp atomic
        sum += total;

    if (localMax.value > maxIndices.value) {
      if (localMax.value > maxIndices.value) {
        maxIndices.value = localMax.value;
        maxIndices.i = localMax.i;
        maxIndices.j = localMax.j;
      }
      if (localMin.value > minIndices.value) {
        if (localMin.value > minIndices.value) {
        minIndices.value = localMin.value;
        minIndices.i = localMin.i;
        minIndices.j = localMin.j;
      }
    }
// implicit barrier

  end_time = omp_get_wtime();

  printf("the total is %d\n", total);
  printf("%d\n", maxIndices.value);
  printf("it took %g seconds\n", end_time - start_time);
}
}
