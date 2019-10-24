#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <mqueue.h>
#include <string.h>

#define  MESSAGESIZE 2
#define Q_FLAGS O_RDWR | O_CREAT | O_EXCL
#define Q_Mode S_IRUSR | S_IWUSR

enum states { State0, State1, State2, State3, State4, State5, State6};

const char * MqueueLocation = "/dev/mqueue/244_queue";	/* will be located /dev/mqueue/test_queue  */


typedef struct{
	char ch[MESSAGESIZE];
	pthread_mutex_t mutex;  // needs to be set to PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t condvar; // needs to be set to PTHREAD_COND_INITIALIZER;
}app_data;


void SingleStep_TrafficLight_SM(enum states * curr, app_data * std){
	char buf[MESSAGESIZE];
	mqd_t mainq = mq_open(MqueueLocation, O_RDONLY);
	struct	mq_attr	attr;
	mq_getattr(mainq, &attr);

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
			while(*curr == State2){
				if(mainq != -1){
					while(mq_receive(mainq, buf, attr.mq_msgsize, NULL)> 0){
						printf("%s received\n",buf);
						if(!strcmp(buf,"n")){
							pthread_mutex_lock(&std->mutex);
							strcpy(std->ch,buf);
							pthread_mutex_unlock(&std->mutex);
							*curr = State3;
							break;
						}
					}
				}else{
					printf("queue error\n");
				}
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
			while(*curr == State5){
				while(mq_receive(mainq, buf, attr.mq_msgsize, NULL)> 0){
					printf("%s received\n",buf);
					if(!strcmp(buf,"e")){
						pthread_mutex_lock(&std->mutex);
						strcpy(std->ch,buf);
						pthread_mutex_unlock(&std->mutex);
						*curr = State6;
						break;
					}
				}
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

void * keyRead (){
	struct  mq_attr  attr;
	attr.mq_maxmsg = 100;
	attr.mq_msgsize = MESSAGESIZE;
	attr.mq_flags = 0;
	attr.mq_curmsgs = 0;
	attr.mq_sendwait = 0;
	attr.mq_recvwait = 0;
	struct mq_attr * my_attr = &attr;
	mqd_t	qd = mq_open(MqueueLocation, Q_FLAGS, Q_Mode, my_attr);
	char temp[2];
	while(1){
		scanf(" %c", &temp);
		fflush(stdin);
		mq_send(qd, temp, MESSAGESIZE, 0);
		printf("%s sent\n",temp);
	}
}

int main(int argc, char *argv[])
{
	//message queue config


	printf("Fixed Sequence Traffic Lights State Machine Example\n");
	int Runtimes=30, counter = 0;
	enum states CurrentState = State0;
	app_data master = {"", PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER};

	pthread_t th1;
	pthread_create(&th1, NULL, keyRead, NULL);

	while (counter < Runtimes)
	{
		SingleStep_TrafficLight_SM( &CurrentState, &master); // pass address
		counter++;
	}
}

