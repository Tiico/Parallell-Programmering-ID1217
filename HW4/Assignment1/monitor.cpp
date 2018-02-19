#include "monitor.h"

monitor::monitor(){
  pthread_mutex_init(&pot_lock, NULL);
  pthread_cond_init(&pot_empty, NULL);
  pthread_cond_init(&pot_full, NULL);

  honey_count = 0;

  read_timer();
}

monitor::~monitor();
{
}

void monitor::fill_pot(long number){
  pthread_mutex_lock(&pot_lock);
    if (honey_count == MAX_COUNT_HONEY) {
      std::cout << read_timer() << ": Bee " << number << " wakes the bear and waits for the pot to be empty."
            << std::endl;
          while (honey_count == MAX_COUNT_HONEY) {
            pthread_cond_broadcast(&pot_full);
            pthread_cond_wait(&pot_empty, &pot_lock);
          }
    }
    honey_count++;
    std::cout << read_timer() << ": Bee " << number << " fills the pot (" << honey_count << "/" <<
            MAX_COUNT_HONEY << ")." << std::endl;
    pthread_mutex_unlock();
}
void monitor::eat_honey(){
  pthread_mutex_lock(&pot_lock);
  std::cout << std::endl << read_timer() << ": Bear wants more honey!" <<
        std::endl;
  if (honey_count < MAX_COUNT_HONEY) {
    std::cout << read_timer() << ": Bear is waiting for the pot to be full."
            << std::endl << std::endl;
            while (honey_count < MAX_COUNT_HONEY) {
              pthread_cond_wait(&pot_full, &pot_lock);
            }
  }
  honey_count = 0;
  std::cout << std::endl << read_timer() << ": Bear ate all the honey!" << std::endl << std::endl;
  pthread_cond_broadcast(&pot_empty);
  pthread_mutex_unlock(&pot_lock);
}

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
