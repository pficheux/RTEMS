/*
 * RTEMS dummy driver
 */
#include <rtems.h>
#include <bsp.h>
#include "dummy_drv.h"

static char initialized;
static int value;

rtems_device_driver dummy_drv_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
)
{
  rtems_device_driver status;

  if ( !initialized ) {
    initialized = 1;

    status = rtems_io_register_name(
      "/dev/dummy_drv",
      major,
      (rtems_device_minor_number) 0
    );

    if (status != RTEMS_SUCCESSFUL)
      rtems_fatal_error_occurred(status);
  }

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver dummy_drv_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
)
{
  printk ("%s\n", __FUNCTION__);
  return RTEMS_SUCCESSFUL;
}

rtems_device_driver dummy_drv_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
)
{
  printk ("%s\n", __FUNCTION__);
  return RTEMS_SUCCESSFUL;
}

rtems_device_driver dummy_drv_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
)
{
  int v, cmd;
  rtems_libio_ioctl_args_t *args = pargp;

  v = (int)(args->buffer);
  cmd = (int)(args->command);

  switch (cmd) {
  case DUMMY_SET :
    //    printk ("%s: SET\n", __FUNCTION__);
    value = v;
    break;

  case DUMMY_GET :
    //    printk ("%s: GET\n", __FUNCTION__);
    args->ioctl_return = value;
    return RTEMS_SUCCESSFUL;

  default: 
    printk ("dummy_drv_control: unknown cmd %x\n", cmd); 

    args->ioctl_return = -1;
    return RTEMS_UNSATISFIED;
  }
    
  args->ioctl_return = 0;

  return RTEMS_SUCCESSFUL;
}
