#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"
#include "queue.h"

enum state
{
   READY,
   RUNNING,
   BLOCKED,
   DEAD
};

/**Global Vars**/
queue_t schedule;
struct uthread_tcb *current;  
/**End**/


struct uthread_tcb {
   enum state state;  
   void *stack;  
   uthread_ctx_t *context;  //saves context of where thread left off
};

struct uthread_tcb *uthread_current(void)
{
	return current;
}

// Function to perform thread scheduling (round-robin)
static void uthread_schedule(void)
{
    struct uthread_tcb *prev_thread = uthread_current();
    struct uthread_tcb *next_thread;

    //disable preempt to ensure safe switch
    //gets re-enabled upon exit
    //also gets re-enabled upon next thread
    //creation
    preempt_disable();
    int deq_err = queue_dequeue(schedule, (void**)&next_thread);

    if(deq_err == -1) return;

    //switch contexts only if thread is ready
    if (next_thread != NULL && next_thread -> state == READY)
    {
        current = next_thread;
	current -> state = RUNNING;
        uthread_ctx_switch(prev_thread->context,current->context);
    }
    preempt_enable();
}

// Yield execution and invoke scheduler
void uthread_yield(void)
{
    if (current == NULL) return;

    if(current->state == RUNNING){ //stop running thread to give up cpu 
       current->state = READY;
       queue_enqueue(schedule,current);
    }
    
    uthread_schedule();
}

// Exit the currently running thread and invoke scheduler
void uthread_exit(void)
{
   if(current == NULL){
     printf("Exit if\n");
	   return;
   }

   preempt_disable(); //disable to ensure atomic destruction
   current -> state = DEAD;  //kill process
   uthread_ctx_destroy_stack(current->stack);  //free stack
   free(current -> context);  
   preempt_enable();

   uthread_schedule();
}

//creates a new thread
int uthread_create(uthread_func_t func, void *arg) 
{
    /*Allocating memory for thread*/
    struct uthread_tcb *new_thread = 
          (struct uthread_tcb *)malloc(sizeof(struct uthread_tcb));
    new_thread -> context = 
          (uthread_ctx_t *)malloc(sizeof(uthread_ctx_t));

    if (new_thread == NULL || new_thread->context == NULL){
        perror("uthread_create");
        return -1;
    }
    
    preempt_disable();
    // Allocate stack space for the new thread
    new_thread -> state = READY;
    new_thread -> stack = uthread_ctx_alloc_stack();

    if(uthread_ctx_init(new_thread->context, new_thread->stack,func,arg)==-1 
         || new_thread->stack == NULL )
       	return -1;

    // Enqueue the new thread in the thread queue
    queue_enqueue(schedule, new_thread);
    preempt_enable();
    return 0;
}


// Initialize the multithreading library and start the first thread
int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
    // Create the thread queue
    schedule = queue_create();
    if (schedule == NULL || func == NULL) return -1;

    // Create the idle thread and start the scheduler
    struct uthread_tcb *idle_thread = 
          (struct uthread_tcb *)malloc(sizeof(struct uthread_tcb));
    if (idle_thread == NULL) return -1;

    //creates idle thread to keep keep loop looping
    idle_thread -> state   = RUNNING;
    idle_thread -> stack   = uthread_ctx_alloc_stack();
    idle_thread -> context = (uthread_ctx_t*)malloc(sizeof(uthread_ctx_t));
    if(idle_thread->stack == NULL|| idle_thread->context == NULL)
       return -1;
    if(uthread_ctx_init(idle_thread->context, idle_thread->stack, func, arg)==-1)
       return -1;
    current = idle_thread;
  
    preempt_start(preempt);
    //creates second thread with the func/arg passed
    if(uthread_create(func, arg) == -1) return -1;
    // Continue scheduling threads until no more runnable threads
    while (queue_length(schedule)!=0)
    {
        uthread_yield();
    }
    // Destroy the thread queue
    queue_destroy(schedule);
    preempt_stop();
    return 0;
}


// Block the currently running thread and invoke scheduler
void uthread_block(void)
{
    if (current == NULL) return;
    current -> state = BLOCKED;
    uthread_schedule();
}

// Unblock the specified thread and invoke scheduler
void uthread_unblock(struct uthread_tcb *uthread)
{
    if (uthread == NULL || uthread->state != BLOCKED) return;
    uthread->state = READY;
    queue_enqueue(schedule, uthread);
    
}
