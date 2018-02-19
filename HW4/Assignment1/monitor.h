#ifndef __FEED_MONITOR
#define __FEED_MONITOR

#include <pthread.h>
#include <iostream>
#include <sys/time.h>

#define MAX_COUNT_HONEY 50

class monitor{
public:
  monitor();
  ~monitor();

  void fill_pot(long number);
  void eat_honey();

private:
  double read_timer();

  pthread_cond_t pot_empty;
  pthread_cond_t pot_full;
  pthread_mutex_t pot_lock;

  int honey_count;
};

#endif
