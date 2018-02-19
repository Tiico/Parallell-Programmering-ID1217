#ifndef _REENTRANT
#define _REENTRANT
#endif
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "monitor.h"
#include <iostream>
#include <stdbool.h>
#define CAPACITY 50  /* Pot CAPACITY */
#define MAXBEES 10   /* maximum number of bees */
#define MAXMEALS 10  /* specify max amount of meals for the bear.*/
#define MAX_GATHER_TIME 500 /* Max time of gathering honey. */
#define MIN_GATHER_TIME 100 /* Min time of gathering honey. */

int amountBees, times_eaten;

/*Counters for Bee's */
int potCounter;

void *bee(void *);
void *bear(void *);

bool done;

monitor * mon;

int main(int argc, char *argv[]) {
  long l; /* use long in case of a 64-bit system */
  done = false;
  //Seed the rand function
  srand(time(NULL));
  pthread_attr_t attr;
  pthread_t animal[MAXBEES + 1];

  /* set global thread attributes */
  pthread_attr_init(&attr);
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

  /* read command line args if any */
  amountBees = (argc > 1)? atoi(argv[1]) : MAXBEES;
  if (amountBees > MAXBEES) amountBees = MAXBEES;

  mon = new monitor();
  /* do the parallel work: create the workers */
  for (l = 0; l < amountBees; l++){
    pthread_create(&animal[l], &attr, bee, (void *) l);
  }
    pthread_create(&animal[amountBees], &attr, bear, NULL);

  for (l = 0; l < amountBees + 1; l++) {
      pthread_join(animal[l], NULL);
  }
  delete mon;
  pthread_exit(NULL);
}

   void *bee(void *arg) {
     long number = (long)arg;
     while (done == false) {
       sleep((rand() % (MAX_GATHER_TIME - MIN_GATHER_TIME)) + MIN_GATHER_TIME);

       if(done){
         pthread_exit(NULL);
       }
       mon->fill_pot(number);
       if(done){
         pthread_exit(NULL);
       }
     }
     pthread_exit(NULL);
   }

   void *bear(void *arg) {
     while(true){
      mon->eat_honey();
      times_eaten++;
      std::cout << "The bear has now eaten " << times_eaten << " out of " << MAXMEALS << " times." << std::endl;
      if(times_eaten >= MAXMEALS){
        done = true;
        pthread_exit(NULL);
    }
  }
}
