/*  pi == 4 * arctan(1), and arctan(x) is the
 *  integral from 0 to x of (1/(1+x*x),
 *  the for loop below approximates that integration.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <stdbool.h>

/* global variables (shared by all threads */
volatile long double global_PI = 0.0; /* the approximation, to 31 sigfigs */
pthread_mutex_t lock;        /* how we synchronize writes to 'pi' */
long double intervals;         /* how finely we chop the integration */
int numThreads;                /* how many threads we use */


void *computePI(void *id)
{
    long double x, steps, local_Sum = 0;
    int threadID = *((int*)id);
    steps = 1.0 / intervals;
    printf("thread %d started\n",threadID);

    for(int i = threadID ; i < intervals; i += numThreads) {
        x = (i+0.5) * steps;
        local_Sum += 4.0 / (1.0 + x*x);
    }

    local_Sum *= steps;

    pthread_mutex_lock(&lock);
    global_PI += local_Sum;
    pthread_mutex_unlock(&lock);


}
double read_timer() {
    static bool initialized = false;
    static struct timeval start;
    struct timeval end;
    if( !initialized )
    {
        gettimeofday(&start, NULL);
        initialized = true;
    }
    gettimeofday( &end, NULL );
    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}

int main(int argc, char *argv[]){

    pthread_t *threads;
    int *threadID;
    int i;                     // Variable for iteration
    double starttime, endtime;
      intervals = atoi(argv[1]);
      numThreads = atoi(argv[2]);
      threads = malloc(numThreads*sizeof(pthread_t)); //Allocate space in memory
      threadID = malloc(numThreads*sizeof(int));      //Allocate space in memory
      pthread_mutex_init(&lock, NULL);

        starttime = read_timer(); // Reads the clock the first time.

      for (i = 0; i < numThreads; i++) {
        threadID[i] = i;
        pthread_create(&threads[i], NULL, computePI, threadID+i);
      }

      for (i = 0; i < numThreads; i++) {
         pthread_join(threads[i], NULL);
      }
        endtime = read_timer(); // Reads the clock the second time.

      printf("Estimation of pi is %32.30Lf \n", global_PI);
      printf("(Actual pi value is 3.141592653589793238462643383279...)\n");
      printf("Execution time: %g\n", endtime - starttime );

    return 0;
}
