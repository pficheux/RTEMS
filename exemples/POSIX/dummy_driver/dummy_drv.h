#ifndef __DUMMY_DRV_DRIVER_h
#define __DUMMY_DRV_DRIVER_h

#include <rtems/libio.h>


/* Driver cmds */
#define DUMMY_SET     0
#define DUMMY_GET     1

#ifdef __cplusplus
extern "C" {
#endif

#define DUMMY_DRV_DRIVER_TABLE_ENTRY \
  { dummy_drv_initialize, dummy_drv_open, dummy_drv_close, NULL, \
    NULL, dummy_drv_control }

rtems_device_driver dummy_drv_initialize(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver dummy_drv_open(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver dummy_drv_close(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver dummy_drv_control(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
