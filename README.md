libuthread: User-Level Threading Library
libuthread is a user-level threading library that leverages ucontext to provide an interface for creating user-level threads. It includes support for preemption, queues, and semaphores.

APIs:
Queue:

Implementation: Utilizes a linked list to efficiently manage user-threads.
Testing: Extensive tests include checking for NULL arguments, iterating through the queue, and testing the deletion of the tail node.
Uthread:

Implementation: Uses ucontext and the queue API to create and schedule threads.
Testing: Ensures proper creation of idle and user threads, along with safeguards against NULL functions. Additional tests verify the functionality of single, two, and multiple threads.
Semaphore:

Implementation: Supports blocking certain threads to prevent race conditions.
Testing: Verifies the creation of semaphores, blocking threads on sem_down(), and the associated functionality to avoid race conditions.
Preemption:

Implementation: Implements preemption by forcing threads to yield after a set time interval.
Testing: Ensures proper preemption functionality by testing with semaphore scenarios and a custom test called preempt_test(). Disabling and re-enabling preemption is crucial in sensitive areas, such as changing semaphore values and thread scheduling.
