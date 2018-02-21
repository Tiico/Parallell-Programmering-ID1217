#ifndef __FEED_MONITOR
#define __FEED_MONITOR

#include <pthread.h>
#include <iostream>
#include <sys/time.h>

class monitor
{
    public:
        monitor(); /* Constructor. */
        ~monitor(); /* Destructor. */

        void fill_dish(); /* Fill the dish with random amount of food. */
        void eat_dish(long number); /* Eats the food. */

        const static int MAX_COUNT_DISH = 50; /* Max portions*/
    private:
        pthread_cond_t dish_empty; /* Used to wait for the dish to be empty. */
        pthread_cond_t dish_full; /* Used to wait fot the dish to be full. */
        pthread_mutex_t dish_lock; /* Mutex lock for the dish. */

        int dish_count; /* Counter for the amount of food in the dish. */
};

#endif
