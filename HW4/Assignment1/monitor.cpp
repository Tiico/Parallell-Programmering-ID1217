#include "monitor.h"
/**
 * Intialize the monitor.
 */
monitor::monitor()
{
    /* Initiates the lock and the condition variables. */
    pthread_mutex_init(&pot_lock, NULL);
    pthread_cond_init(&pot_empty, NULL);
    pthread_cond_init(&pot_full, NULL);

    /* Initiates the counter. */
    honey_count = 0;

    /* Read timer the first time. */
    read_timer();
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
    honey_count++; /* Increase the portions of honey inside the pot. */
    std::cout << read_timer() << ": Bee " << number << " fills the pot (" << honey_count << "/" << MAX_COUNT_HONEY << ")." << std::endl;
    if(honey_count == MAX_COUNT_HONEY) /* If the pot is full we signal to the bear. */
    {
        std::cout << read_timer() << ": Bee " << number << " wakes the bear and waits for the pot to be empty." << std::endl;
        while(honey_count == MAX_COUNT_HONEY) /* Make sure we dont try to fill a full pot. */
        {
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
    pthread_mutex_lock(&pot_lock); /* Lock for mutex. */
    std::cout << std::endl << read_timer() << ": Bear wants more honey!" << std::endl;
    if(honey_count < MAX_COUNT_HONEY) /* If the pot is not full we wait. */
    {
        std::cout << read_timer() << ": Bear is waiting for the pot to be full." << std::endl << std::endl;
        while(honey_count < MAX_COUNT_HONEY){
            pthread_cond_wait(&pot_full, &pot_lock); /* Wait for the pot to be full. */
        }
    }
    honey_count = 0; /* Eat the honey. */
    std::cout << std::endl << read_timer() << ": Bear ate all the honey!" << std::endl << std::endl;
    pthread_cond_broadcast(&pot_empty); /* Let the bees know that the pot is empty. */
    pthread_mutex_unlock(&pot_lock); /* No longer need mutex. */
}


/**
 * Timer. First time called will be the starting time and any calls
 * after will calculate the time that has passed since intial call.
 *
 * Taken from lab1.
 */
double monitor::read_timer() {
    static bool initialized = false;
    static struct timeval start;
    struct timeval end;
    if( !initialized )
    {
        gettimeofday( &start, NULL );
        initialized = true;
    }
    gettimeofday( &end, NULL );
    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}
