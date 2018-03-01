#include "monitor.h"
/**
 * Intialize the monitor.
 */
monitor::monitor()
{
    /* Initiates the lock and the condition variables. */
    pthread_mutex_init(&pot_lock, NULL);
    pthread_mutex_init(&eat_lock, NULL);
    pthread_cond_init(&pot_empty, NULL);
    pthread_cond_init(&pot_full, NULL);

    /* Initiates the counter. */
    honey_count = 0;
}

/**
 * Destructor. Nothing to do here as there are nothing saved on the heap.
 */
monitor::~monitor()
{
}

/**
 * Method used for the bees to fill the pot with one portion of honey.
 */
void monitor::fill_pot(long number)
{
    pthread_mutex_lock(&pot_lock); /* Lock for mutex. */
    if(honey_count < MAX_COUNT_HONEY) /* If the pot is full we signal to the bear. */
    {
        honey_count++; /* Increase the portions of honey inside the pot. */
        std::cout << "Bee " << number << " fills the pot (" << honey_count << "/" << MAX_COUNT_HONEY << ")" << std::endl;
        while(honey_count == MAX_COUNT_HONEY) /* Make sure we dont try to fill a full pot. */
        {
            std::cout << "Bee " << number << " wakes the bear and waits for the pot to be empty" << std::endl;
            pthread_cond_broadcast(&pot_full); /* Signal the bear that the pot is full. */
            pthread_cond_wait(&pot_empty, &pot_lock); /* Wait for the pot to be empty. */
        }
    }

    pthread_mutex_unlock(&pot_lock); /* Unlock because we no longer need mutex. */
}

/**
 * Method used for the bear to eat the honey inside the pot once it is full.
 */
void monitor::eat_honey()
{
    pthread_mutex_lock(&eat_lock); /* Lock for mutex. */
    if(honey_count < MAX_COUNT_HONEY) /* If the pot is not full we wait. */
    {
        while(honey_count < MAX_COUNT_HONEY){
            pthread_cond_wait(&pot_full, &pot_lock); /* Wait for the pot to be full. */
        }
    }
    honey_count = 0; /* Eat the honey. */
    std::cout << std::endl << "Bear ate all the honey!" << std::endl << std::endl;
    std::cout << "Bear goes to sleep" << std::endl << std::endl;
    pthread_cond_broadcast(&pot_empty); /* Let the bees know that the pot is empty. */
    pthread_mutex_unlock(&eat_lock); /* No long er need mutex. */
}
