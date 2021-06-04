/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <dev/i2c/i2c.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <rtems/libcsupport.h>

typedef struct test_device test_device;

struct test_device {
  int (*transfer)(
    i2c_bus *bus,
    i2c_msg *msgs,
    uint32_t msg_count,
    test_device *dev
  );
};

typedef struct {
  test_device base;
  char buf[3];
} test_device_simple_read_write;


static const char bus_path[] = "/dev/i2c";

static void test(void)
{
  i2c_bus *bus;
  int rv;
  int fd;

  printf ("Allocate I2C\n");
  bus = i2c_bus_alloc_and_init(sizeof(*bus));
  assert(bus != NULL);

  printf ("Open I2C\n");
  fd = open(&bus_path[0], O_RDWR);
  assert(fd >= 0);

  printf ("Close I2C\n");
  rv = close(fd);
  assert(rv == 0);
}

static void Init(rtems_task_argument arg)
{
  test();
  exit (0);
}

#define CONFIGURE_MICROSECONDS_PER_TICK 2000

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 7

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_MAXIMUM_SEMAPHORES 1

#define CONFIGURE_INIT_TASK_STACK_SIZE (5 * RTEMS_MINIMUM_STACK_SIZE)

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
