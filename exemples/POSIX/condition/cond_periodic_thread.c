#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <sys/mman.h>
#include <assert.h>

#include <bsp/gpio.h>
#define GPIO_OUT  BBB_P8_7

#define PERIOD_NS 500000000

pthread_mutex_t my_mutex;
pthread_cond_t my_cond;

void *thread_process (void * arg)
{
  struct timespec t;
  int status;
  unsigned int cnt = 0;

  while (1) 
    {
      status = pthread_mutex_lock( &my_mutex );
      assert ( !status );
      
      status = clock_gettime (CLOCK_REALTIME, &t);
      assert( !status );
      //      t.tv_sec = 0;
      t.tv_nsec += PERIOD_NS;
      
      printf ("Periodic thread is running %llu !\n", t.tv_sec);

      if (cnt % 2)
	rtems_gpio_set (GPIO_OUT);
      else
	rtems_gpio_clear(GPIO_OUT);

      cnt++;
      
      status = pthread_cond_timedwait (&my_cond, &my_mutex, &t);
      if ( status != ETIMEDOUT )
	printf( "*** status = %d\n", status );
      //      assert( status == ETIMEDOUT );
      //      puts( "pthread_cond_timedwait - ETIMEDOUT - (mutex not acquired)" );
      status = pthread_mutex_unlock( &my_mutex );
      assert ( !status );


    }
}

#ifdef __rtems__
void *POSIX_Init() 
#else
void main (int ac, char **av)
#endif
{
  pthread_t th;
  int status;

  /* Initializes the GPIO API */
  rtems_gpio_initialize();

  status = rtems_gpio_request_pin(GPIO_OUT, DIGITAL_OUTPUT, false, false, NULL);
  assert(status == RTEMS_SUCCESSFUL);

  pthread_mutex_init (&my_mutex, NULL);
  pthread_cond_init (&my_cond, NULL);
  
  if (pthread_create (&th, NULL, thread_process, NULL) < 0) {
    perror ("pthread_create");
    exit (1);
  }

  pause ();
}

#ifdef __rtems__

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE__MAXIMUM_FILE_DESCRIPTORS 10
#define CONFIGURE_UNLIMITED_OBJECTS
#define CONFIGURE_UNIFIED_WORK_AREAS
 
#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_MAXIMUM_POSIX_THREADS		2
#define CONFIGURE_POSIX_INIT_THREAD_TABLE
#define CONFIGURE_INIT

#include <rtems/confdefs.h>

#endif
