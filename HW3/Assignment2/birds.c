#ifndef _REENTRANT
#define _REENTRANT
#endif
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#define CAPACITY 50  /* Pot CAPACITY */
#define MAXBABYBIRDS 30   /* maximum number of baby birds */
#define MAXINTERVAL 5
#define MININTERVAL 1

sem_t dishFull, dishEmpty, critSec;

int amountBaby;

/*Counters for Bee's */
int dishCounter;

void *babybird(void *);
void *mamabird(void *);

int main(int argc, char *argv[]) {
  long l; /* use long in case of a 64-bit system */
  //Seed the rand function
  srand(time(NULL));
  pthread_attr_t attr;
  pthread_t animal[MAXBABYBIRDS + 1];

  /* set global thread attributes */
  pthread_attr_init(&attr);
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

  sem_init(&dishFull, 0, 0);
  sem_init(&dishEmpty, 0, 0);
  sem_init(&critSec, 0, 1);

  /* read command line args if any */
  amountBaby = (argc > 1)? atoi(argv[1]) : MAXBABYBIRDS;
  if (amountBaby > MAXBABYBIRDS) amountBaby = MAXBABYBIRDS;

  /* do the parallel work: create the workers */
  for (l = 0; l < amountBaby; l++){
    pthread_create(&animal[l], &attr, babybird, (void *) l);
  }
    pthread_create(&animal[amountBaby], &attr, mamabird, NULL);

  for (l = 0; l < amountBaby; l++) {
      pthread_join(animal[l], NULL);
  }
  pthread_exit(NULL);
}

/* Each worker sums the values in one strip of the matrix.
   After a barrier, worker(0) computes and prints the total */
   void *babybird(void *arg) {
     long id = (long)arg;
     while (true) {
       sleep((rand() % (MAXINTERVAL - MININTERVAL) + MININTERVAL));

       sem_wait(&critSec);
       printf("Thread ID: %d eats from dish, (%d/%d)\n", id, dishCounter, CAPACITY);
       if (dishCounter == 0) {
         printf("Dish is empty!\n");
          sem_post(&dishEmpty);
          printf("Thread ID: %d Chirps for more food!\n", id);
          sem_wait(&dishFull);
          printf("Mama filled the dish\n");
       }
       dishCounter--;
       sem_post(&critSec);
     }
     pthread_exit(NULL);
   }

   void *mamabird(void *arg) {
     while(true){
     sem_wait(&dishEmpty);
     printf("MamaBird fills the dish \n");
     sleep((rand() % (MAXINTERVAL - MININTERVAL) + MININTERVAL));
     dishCounter = (rand()%CAPACITY)+1;
     sem_post(&dishFull);
   }
     pthread_exit(NULL);
   }
