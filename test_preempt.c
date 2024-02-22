#include <stdio.h>
#include <stdlib.h>
#include <sem.h>
#include <limits.h>
#include <uthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

volatile bool threadFlag = true;
static void thread2(void *arg)
{
	(void)arg;
	threadFlag = false;
	printf("Freeing thread1...\n");
}

static void thread1(void *arg)
{

    (void)arg;
    uthread_create(thread2, NULL);
    while(threadFlag);
    printf("I AM FREEE!!!!\n");

}

int main(void)
{
	uthread_run(true, thread1, NULL);
	return 0;
}
