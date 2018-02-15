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
#define MAXBEES 10   /* maximum number of bees */
#define MAXMEALS 10  /* specify max amount of meals for the bear.*/
#define MAXINTERVAL 5
#define MININTERVAL 1

sem_t potFull, potEmpty, critSec;

int amountBees;

/*Counters for Bee's */
int potCounter;

void *bee(void *);
void *bear(void *);

int main(int argc, char *argv[]) {
  long l; /* use long in case of a 64-bit system */
  //Seed the rand function
  srand(time(NULL));
  pthread_attr_t attr;
  pthread_t animal[MAXBEES + 1];

  /* set global thread attributes */
  pthread_attr_init(&attr);
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

  sem_init(&potFull, 0, 0);
  sem_init(&potEmpty, 0, 0);
  sem_init(&critSec, 0, 1);

  /* read command line args if any */
  amountBees = (argc > 1)? atoi(argv[1]) : MAXBEES;
  if (amountBees > MAXBEES) amountBees = MAXBEES;

  /* do the parallel work: create the workers */
  for (l = 0; l < amountBees; l++){
    pthread_create(&animal[l], &attr, bee, (void *) l);
  }
    pthread_create(&animal[amountBees], &attr, bear, NULL);

  for (l = 0; l < amountBees; l++) {
      pthread_join(animal[l], NULL);
  }
  pthread_exit(NULL);
}

/* Each worker sums the values in one strip of the matrix.
   After a barrier, worker(0) computes and prints the total */
   void *bee(void *arg) {
     long id = (long)arg;
     while (true) {
       sleep((rand() % (MAXINTERVAL - MININTERVAL) + MININTERVAL));

       sem_wait(&critSec);
       potCounter++;
       printf("Thread ID: %d fills pot, (%d/%d)\n", id, potCounter, CAPACITY);
       if (potCounter >= CAPACITY) {
         printf("Honey pot is full\n");
          sem_post(&potFull);
          printf("Thread ID: %d wakes bear\n", id);
          sem_wait(&potEmpty);
          printf("Bear has eaten the honey\n");
       }
       sem_post(&critSec);
     }
     pthread_exit(NULL);
   }

   void *bear(void *arg) {
     while(true){
     sem_wait(&potFull);
     printf("Bear is awake and eats the honey \n");
     potCounter = 0;
     sem_post(&potEmpty);
   }
     pthread_exit(NULL);
   }
