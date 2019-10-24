#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/dispatch.h>
#include <fcntl.h>
#include <share.h>

// Define where the channel is located
// only need to use one of these (depending if you want to use QNET networking or running it locally):
#define LOCAL_ATTACH_POINT "s244"			  			     // change myname to the same name used for the server code.
#define QNET_ATTACH_POINT  "/net/iMX6_Lab/dev/name/local/s244"  // hostname using full path, change myname to the name used for server

#define BUF_SIZE 100


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
int client(char *sname);

int main(int argc, char *argv[])
{
	printf("This is A Client running\n");

    int ret = 0;

    ret = client(QNET_ATTACH_POINT);

	printf("Main (client) Terminated....\n");
	return ret;
}

/*** Client code ***/
int client(char *sname)
{
    my_data msg;
    my_reply reply;

    msg.ClientID = 601; // unique number for this client (optional)

    int server_coid;
    int index = 0;

    printf("  ---> Trying to connect to server named: %s\n", sname);
    if ((server_coid = name_open(sname, 0)) == -1)
    {
        printf("\n    ERROR, could not connect to server!\n\n");
        return EXIT_FAILURE;
    }

    printf("Connection established to: %s\n", sname);

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
    name_close(server_coid);

    return EXIT_SUCCESS;
}
