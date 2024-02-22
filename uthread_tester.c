#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <uthread.h>
#define ARR_SIZE 10
#define TEST_ASSERT(assert)				\
do {									\
	printf("ASSERT: " #assert " ... ");	\
	if (assert) {						\
		printf("PASS\n");				\
	} else	{							\
		printf("FAIL\n");				\
		exit(1);						\
	}									\
} while(0)

int arr[] = {0,1,2,3,4,5,6,7,8,9};

/*used to set arr back to original values*/
void reset_arr(void)
{
   for(int i = 0; i<ARR_SIZE;i++) arr[i]=i ;
}

/* Simple function to increment arr*/
void increment(void *arg){
   (void) arg;
   for(int i = 0; i<ARR_SIZE; i++) arr[i] += 1;
}

/*copy of increment but creates thread*/
void threadIncrement(void *arg){
   (void) arg;
   uthread_create(increment, NULL);
   for(int i = 0; i<ARR_SIZE; i++) arr[i] += 1;
}
/*Tests the creation of threads*/
void test_create(void)
{
	fprintf(stderr, "*** TEST create ***\n");
        	
	TEST_ASSERT(uthread_run(true, increment, NULL) == 0);
	TEST_ASSERT(uthread_run(true, NULL, NULL) == -1);
	reset_arr();
}

/* Increment single thread test*/
void test_single_thread(void)
{
   fprintf(stderr, "*** TEST single thread ***\n");
   uthread_run(false, increment, NULL);
    
   for(int i = 0; i<ARR_SIZE;i++) TEST_ASSERT(arr[i] == i+1 );
   
   reset_arr();
}

/* Increment multithread test*/
void test_two_thread(void)
{
   fprintf(stderr, "*** TEST two thread ***\n");
   //creates a thread that creates a thread
   uthread_run(false, threadIncrement, NULL);  
					       
   for(int i = 0; i<ARR_SIZE;i++) TEST_ASSERT(arr[i] == i+2 );
   reset_arr();
}

/* Multiple single thread test */
void test_multiple_thread(void)
{
   fprintf(stderr, "*** TEST multiple thread ***\n");
   uthread_run(false, increment, NULL);
   uthread_run(false, increment, NULL);
   uthread_run(false, increment, NULL);
    
   for(int i = 0; i<ARR_SIZE;i++) TEST_ASSERT(arr[i] == i+3);
   reset_arr();

}

int main(void)
{
	test_create();
	printf("\n\n");
	test_single_thread();
	printf("\n\n");
	test_two_thread();
	printf("\n\n");
	test_multiple_thread();
	return 0;
}
