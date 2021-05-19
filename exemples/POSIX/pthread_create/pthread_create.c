#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

void *thread_process (void * arg)
{
  static int cnt = 0;
  struct timespec ts;

  while (1) {
    clock_gettime (CLOCK_REALTIME, &ts);
    printf ("%lld sec %lld nsec\n", ts.tv_sec, ts.tv_nsec);

    printf ("<%s> : Thread %x is running (%d) with arg \"%s\"\n", __FUNCTION__, (int)pthread_self(), cnt, (char*) arg);
    sleep (1);
    cnt++;
  }
}

#ifdef __rtems__
void *POSIX_Init() 
#else
void main (int ac, char **av)
#endif
{
  pthread_t th1, th2;
  struct timespec ts;

  clock_gettime (CLOCK_REALTIME, &ts);
  printf ("Initial time= %lld sec %lld nsec\n", ts.tv_sec, ts.tv_nsec);

  if (pthread_create (&th1, NULL, thread_process, "1") < 0) {
    perror ("pthread_create");
    exit (1);
  }

  if (pthread_create (&th2, NULL, thread_process, "2") < 0) {
    perror ("pthread_create");
    exit (1);
  }

  pause ();
}

#ifdef __rtems__

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS		3
#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

#endif
