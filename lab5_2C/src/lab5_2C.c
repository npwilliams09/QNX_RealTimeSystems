#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/iofunc.h>
#include <sys/netmgr.h>

#define BUF_SIZE 100
#define FILEADDR "/tmp/s244/servInfo.txt"

enum states { State0, State1, State2, State3, State4, State5, State6};

typedef struct
{
	struct _pulse hdr; // Our real data comes after this header
	int ClientID; // our data (unique id from client)
    char data;     // our data
} my_data;

typedef struct
{
	struct _pulse hdr;  // Our real data comes after this header
    char buf[BUF_SIZE]; // Message we send back to clients to tell them the messages was processed correctly.
} my_reply;

typedef struct{
	char ch;
	pthread_mutex_t mutex;  // needs to be set to PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t condvar; // needs to be set to PTHREAD_COND_INITIALIZER;
}app_data;

void SingleStep_TrafficLight_SM(enum states * curr, int channel){

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
				my_data msg;
				int rcvid = MsgReceive(channel, &msg, sizeof(msg), NULL);
				MsgReply( rcvid, EOK, NULL, 0 );
				printf("received %c\n",msg.data);
				if(msg.data == 'n'){
					*curr = State3;
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
				my_data msg;
				int rcvid = MsgReceive(channel, &msg, sizeof(msg), NULL);
				MsgReply( rcvid, EOK, NULL, 0 );
				printf("received %c\n",msg.data);
				if(msg.data == 'e'){
					*curr = State6;
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

	//get ids
	//read file
	char line[256];
	FILE * text = fopen(FILEADDR,"r");
	fgets(line, sizeof(line), text);
	//set server ID
	int serverPID;
	sscanf(line, "%d", &serverPID);
	fgets(line, sizeof(line), text);
	//set CHID
	int serverCHID;
	sscanf(line, "%d", &serverCHID);
	fclose(text);

	my_data msg;
	my_reply reply;

	msg.ClientID = 500;

	int server_coid;

	printf("   --> Trying to connect (server) process which has a PID: %d\n",   serverPID);
	printf("   --> on channel: %d\n\n", serverCHID);

	// set up message passing channel
	server_coid = ConnectAttach(ND_LOCAL_NODE, serverPID, serverCHID, _NTO_SIDE_CHANNEL, 0);
	if (server_coid == -1)
	{
		printf("\n    ERROR, could not connect to server!\n\n");
	}


	printf("Connection established to process with PID:%d, Ch:%d\n", serverPID, serverCHID);

	// We would have pre-defined data to stuff here
	msg.hdr.type = 0x00;
	msg.hdr.subtype = 0x00;

	char temp;
	while(1){
		scanf(" %c", &temp);
		msg.data=temp;
		if (MsgSend(server_coid, &msg, sizeof(msg), &reply, sizeof(reply)) == -1)
		{
			printf(" Error data '%d' NOT sent to server\n", msg.data);
				// maybe we did not get a reply from the server
			break;
		}
		else
		{ // now process the reply
			printf("   -->Reply is: 'Acknowledged'\n", reply.buf);
		}
	}
	// Close the connection
	printf("\n Sending message to server to tell it to close the connection\n");
	ConnectDetach(server_coid);
	return 0;
}

int main(void) {
	int serverPID=0, chid=0; 	// Server PID and channel ID

	serverPID = getpid(); 		// get server process ID

	// Create Channel
	chid = ChannelCreate(_NTO_CHF_DISCONNECT);
	if (chid == -1)  // _NTO_CHF_DISCONNECT flag used to allow detach
	{
		printf("\nFailed to create communication channel on server\n");
		return EXIT_FAILURE;
	}

	//write to file
	FILE * text = fopen(FILEADDR,"w");
	fprintf(text,"%d\n",serverPID);
	fprintf(text,"%d\n",chid);
	fclose(text);

	printf("Server Listening for Clients on:\n");
	printf("These printf statements can be removed when the myServer.info file is implemented\n");
	printf("  --> Process ID   : %d \n", serverPID);
	printf("  --> Channel ID   : %d \n\n", chid);

	/*
	 *   Your code here to write this information to a file at a known location so a client can grab it...
	 *
	 *   The data should be written to a file like:
	 *   /tmp/myServer.info
	 *	 serverPID  (first line of file)
	 *	 Channel ID (second line of file)
	 */



	//my_reply replymsg; 			// replymsg structure for sending back to client
	//replymsg.hdr.type = 0x01;
	//replymsg.hdr.subtype = 0x00;

	printf("Fixed Sequence Traffic Lights State Machine Example\n");
	int Runtimes=16, counter = 0;
	enum states CurrentState = State0;

	pthread_t th1;
	pthread_create(&th1, NULL, keyRead, NULL);

	while (counter < Runtimes)
	{
		SingleStep_TrafficLight_SM(&CurrentState, chid); // pass address
		counter++;
	}

}
