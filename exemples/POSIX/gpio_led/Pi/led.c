// Testing led for Pi 3

#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>
#include <time.h>
#include <bsp/gpio.h>

#define GPIO_OUT 4
#define GPIO_IN 17

void got_signal (int sig)
{
  static int cnt = 0, modulo = 2;
  int btn_status;

  if (cnt == 0 || (cnt % 1000 == 0)) {
    btn_status = rtems_gpio_get_value (GPIO_IN);
    printf ("cnt= %d modulo= %d %d\n", cnt, modulo, btn_status);

    if (btn_status == 0)
      modulo++;
    if (modulo == 6)
      modulo = 2;
  }
	
  if (cnt % 2)
    rtems_gpio_set (GPIO_OUT);
  else
    rtems_gpio_clear(GPIO_OUT);

  cnt++;
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
  sc = rtems_gpio_request_pin(GPIO_OUT, DIGITAL_OUTPUT, false, false, NULL);
  assert(sc == RTEMS_SUCCESSFUL);
  sc = rtems_gpio_request_pin(GPIO_IN, DIGITAL_INPUT, false, false, NULL);
  assert(sc == RTEMS_SUCCESSFUL);

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
  ti.it_interval.tv_nsec = 1000000;

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
