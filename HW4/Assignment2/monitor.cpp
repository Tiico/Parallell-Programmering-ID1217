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
    dish_count = MAX_COUNT_DISH;

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
void monitor::eat_dish(long number)
{
      pthread_mutex_lock(&pot_lock); /* Lock for mutex. */
      while (dish_count == 0) {
        pthread_cond_broadcast(&pot_empty);
        std::cout << read_timer() << ": Babybird " << number << " chirps for more food." << std::endl;
        pthread_cond_wait(&pot_full, &pot_lock);
      }
      dish_count--;
      std::cout << read_timer() << ": Babybird " << number << " eats from the dish (" << dish_count << "/" << MAX_COUNT_DISH << ")." << std::endl;
      pthread_mutex_unlock(&pot_lock); /* Unlock because we no longer need mutex. */
}

/**
 * Method used for the bear to eat the honey inside the pot once it is full.
 */
void monitor::fill_dish()
{
  while (true) {
    pthread_cond_wait(&pot_empty, &pot_lock);
    printf("MamaBird fills the dish \n");
    dish_count = (rand()%MAX_COUNT_DISH)+1;
    pthread_cond_broadcast(&pot_full);
  }
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
