/*
 *  barrier1.c
 *
 *  Simple example showing how to start threads
 *  and use the barrier synchronization object
 *
 *  A barrier is a mechanism which allows a defined number of threads to synchronise or
 *  rendezvous at a given point in each thread. First the barrier must be initialised by calling
 *  pthread_barrier_init with the third parameter being the number of threads that will
 *  participate in the barrier. Each participating thread, when it reaches the desired
 *  synchronization point in its code it will call pthread_barrier_wait, which blocks the thread until
 *  all have called pthread_barrier_wait when all the threads will resume execution.
*/
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <sync.h>
#include <sys/neutrino.h>

pthread_barrier_t   barrier;        // the barrier synchronization object

void * thread1 (void *not_used)
{
    time_t  now;

    time(&now);
    printf("thread1 starting at        %s", ctime(&now));
    fflush(stdout);

    // do the computation
    // let's just do a sleep here
    sleep (3);

    time(&now);
    printf("thread1 reached barrier at %s", ctime(&now));
    fflush(stdout);

    pthread_barrier_wait(&barrier);

    // after this point, all three threads have completed.
    time(&now);
    printf("barrier in thread1 done at %s", ctime(&now));
}

void * thread2 (void *not_used)
{
    time_t  now;

    time(&now);
    printf("thread2 starting at        %s", ctime(&now));
    fflush(stdout);

    // do the computation
    // let's just do a sleep here
    sleep(6);

    time(&now);
    printf("thread2 reached barrier at %s", ctime(&now));

    pthread_barrier_wait(&barrier);
    // after this point, all three threads have completed.

    time (&now);
    printf("barrier in thread3 done at %s", ctime(&now));
}
void * thread3 (void *not_used)
{
    time_t  now;

    time(&now);
    printf("thread3 starting at        %s", ctime(&now));
    fflush(stdout);

    // do the computation
    // let's just do a sleep here
    sleep(10);

    time(&now);
    printf("thread3 reached barrier at %s", ctime(&now));

    pthread_barrier_wait(&barrier);
    // after this point, all three threads have completed.

    time (&now);
    printf("barrier in thread3 done at %s", ctime(&now));
}


int main(int argc, char *argv[])
{
	printf("Welcome to the QNX Momentics Barrier Example\n");

    time_t  now;

    // create a barrier object with a count of 3
    pthread_barrier_init (&barrier, NULL, 4);

    // start up two threads, thread1 and thread2
    pthread_create (NULL, NULL, thread1, NULL);
    pthread_create (NULL, NULL, thread2, NULL);
    pthread_create (NULL, NULL, thread3, NULL);

    // at this point, thread1 and thread2 are running
    usleep(1);

    // now wait for completion
    time(&now);
    printf("\nMain waiting at barrier at %s\n", ctime(&now));

    pthread_barrier_wait (&barrier); // wait here until barrier completes.

    // after this point, all three threads have completed.
    time(&now);
    printf("barrier in main () done at %s", ctime(&now));

    fflush(stdout);

	printf("\nMain Terminating....\n");
	return EXIT_SUCCESS;
}
