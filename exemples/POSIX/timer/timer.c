#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>

void *threadfn1(void *p)
{
  while(1){
    printf("thread1 ID= %x\n", pthread_self());
    sleep(2);
  }
  return 0;
}
 
void *threadfn2(void *p)
{
  while(1){
    printf("thread2 ID= %x\n", pthread_self());
    sleep(2);
  }
  return 0;
}
 
void *threadfn3(void *p)
{
  while(1){
    printf("thread3 ID= %x\n", pthread_self());
    sleep(2);
  }
  return 0;
}

void got_signal (int sig)
{
  printf ("Got signal %d ID= %x\n", sig, pthread_self());
}


#ifdef __rtems__
void *POSIX_Init() 
#else
int main()
#endif
{
  timer_t myTimer;
  struct sigaction sig;
  struct itimerspec ti, ti_old;
  struct sigevent event;
  sigset_t mask;
  pthread_t t1,t2,t3;
  
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
  ti.it_interval.tv_sec = 1;
  ti.it_interval.tv_nsec = 0;

  timer_settime(myTimer, 0, &ti, &ti_old);

  pthread_create(&t1,NULL,threadfn1,NULL);
  pthread_create(&t2,NULL,threadfn2,NULL);
  pthread_create(&t3,NULL,threadfn3,NULL);

  printf ("main thread ID= %x\n", pthread_self());
  
  while (1)
    pause ();
}

#ifdef __rtems__

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_TIMERS          1
#define CONFIGURE_MAXIMUM_POSIX_THREADS		4
#define CONFIGURE_POSIX_INIT_THREAD_TABLE
#define CONFIGURE_INIT
#include <rtems/confdefs.h>

#endif
