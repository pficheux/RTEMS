// Testing led for BBB

#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>
#include <time.h>
#include <bsp/gpio.h>
 
void got_signal (int sig)
{
  static int cnt = 0;

  printf ("Got signal %d (%d)\n", sig, ++cnt);
  if (cnt % 2)
    rtems_gpio_set (BBB_LED_USR0);
  else
    rtems_gpio_clear(BBB_LED_USR0);
}

void *POSIX_Init() 
{
  rtems_status_code sc;
  timer_t myTimer;
  struct sigaction sig;
  struct itimerspec ti, ti_old;
  struct sigevent event;
  sigset_t mask;

  printf("Starting timer + GPIO Testing\n");

  /* Initializes the GPIO API */
  rtems_gpio_initialize();

  /* Used Led 0 */
  sc = rtems_gpio_request_pin(BBB_LED_USR0, DIGITAL_OUTPUT, false, false, NULL);
  assert(sc == RTEMS_SUCCESSFUL);

  /* Leds 1 to 3 off */
  sc = rtems_gpio_request_pin(BBB_LED_USR1, DIGITAL_OUTPUT, false, false, NULL);
  assert(sc == RTEMS_SUCCESSFUL);
  rtems_gpio_clear(BBB_LED_USR1);
  sc = rtems_gpio_request_pin(BBB_LED_USR2, DIGITAL_OUTPUT, false, false, NULL);
  assert(sc == RTEMS_SUCCESSFUL);
  rtems_gpio_clear(BBB_LED_USR2);
  sc = rtems_gpio_request_pin(BBB_LED_USR3, DIGITAL_OUTPUT, false, false, NULL);
  assert(sc == RTEMS_SUCCESSFUL);
  rtems_gpio_clear(BBB_LED_USR3);

  
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
  ti.it_interval.tv_sec = 1;
  ti.it_interval.tv_nsec = 0;

  timer_settime(myTimer, 0, &ti, &ti_old);

  while (1)
    pause();
}
 
#define CONFIGURE_MICROSECONDS_PER_TICK 1000
 
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
 
#define CONFIGURE__MAXIMUM_FILE_DESCRIPTORS 10
#define CONFIGURE_UNLIMITED_OBJECTS
#define CONFIGURE_UNIFIED_WORK_AREAS
 
#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_MAXIMUM_POSIX_THREADS		1
#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_MAXIMUM_POSIX_TIMERS          1

#define CONFIGURE_INIT
 
#include <rtems/confdefs.h>
