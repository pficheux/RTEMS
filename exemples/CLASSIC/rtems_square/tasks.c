#include "system.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <rtems/error.h>
#include <bsp/gpio.h>

// To print statistics
//#define PRINT_STATS

//#define GPIO_OUT  BBB_P8_7
#define GPIO_OUT 4

/* CPU usage and Rate monotonic manger statistics */
#include "rtems/cpuuse.h"

// Periods for the various tasks
#define PERIOD_TASK_RATE_MONOTONIC     1000 // 1 ms

//
// Rate Monotonic Scheduling
//
rtems_task Task_Rate_Monotonic_Period(
				      rtems_task_argument unused
				      )
{
  rtems_time_of_day time;
  rtems_status_code status;
  uint32_t          ticks_since_boot;
  rtems_name        my_period_name; 
  rtems_id          RM_period;
  bool              is_RM_created = 0;
  uint32_t          count;
  int nibl = 0, sc;

  count = 0;

  /* Initializes the GPIO API */
  rtems_gpio_initialize();

  /* Allocate GPIO */
  sc = rtems_gpio_request_pin(GPIO_OUT, DIGITAL_OUTPUT, false, false, NULL);
  assert(sc == RTEMS_SUCCESSFUL);


  // Init RMS
  my_period_name = rtems_build_name( 'P', 'E', 'R', '1' );
  status = rtems_rate_monotonic_create( my_period_name, &RM_period );
  if( RTEMS_SUCCESSFUL != status ) {
    printf("RM failed with status: %d\n", status);
    exit(1);
  }
  
  while( 1 ) {
    count++;

    // write to GPIO
    if (count % 2)
      rtems_gpio_set (GPIO_OUT);
    else
      rtems_gpio_clear(GPIO_OUT);

#idef PRINT_STATS
    // Print stats ?
    if (count % PERIOD_TASK_RATE_MONOTONIC == 0) {
      ticks_since_boot = rtems_clock_get_ticks_since_boot();
      printf(" - Ticks since boot: %" PRIu32 " %d %d\n", ticks_since_boot, rtems_clock_get_ticks_per_second(), count);
      rtems_rate_monotonic_report_statistics();
      printf ("t = %d\n", rtems_clock_get_ticks_per_second() / PERIOD_TASK_RATE_MONOTONIC);
    }
#endif
    
    // Block until RM period has expired
    status = rtems_rate_monotonic_period(
					 RM_period,
					 rtems_clock_get_ticks_per_second() / PERIOD_TASK_RATE_MONOTONIC
					 );

    // Overrun ?
    if( RTEMS_SUCCESSFUL != status ) {
      if( RTEMS_TIMEOUT != status ) {
	printf("RM missed period!\n");
      }

      printf("RM failed with status: %d\n", status);
      exit(1);
    }
  }
}

