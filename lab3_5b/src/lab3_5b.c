/*
 * Modified Conditional Variables Example
 *
 * Very simple producer-consumer example
 * showing use of condition variables
 *
*/
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <fcntl.h>

#define BUFFER_SIZE 10

pthread_barrier_t   barrier;


typedef struct
{
	 char buffer[BUFFER_SIZE];
	 int readFinished;
	 pthread_mutex_t mutex;  // needs to be set to PTHREAD_MUTEX_INITIALIZER;
}app_data;

void *consumer(void *Data)
{
    printf("->Consumer thread started...\n");

    app_data *tdc = (app_data*) Data;
    char x;
    int i = 0;
    while(!tdc->readFinished)
	{
		//critical start
		pthread_mutex_lock(&tdc->mutex);
		x = tdc->buffer[i%10];
		pthread_mutex_unlock(&tdc->mutex);
		//critical finished
		printf("consumer:  got data from producer : %c\n", x);
		sleep(1);
    	if (i == 14){
			pthread_mutex_lock(&tdc->mutex);
			tdc->readFinished = 1;
			pthread_mutex_unlock(&tdc->mutex);
    	}
    	i++;
    }
    printf("Consumer Finished\n");
    pthread_barrier_wait(&barrier);
    return 0;
}

void *producer(void *Data)
{
    printf("->Producer thread started...\n");
    char y;
    // initialize the fake data:
    app_data *tdp = (app_data*) Data;
    for(int j = 0; j < 15; j++){
    	y = 'a' + j;
    	//critical start
		pthread_mutex_lock(&tdp->mutex);
		tdp->buffer[j%10] = y;
		pthread_mutex_unlock(&tdp->mutex);
		//critical finished
		printf ("producer:  Data(%c) from simulated h/w ready\n",y);
		if(j%2){
			sleep(1);
		}
    }

    pthread_barrier_wait(&barrier);
    return 0;
}


int main(int argc, char *argv[])
{
	printf("Starting Consumer and Producer\n");

	// initialize the data, mutex and condvar
	app_data data;
	data.readFinished = 0;
	pthread_mutex_init(&data.mutex,NULL);
	void *retval;


	//init barrier to synchronise thread finish
	pthread_barrier_init (&barrier, NULL, 3);

    printf("Starting consumer/producer example...\n");

	//Set producer thread, priority 5
	pthread_t th1;
	pthread_attr_t th1_attr;
	struct sched_param th1_param;
	pthread_attr_init (&th1_attr);
	pthread_attr_setschedpolicy(&th1_attr, SCHED_RR);
	th1_param.sched_priority = 5;
	pthread_attr_setschedparam (&th1_attr, &th1_param);
	pthread_attr_setinheritsched (&th1_attr, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setstacksize (&th1_attr, 8000);

	//Set consumer thread, priority 2
	pthread_t th2;
	pthread_attr_t th2_attr;
	struct sched_param th2_param;
	pthread_attr_init (&th2_attr);
	pthread_attr_setschedpolicy(&th2_attr, SCHED_RR);
	th2_param.sched_priority = 2;
	pthread_attr_setschedparam (&th2_attr, &th2_param);
	pthread_attr_setinheritsched (&th2_attr, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setstacksize (&th2_attr, 8000);

	//create threads
	pthread_create(&th1, &th1_attr, producer, &data);
	pthread_create(&th2, &th2_attr, consumer, &data);




    //wait for threads
    pthread_barrier_wait(&barrier);

    pthread_join (th1, &retval);
    pthread_join (th2, &retval);

	printf("\nMain Terminating....");
	return EXIT_SUCCESS;
}
