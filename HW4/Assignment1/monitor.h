#include <pthread.h>
#include <iostream>
#include <sys/time.h>

class monitor
{
    public:
        monitor(); /* Constructor. */
        ~monitor(); /* Destructor. */

        void fill_pot(long number); /* Fill the pot with one portion of honey. */
        void eat_honey(); /* Eats the honey once the pot is full. */

        const static int MAX_COUNT_HONEY = 5; /* Max portions of honey in pot. */
    private:
        pthread_cond_t pot_empty; /* Used to wait for the pot to be empty. */
        pthread_cond_t pot_full; /* Used to wait fot the pot to be full. */
        pthread_mutex_t pot_lock; /* Mutex lock for the pot. */
        pthread_mutex_t eat_lock; /* Mutex lock for the eat function */

        int honey_count; /* Counter for the amount of honey in the pot. */
};
