#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "private.h"

#define FAILED -1
#define SUCCESS 0

struct semaphore {
   int count;    //used to keep track of resources
   struct uthread_tcb *sems_thread;    //associates a semaphor with a thread
};

sem_t sem_create(size_t count)
{
   //allocate mem and set count
   sem_t new_sem;
   if( (new_sem = (sem_t)malloc(sizeof(struct semaphore))) == NULL)
      return NULL;
   new_sem -> count = count;
   return new_sem;
}

int sem_destroy(sem_t sem)
{
   free(sem);
   return SUCCESS;
}

int sem_down(sem_t sem)
{
   //associates sem with thread that calls sems down 
   //blocks if resource is being used
   if(sem == NULL) return FAILED;

   sem -> sems_thread = uthread_current();

   preempt_disable();
   while(sem -> count == 0){
      uthread_block();
   }
   //resource in use   decrement
   sem -> count -= 1;
   preempt_enable();
   return SUCCESS;
}

int sem_up(sem_t sem)
{
   //unblocks thread and increments count
   //resource available	
   if(sem == NULL) return FAILED;
   preempt_disable();
   sem -> count += 1;
   uthread_unblock(sem->sems_thread);
   preempt_enable();
   
   return SUCCESS;
}


