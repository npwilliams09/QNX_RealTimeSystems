#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>

enum states { State0, State1, State2, State3, State4, State5, State6};

typedef struct{
	char ch;
	pthread_mutex_t mutex;  // needs to be set to PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t condvar; // needs to be set to PTHREAD_COND_INITIALIZER;
}app_data;

char keyCheck(app_data * kcd){
	while(kcd->ch == 0){
		pthread_cond_wait(&kcd->condvar, &kcd->mutex);
	}

	pthread_mutex_lock(&kcd->mutex);
	char temp = kcd -> ch;
	kcd -> ch = 0;
	pthread_mutex_unlock(&kcd->mutex);
	pthread_cond_signal(&kcd->condvar);

	return temp;
}

void SingleStep_TrafficLight_SM(enum states * curr, app_data * std){

	switch(*curr){
		//initial state
		case State0:
			printf("Init: EWR_NSR\n");
			*curr = State1;
			break;

		//both red
		case State1:
			printf("EWR_NSR\n");
			sleep(1);
			*curr = State2;
			break;

		//EWG
		case State2:
			printf("EWG_NSR\n");
			if (keyCheck(std) == 'n'){
				*curr = State3;
			}
			break;

		//EWY
		case State3:
			printf("EWY_NSR\n");
			sleep(1);
			*curr = State4;
			break;

		//both red
		case State4:
			printf("EWR_NSR\n");
			sleep(1);
			*curr = State5;
			break;
		//NSG
		case State5:
			printf("EWR_NSG\n");
			if (keyCheck(std) == 'e'){
				*curr = State6;
			}
			break;
		//NSY
		case State6:
			printf("EWR_NSY\n");
			sleep(1);
			*curr = State1;
			break;
	}
}

void * keyRead (void * data){
	app_data * krd = (app_data *) data;
	char temp;
	while(1){
		while(krd->ch != 0){
			 pthread_cond_wait(&krd->condvar, &krd->mutex);
		}

		scanf(" %c", &temp);

		//critical
		pthread_mutex_lock(&krd->mutex);
		krd->ch = temp;
		pthread_mutex_unlock(&krd->mutex);
		pthread_cond_signal(&krd->condvar);

	}
}

int main(int argc, char *argv[])
{
	printf("Fixed Sequence Traffic Lights State Machine Example\n");
	int Runtimes=30, counter = 0;
	enum states CurrentState = State0;
	app_data master = {0,PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER};

	pthread_t th1;
	pthread_create(&th1, NULL, keyRead, &master);

	while (counter < Runtimes)
	{
		SingleStep_TrafficLight_SM( &CurrentState, &master); // pass address
		counter++;
	}
}

