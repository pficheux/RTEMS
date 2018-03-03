#ifdef __rtems__
#include <bsp.h>
#else
#include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>

void got_signal (int sig)
{
  printf ("Got signal %d\n", sig);
}

#ifdef __rtems__
void *POSIX_Init() 
#else
void main (int ac, char **av)
#endif
{
  timer_t myTimer;
  struct sigaction sig;
  struct itimerspec ti, ti_old;
  struct sigevent event;
  sigset_t mask;

#ifdef __rtems__
  printf("\nTicks per second in your system: %d\n", rtems_clock_get_ticks_per_second());
#endif 

  sig.sa_flags = 0;
  sig.sa_handler = got_signal;
  sigemptyset (&sig.sa_mask);
  sigaction (SIGALRM, &sig, NULL);
  sigemptyset (&mask);
  sigaddset (&mask, SIGALRM);
  sigprocmask (SIG_UNBLOCK, &mask, NULL);

  event.sigev_notify = SIGEV_SIGNAL;
  event.sigev_value.sival_int = 0;
  event.sigev_signo = SIGALRM;

  timer_create (CLOCK_REALTIME, &event, &myTimer);

  ti.it_value.tv_sec = 0;
  ti.it_value.tv_nsec = 50000000;
  ti.it_interval.tv_sec = 0;
  ti.it_interval.tv_nsec = 500000000;

  timer_settime(myTimer, 0, &ti, &ti_old);

  while (1)
    pause ();
}

#ifdef __rtems__

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_TIMERS          1
#define CONFIGURE_MAXIMUM_POSIX_THREADS		1
#define CONFIGURE_POSIX_INIT_THREAD_TABLE
#define CONFIGURE_INIT
#include <rtems/confdefs.h>

#endif
