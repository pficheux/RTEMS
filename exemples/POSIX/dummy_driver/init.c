#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>

#include "dummy_drv.h"

#define DEVICE   "/dev/dummy_drv"

int fd;
struct itimerspec ti, ti_old;
timer_t myTimer;

void got_signal (int sig)
{
  int status, v_get;
  static int v_set = 0;

  printf ("setting value to %d (SET)\n", v_set);
  status = ioctl (fd, DUMMY_SET, v_set);

  v_get = ioctl (fd, DUMMY_GET, 0);
  printf ("reading value (GET) -> %d\n", v_get);

  v_set++;
}

void *POSIX_Init() 
{
  struct sigaction sig;
  struct sigevent event;
  sigset_t mask;

  puts( "\n\n*** RTEMS dummy driver test ***\n" );

  if ((fd = open (DEVICE, O_RDWR)) < 0) {
    fprintf (stderr, "error when opening %s (%d)\n", DEVICE, errno);
    exit (1);
  }

  printf ("device %s opened, fd = %d\n\n", DEVICE, fd);

  // Set up signal
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

  // Start timer
  timer_create (CLOCK_REALTIME, &event, &myTimer);

  ti.it_value.tv_sec = 0;
  ti.it_value.tv_nsec = 1000000;
  ti.it_interval.tv_sec = 1;
  ti.it_interval.tv_nsec = 0;

  timer_settime(myTimer, 0, &ti, &ti_old);

  while (1)
    pause ();
}


/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_EXTRA_DRIVERS DUMMY_DRV_DRIVER_TABLE_ENTRY

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 5

#define CONFIGURE_MAXIMUM_POSIX_TIMERS          1
#define CONFIGURE_MAXIMUM_POSIX_THREADS		1

#define CONFIGURE_EXTRA_TASK_STACKS         (6 * RTEMS_MINIMUM_STACK_SIZE)

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* end of file */
