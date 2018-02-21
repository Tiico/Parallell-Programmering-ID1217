#include "monitor.h"
/**
 * Intialize the monitor.
 */
monitor::monitor()
{
    /* Initiates the lock and the condition variables. */
    pthread_mutex_init(&dish_lock, NULL);
    pthread_cond_init(&dish_empty, NULL);
    pthread_cond_init(&dish_full, NULL);

    /* Initiates the counter. */
    dish_count = MAX_COUNT_DISH;
}

/**
 * Destructor. Nothing to do here as there are nothing saved on the heap.
 */
monitor::~monitor()
{
}

/**
 * Method used for the babybirds to eat from the dish.
 */
void monitor::eat_dish(long number)
{
      pthread_mutex_lock(&dish_lock); /* Lock for mutex. */
      if (dish_count > 0) {
      dish_count--;
      std::cout << "Babybird " << number << " eats from the dish (" << dish_count << "/" << MAX_COUNT_DISH << ")" << std::endl;
      while (dish_count == 0) {
        pthread_cond_broadcast(&dish_empty);
        std::cout << "Babybird " << number << " chirps for more food" << std::endl;
        pthread_cond_wait(&dish_full, &dish_lock);
      }
    }
      pthread_mutex_unlock(&dish_lock); /* Unlock because we no longer need mutex. */
}

/**
 * Method used for the mamabird when filling the dish with more food.
 */
void monitor::fill_dish()
{
    pthread_cond_wait(&dish_empty, &dish_lock);
    printf("MamaBird fills the dish \n");
    dish_count = (rand()%MAX_COUNT_DISH)+1;
    pthread_cond_broadcast(&dish_full);
}
