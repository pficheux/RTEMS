/*
 * RTEMS GPIO driver for RPi
 */
#include <rtems.h>
#include <bsp.h>
#include "rpi_gpio.h"

static char initialized;

rtems_device_driver rpi_gpio_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
)
{
  rtems_device_driver status;

  if ( !initialized ) {
    initialized = 1;

    status = rtems_io_register_name(
      "/dev/rpi_gpio",
      major,
      (rtems_device_minor_number) 0
    );

    if (status != RTEMS_SUCCESSFUL)
      rtems_fatal_error_occurred(status);
  }

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver rpi_gpio_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
)
{
  return RTEMS_SUCCESSFUL;
}

rtems_device_driver rpi_gpio_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
)
{
  return RTEMS_SUCCESSFUL;
}

rtems_device_driver rpi_gpio_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
)
{
  int n, cmd;
  rtems_libio_ioctl_args_t *args = pargp;

  n = (int)(args->buffer);
  cmd = (int)(args->command);

  printk ("rpi_gpio_control: cmd %x OK !\n", cmd); 
  
  switch (cmd) {
  case RPI_GPIO_SET :
    break;

  case RPI_GPIO_CLR :
    break;

  case RPI_GPIO_OUT :
    break;

  case RPI_GPIO_IN :
    break;

  case RPI_GPIO_READ :
    return RTEMS_SUCCESSFUL;

  default: 
    printk ("rpi_gpio_control: unknown cmd %x\n", cmd); 

    args->ioctl_return = -1;
    return RTEMS_UNSATISFIED;
  }
    
  args->ioctl_return = 0;

  return RTEMS_SUCCESSFUL;
}
