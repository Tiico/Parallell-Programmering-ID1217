#ifndef _REENTRANT
#define _REENTRANT
#endif

#include <iostream>
#include <pthread.h>
#include "monitor.h"
#include <unistd.h>
#include <stdbool.h>

#define MAX_BEES 10         /* Max number of bees.          */
#define MAXMEALS 10        /* Max number of times eating.  */
#define MAXINTERVAL 3
#define MININTERVAL 1

void * bee(void *);         /* Simulation method for bees.   */
void * bear(void *);        /* Simulation method for bears. */

/* Counters to hold number of bees and the number of
 * times the bear has eaten. */
int bees, times_eaten;

/* Boolean to signal to the bees that the simulation is done. */
bool done;

/* The monitor. */
monitor * mon;

/**
 * Main method. First initializes and then reads the input of how
 * many bees that should be in the simulation. Then
 * creates the threads and then joins them again. */
int main(int argc, char ** argv)
{
    pthread_t animals[MAX_BEES + 1]; /* The threads are saved here. */
    pthread_attr_t attr; /* Attributes for the pthreads. */

    long index; /* For 64-bit systems. */

    times_eaten = 0;

    /* Simulation is not done by default. */
    done = false;

    if(argc > 1){
        /* We cap them at MAX_BEES. */
        bees = (atoi(argv[1]) > MAX_BEES) ? MAX_BEES : atoi(argv[1]);
    }
    else{
      bees = MAX_BEES;
    }

    /* Initialize the attributes. */
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

    /* Seed the randomizer with the time to provide different result every run. */
    srand(time(NULL));

    mon = new monitor(); /* Create the monitor. */
    for(index = 0; index < bees; ++index) /* Spawn the bee threads. */
    {
        pthread_create(&animals[index], &attr, bee, (void *) index);
    }
    pthread_create(&animals[bees], &attr, bear, NULL);
    for(int i = 0; i < bees + 1; ++i) /* Joins the threads again. */
    {
        pthread_join(animals[i], NULL);
    }
    delete mon; /* Delete the monitor when done. */
    return EXIT_SUCCESS;
}
void * bee(void * input)
{
    long number = (long) input;
    while(done == false)
    {
        if(done) /* If the simulation is done. */
        {
            pthread_exit(NULL);
        }
        /* Sleep to simulate time required to gather honey. */
        sleep((rand() % (MAXINTERVAL - MININTERVAL) + MININTERVAL));
        mon->fill_pot(number); /* Fills the pot with one portion of honey. */
        if(done) /* If the simulation is done. */
        {
            pthread_exit(NULL);
        }
    }
    pthread_exit(NULL);
}
void * bear(void * input)
{
    while(true)
    {
        sleep(1);
        mon->eat_honey(); /* Eat honey from the pot. */
        /* Update times_eaten. */
        times_eaten++; /* ONLY the bear can update this, therefore we do not need mutex. */
        std::cout << "The bear has now eaten " << times_eaten << "/" << MAXMEALS << " times." << std::endl;
        if(times_eaten >= MAXMEALS) /* If we are done eating. */
        {
            done = true; /* Signal to the bees that we are done. */
            pthread_exit(NULL);
        }
    }
}
