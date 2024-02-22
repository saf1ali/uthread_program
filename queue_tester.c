#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <queue.h>
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
/* Create */
void test_create(void)
{
	fprintf(stderr, "*** TEST create ***\n");

	TEST_ASSERT(queue_create() != NULL);
}

/* Enqueue/Dequeue simple */
void test_queue_simple(void)
{
	int data = 3, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST queue_simple ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data);
}

/* Dequeue NULL ptrs */
void test_dequeue_null(void)
{
   queue_t q;
   void *dptr;
   fprintf(stderr, "*** TEST dequeue_null ***\n");
   q = queue_create();
   TEST_ASSERT(queue_dequeue(q,NULL)==-1);
   TEST_ASSERT(queue_dequeue(NULL,(void **)&dptr)==-1);
}

/* Enqueue NULL ptrs */
void test_enqueue_null(void)
{
        int data = 3;
        queue_t q;
	fprintf(stderr, "*** TEST enqueue_null ***\n");

	q = queue_create(); 
	TEST_ASSERT(queue_enqueue(q,NULL));
	TEST_ASSERT(queue_enqueue(NULL,&data));
}

/* Method used to test iterator test */
static void iterator_inc(queue_t q, void *data)
{
    int *a = (int*)data;

    if (*a == 42)
        queue_delete(q, data);
    else
        *a += 1;
}

/** TEST ITERATOR **/
//I STOLE THIS FROM THE PROJECT 2 HTML
void test_iterator(void)
{
   queue_t q;
   int data[] = {1,2,3,4,5,42,6,7,8,9};
   size_t i;
   int *dptr;
   int ref =2;
   int queueLen;

    fprintf(stderr, "*** TEST ITERATOR ***\n");
    q = queue_create();
    for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
        queue_enqueue(q, &data[i]);

    /* Increment every item of the queue, delete item '42' */
    queue_iterate(q, iterator_inc);
    TEST_ASSERT(queue_length(q) == 9); 
    queueLen = queue_length(q);

    for(int i = 0; i<queueLen; i++){ 
       queue_dequeue(q,(void **)&dptr);
       TEST_ASSERT(*dptr == ref);
       ref++;
    }

}

/* Deletes tail node and enqueues another node 
 * ensures that tail node is being relinked
 * properly                                  */
void test_delete_tail()
{
   queue_t q;
   int data[] = {1,2,3,4,5,6,7,8,42};
   int insertInt = 10;
   size_t i;
   int *dptr;
   int ref =2;
   int queueLen;

    fprintf(stderr, "*** TEST TAIL DELETION ***\n");
    q = queue_create();
    for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
        queue_enqueue(q, &data[i]);

    /* Increment every item of the queue, delete item '42' */
    queue_iterate(q, iterator_inc);
    queue_enqueue(q, &insertInt );
    queueLen = queue_length(q);

    for(int i = 0; i<queueLen; i++){ 
       queue_dequeue(q,(void **)&dptr);
       TEST_ASSERT(*dptr == ref); 
       ref++;
    }

}

int main(void)
{
	test_create();
	printf("\n\n");
	test_queue_simple();
	printf("\n\n");
	test_dequeue_null();
	printf("\n\n");
	test_enqueue_null();
	printf("\n\n");
	test_iterator();
	printf("\n\n");
	test_delete_tail();

	return 0;
}
