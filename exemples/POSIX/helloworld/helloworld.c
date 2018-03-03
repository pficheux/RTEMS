#ifdef __rtems__
#include <bsp.h>
#endif
#include <stdio.h>
#include <stdlib.h>

#ifdef __rtems__
void *POSIX_Init() 
#else
void main (int ac, char **av)
#endif
{
  printf ("Hello RTEMS (POSIX)\n");
}

#ifdef __rtems__

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS		1
#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

#endif
