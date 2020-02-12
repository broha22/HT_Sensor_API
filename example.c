/*
 * @Author: you@you.you
 * @Date:   Tuesday February 11th 2020
 * @Last Modified By:  Brogan Miner
 * @Last Modified Time:  Tuesday February 11th 2020
 * @Copyright:  (c) Oregon State University 2020
 */
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <math.h>

int main()
{
    int Ret;
    sigset_t sigset;
    struct sigevent sig;
    timer_t timerone;

    // initialise the signal set and add SIGUSR1 as a signal we're interested in
    sigemptyset(&sigset);
    sigaddset(&sigset,SIGUSR1);

    // Block the signal we're interested in, this allows us to receive signal information as and when we're ready to
    // process it, as opposed to using sigaction which can only process one signal at a time (if a timer fires while processing
    // another signal handler, typically it is 'missed').
    sigprocmask(SIG_BLOCK,&sigset,NULL);

    // construct a sigevent structure.
    sig.sigev_signo = SIGUSR1;
    sig.sigev_notify = SIGEV_SIGNAL; // see the man pages, but this could be used to specify a handler function to be run in another thread instead of the approach demonstrated here
    sig.sigev_value.sival_int =20; // signal value
    sig.sigev_notify_attributes = NULL;

    //create a new timer.
    Ret = timer_create(CLOCK_REALTIME, &sig, &timerone);

    if (Ret == 0)
    {
        // set the time resolution for the timer created above.

        struct itimerspec in, out;
        in.it_value.tv_sec = 0;
        in.it_value.tv_nsec = (0.5 * powl(10,9)); // start firing after half a second
        in.it_interval.tv_sec = 0;
        in.it_interval.tv_nsec = (50 * powl(10,6));// fire every 50 ms

        Ret = timer_settime(timerone, 0, &in, &out);
    }
    else
    {
        printf("timer_create() failed with %d\n", errno);
        exit(1);
    }
    Ret = 0;

   // loop until killed.
    while(1)
    {
      siginfo_t info;
      sigemptyset(&sigset);
      sigaddset(&sigset,SIGUSR1);

      //You can use a sigtimedwait here (see man page for details) to specify a maximum wait time
      // for interesting events. For example this would allow you to perform 'idle' operations where
      // the system is not busy.

      while(sigwaitinfo(&sigset,&info)>0)
      {
        printf("[%d] Caught signal %d\n",(int)time(0),info.si_value.sival_int);
      }
    }

    return 0;
}