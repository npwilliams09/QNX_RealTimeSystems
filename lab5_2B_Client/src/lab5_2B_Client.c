#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/iofunc.h>
#include <sys/netmgr.h>
#include <unistd.h>

#define BUF_SIZE 100
#define FILEADDR "/tmp/s244/servInfo.txt"

typedef struct
{
	struct _pulse hdr; // Our real data comes after this header
	int ClientID; // our data (unique id from client)
    int data;     // our data
} my_data;

typedef struct
{
	struct _pulse hdr; // Our real data comes after this header
    char buf[BUF_SIZE];// Message we send back to clients to tell them the messages was processed correctly.
} my_reply;

// prototypes
int client(int serverPID, int serverCHID);

int main(int argc, char *argv[])
{
	printf("Client running\n");

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

	/*
	 *   Your code goes here.
	 *
	 *   You need to find the process ID (PID), and a channel ID (CHID) of the server to make a connection to it...
	 *
	 *   The data should be read from file on the server (for example it could be located at):
	 *
	 *	 serverPID  (first line of file)
 	 *	 Channel ID (second line of file)
	 *
	 *   Initially you may NEED TO START THE SERVER and set the serverPID = Process ID of the RUNNING SERVER
	 *
	 */

	int ret=0;
	ret = client(serverPID, serverCHID);

	printf("Main (client) Terminated....\n");
	return ret;
}

/*** Client code ***/
int client(int serverPID,  int serverChID)
{
    my_data msg;
    my_reply reply;

    msg.ClientID = 500;

    int server_coid;
    int index = 0;

	printf("   --> Trying to connect (server) process which has a PID: %d\n",   serverPID);
	printf("   --> on channel: %d\n\n", serverChID);

	// set up message passing channel
    server_coid = ConnectAttach(ND_LOCAL_NODE, serverPID, serverChID, _NTO_SIDE_CHANNEL, 0);
	if (server_coid == -1)
	{
        printf("\n    ERROR, could not connect to server!\n\n");
        return EXIT_FAILURE;
	}


    printf("Connection established to process with PID:%d, Ch:%d\n", serverPID, serverChID);

    // We would have pre-defined data to stuff here
    msg.hdr.type = 0x00;
    msg.hdr.subtype = 0x00;

    // Do whatever work you wanted with server connection
    for (index=0; index < 5; index++) // send data packets
    {
    	// set up data packet
    	msg.data=10+index;

    	// the data we are sending is in msg.data
        printf("Client (ID:%d), sending data packet with the integer value: %d \n", msg.ClientID, msg.data);
        fflush(stdout);

        if (MsgSend(server_coid, &msg, sizeof(msg), &reply, sizeof(reply)) == -1)
        {
            printf(" Error data '%d' NOT sent to server\n", msg.data);
            	// maybe we did not get a reply from the server
            break;
        }
        else
        { // now process the reply
            printf("   -->Reply is: '%s'\n", reply.buf);
        }

        sleep(5);	// wait a few seconds before sending the next data packet
    }

    // Close the connection
    printf("\n Sending message to server to tell it to close the connection\n");
    ConnectDetach(server_coid);

    return EXIT_SUCCESS;
}
