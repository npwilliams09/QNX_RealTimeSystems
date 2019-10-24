#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>

#define MY_PULSE_CODE   _PULSE_CODE_MINAVAIL

enum states { State0, State1, State2, State3, State4, State5, State6};

typedef union
{
	struct _pulse   pulse;
    // your other message structures would go here too
} my_message_t;

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
			sleep(2);
			timer_settime(timer_id, 0, &itime, NULL);
			while(*curr == State2){
				rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);
				if (rcvid == 0) // this process
					   {
						   // received a pulse, now check "code" field...
						   if (msg.pulse.code == MY_PULSE_CODE) // we got a pulse
							{
							   *curr = State3;
							}
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
			sleep(2);
			*curr = State6;
			break;
		//NSY
		case State6:
			printf("EWR_NSY\n");
			sleep(1);
			*curr = State1;
			break;
	}
}


int main(int argc, char *argv[])
{
	struct sigevent         event;
	struct itimerspec       itime;
	timer_t                 timer_id;
	int                     chid;
	int                     rcvid;
	my_message_t            msg;

	chid = ChannelCreate(0); // Create a communications channel

	event.sigev_notify = SIGEV_PULSE;
	event.sigev_coid = ConnectAttach(ND_LOCAL_NODE, 0, chid, _NTO_SIDE_CHANNEL, 0);
	// create a connection back to ourselves for the timer to send the pulse on
	if (event.sigev_coid == -1)
	{
	   printf(stderr, "%s:  couldn't ConnectAttach to self!\n", progname);
	   perror(NULL);
	   exit(EXIT_FAILURE);
	}
	//event.sigev_priority = getprio(0);  // this function is depreciated in QNX 700
	struct sched_param th_param;
	pthread_getschedparam(pthread_self(), NULL, &th_param);
	event.sigev_priority = th_param.sched_curpriority;    // old QNX660 version getprio(0);

	event.sigev_code = MY_PULSE_CODE;

	// create the timer, binding it to the event
	if (timer_create(CLOCK_REALTIME, &event, &timer_id) == -1)
	{
	   printf (stderr, "%s:  couldn't create a timer, errno %d\n", progname, errno);
	   perror (NULL);
	   exit (EXIT_FAILURE);
	}
	itime.it_value.tv_sec = 1;			  // 1 second   // 500 million nsecs = .5 secs
	itime.it_interval.tv_sec = 0;          // 1 second // 500 million nsecs = .5 secs

	printf("Fixed Sequence Traffic Lights State Machine Example\n");
	int Runtimes=30, counter = 0;
	enum states CurrentState = State0;

	while (counter < Runtimes)
	{
		SingleStep_TrafficLight_SM( &CurrentState, &master); // pass address
		counter++;
	}
}

