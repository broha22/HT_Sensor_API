/*
 * @Author: you@you.you
 * @Date:   Tuesday February 11th 2020
 * @Last Modified By:  Brogan Miner
 * @Last Modified Time:  Tuesday February 11th 2020
 * @Copyright:  (c) Oregon State University 2020
 */
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>


void handle_timer (union sigval arg) {
  printf("Hello World\n");
}

int main (int argc, char **argv) {

  /* Define timer signal event */
  struct sigevent timer_fired;
  timer_fired.sigev_notify = SIGEV_THREAD;
  // timer_fired.sigev_signo = SIGUSR1;
  timer_fired.sigev_notify_function = handle_timer;
  timer_fired.sigev_notify_attributes = NULL;
  // timer_fired.sigev_value.sival_int = 20;
  // timer_fired.sigev_notify_thread_id = getpid();

  /* Define timer interval */
  struct itimerspec timer_interval;
  struct timespec timer_repeat;
  timer_repeat.tv_sec = 0;        // 0 S
  timer_repeat.tv_nsec = 1000000; // 1 mS
  timer_interval.it_interval = timer_repeat;
  timer_interval.it_interval = timer_repeat;
  timer_interval.it_value.tv_nsec = 1;
  timer_interval.it_value.tv_sec = 0;



  /* initialize timer */
  timer_t timer_id;
  if (timer_create(CLOCK_REALTIME, &timer_fired, &timer_id)) {
    return errno;
  }
  if (timer_settime(timer_id, 0, &timer_interval, NULL)) {
    return errno;
  }

  /* Loop forever */
  while(1){}

  return 0;
}