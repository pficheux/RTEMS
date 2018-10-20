3/3/2018
========

- Nouveau test RTEMS 5

* nécessaire d'installer python2.7-dev

$ sudo apt-get install python2.7-dev -> https://learnsom2day.wordpress.com/2015/04/21/getting-started-with-rtems-hello-word

* Installation compilateur croisé  (QEMU/i386) avec:

https://docs.rtems.org/branches/master/rsb/source-builder.html

$ git clone git://git.rtems.org/rtems-source-builder.git

$ cd rtems-source-builder/

$ ./source-builder/sb-check
RTEMS Source Builder - Check, 5 (d255e812abd5 modified)
Environment is ok

$ cd rtems/
$ ../source-builder/sb-set-builder --log=l-i386.txt --prefix=$HOME/RTEMS/rtems-i386 5/rtems-i386

* Compilation SDK

$ export PATH=$PATH:~/RTEMS/rtems-i386/bin

$ git clone git://git.rtems.org/rtems.git rtems_git
$ cd rtems_git

$ ./bootstrap

$ mkdir build && build
$ $HOME/RTEMS/rtems_git/configure --target=i386-rtems5 --disable-cxx --enable-rtemsbsp=pc386 --prefix=$HOME/RTEMS/target_i386

$ make -j 4
$ make install

* Test application

$ . ./set_env_i386.sh 
$ cd exemples/POSIX/helloworld
$ make
$ qemu-system-i386 -kernel o-optimize/helloworld.exe

idem avec timer POSIX

- Voir portage BBB -> https://devel.rtems.org/wiki/Developer/Projects/Open/ImproveBeagleBSP

20/10/2018
==========

- Test RTEMS 5 / RPi

* Production toolchain

$ cd rtems-source-builder/
$ ./source-builder/sb-check

$ cd rtems/
$ ../source-builder/sb-set-builder --log=l-arm.txt --prefix=$HOME/RTEMS/rtems-arm  5/rtems-arm

* Production SDK

$ export PATH=$PATH:~/RTEMS/rtems-arm/bin
$ cd rtems_git/
$ mkdir build_arm && cd build_arm
$ $HOME/RTEMS/rtems_git/configure --target=arm-rtems5 --disable-cxx --enable-rtemsbsp=raspberrypi --prefix=$HOME/RTEMS/target_rpi
$ make
$ make install

* Création set_env_rpi.sh

#!/bin/sh

export PATH=$PATH:~/RTEMS/rtems-arm/bin
export RTEMS_MAKEFILE_PATH=~/RTEMS/target_rpi/arm-rtems5/raspberrypi


* Exemple timer

$ . ~/RTEMS/set_env_rpi.sh 
$ make clean
$ make
$ cp o-optimize/timer.ralf /media/pierre/48BD-FC67/

-> OK




