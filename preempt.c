#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100
#define MICRO2_SEC 1000000

static struct itimerval newTimer, oldTimer;
struct sigaction newSA, oldSA;

void alarm_handler(int signum)
{
  // Signal handler for the SIGALRM signal, triggers a context switch
  if(signum == SIGVTALRM) uthread_yield();
}	

void preempt_disable(void)
{
// Disable preemption by blocking the SIGALRM signa
   sigemptyset(&newSA.sa_mask);
   sigaddset(&newSA.sa_mask, SIGVTALRM);
   sigprocmask(SIG_BLOCK, &newSA.sa_mask, &oldSA.sa_mask);
}

void preempt_enable(void)
{
    // Enable preemption by unblocking the SIGALRM signal
    sigemptyset(&newSA.sa_mask);
    sigaddset(&newSA.sa_mask, SIGVTALRM);
    sigprocmask(SIG_UNBLOCK, &newSA.sa_mask,&oldSA.sa_mask);
   
    // Set the signal handler for SIGALRM
    newSA.sa_handler = alarm_handler;
    newSA.sa_flags = 0;
    sigaction(SIGVTALRM, &newSA, &oldSA);
}

void preempt_start(bool preempt)
{
    if (preempt)
    {
	preempt_enable();
        // Set up the timer to trigger the SIGVTALRM signal at HZ frequency
        newTimer.it_value.tv_sec = 0;
        newTimer.it_value.tv_usec = MICRO2_SEC/HZ;
        newTimer.it_interval = newTimer.it_value;

        setitimer(ITIMER_VIRTUAL, &newTimer, &oldTimer);
    }else{
       preempt_disable();
    }

}

void preempt_stop(void)
{
    // Stop the timer and disable preemption
      newSA = oldSA;
      newTimer = oldTimer;
      preempt_disable();

}
