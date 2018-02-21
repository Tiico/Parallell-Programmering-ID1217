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

        void fill_dish(); /* Fill the pot with one portion of honey. */
        void eat_dish(long number); /* Eats the honey once the pot is full. */

        const static int MAX_COUNT_DISH = 50; /* Max portions of honey in pot. */
    private:
        double read_timer();

        pthread_cond_t pot_empty; /* Used to wait for the pot to be empty. */
        pthread_cond_t pot_full; /* Used to wait fot the pot to be full. */
        pthread_mutex_t pot_lock; /* Mutex lock for the pot. */

        int dish_count; /* Counter for the amount of honey in the pot. */
};

#endif
