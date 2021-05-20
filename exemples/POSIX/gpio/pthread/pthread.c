/* 
 * Basic Xenomai test (POSIX) ported to RTEMS
 */

#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <getopt.h>
#include <time.h>
#include <pthread.h>
#include <fcntl.h>
#include <assert.h>
#include <bsp/gpio.h>

#define GPIO_OUT BBB_P8_7

pthread_t thid_square;

#define PERIOD          2000000 // ns -> limited to 10 ms (??)

unsigned long period_ns = 0;
int loop_prt = 100;             /* print every 100 loops */ 
unsigned int test_loops = 0;    /* outer loop count */

/* the struct timespec consists of nanoseconds
 * and seconds. if the nanoseconds are getting
 * bigger than 1000000000 (= 1 second) the
 * variable containing seconds has to be
 * incremented and the nanoseconds decremented
 * by 1000000000.
 */
#define NSEC_PER_SEC    1000000000

static inline void tsnorm(struct timespec *ts)
{
   while (ts->tv_nsec >= NSEC_PER_SEC) {
      ts->tv_nsec -= NSEC_PER_SEC;
      ts->tv_sec++;
   }
}

/* Thread function*/
void *thread_square (void *dummy)
{
  struct timespec ts, tr;
  time_t t = 0, told = 0, jitter;
  static time_t jitter_max = 0, jitter_avg = 0;

  clock_gettime(CLOCK_REALTIME, &ts);
  ts.tv_sec++;

  /* Main loop */
  for (;;)
    {
      // Absolute wait
      if (clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &ts, NULL)) {
	perror ("clock_nanosleep");
	exit (1);
      }

      // Update GPIO
      if (test_loops % 2)
	rtems_gpio_set (GPIO_OUT);
      else
	rtems_gpio_clear(GPIO_OUT);

      // wait period_ns
      ts.tv_nsec += period_ns;
      tsnorm (&ts);

      /* old_time <-- current_time */
      told = t;

      /* get current time */
      clock_gettime (CLOCK_REALTIME, &tr);
      t = (tr.tv_sec * 1000000000) + tr.tv_nsec;    

      // Calculate jitter + display
      jitter = abs(t - told - period_ns);
      jitter_avg += jitter;
      if (test_loops && (jitter > jitter_max))
	jitter_max = jitter;
  
      if (test_loops && !(test_loops % loop_prt)) {
	jitter_avg /= loop_prt;
	//printf ("Loop= %d sec= %ld nsec= %ld delta= %ld ns jitter (ns) current= %ld avg= %ld max= %ld\n", test_loops,  tr.tv_sec, tr.tv_nsec, t-told, jitter, jitter_avg, jitter_max);
	printf ("Loop= %d sec= %lu nsec= %lu delta= %lu ns\n", test_loops,  tr.tv_sec, tr.tv_nsec, t-told);
	jitter_avg = 0;
      }

      test_loops++;
    }
}

#ifdef __rtems__
void *POSIX_Init() 
#else
int main (int ac, char **av)
#endif  
{
  int err, c, sc;
  struct sched_param param_square = { .sched_priority = 99 };
  pthread_attr_t thattr_square;

  period_ns = PERIOD; /* ns */
  
  // Display every 2 sec
  loop_prt = 2000000000 / period_ns;
  
  printf ("Using period %ld ns\n", period_ns);

  /* Initializes the GPIO API */
  rtems_gpio_initialize();

  sc = rtems_gpio_request_pin(GPIO_OUT, DIGITAL_OUTPUT, false, false, NULL);
  assert(sc == RTEMS_SUCCESSFUL);

  // Thread attributes
  pthread_attr_init(&thattr_square);

  // Joinable 
  pthread_attr_setdetachstate(&thattr_square, PTHREAD_CREATE_JOINABLE);

  // Priority, set priority to 99
  pthread_attr_setinheritsched(&thattr_square, PTHREAD_EXPLICIT_SCHED);
  pthread_attr_setschedpolicy(&thattr_square, SCHED_FIFO);
  pthread_attr_setschedparam(&thattr_square, &param_square);

  // Create thread 
  err = pthread_create(&thid_square, &thattr_square, &thread_square, NULL);

  if (err)
    {
      fprintf(stderr,"square: failed to create square thread, code %d\n",err);
      return 0;
    }

  pause();

  return 0;
}

#ifdef __rtems__

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS		2
#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE__MAXIMUM_FILE_DESCRIPTORS 10
#define CONFIGURE_UNLIMITED_OBJECTS
#define CONFIGURE_UNIFIED_WORK_AREAS
 
#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

#endif
