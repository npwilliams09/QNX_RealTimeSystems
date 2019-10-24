#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <fcntl.h>
#include <share.h>

// Define what the channel is called. It will be located at <hostname>/dev/name/local/myname"
// change myname to something unique for you (The client should be set to same name)
#define ATTACH_POINT "s244"

#define BUF_SIZE 100


typedef struct
{
	struct _pulse hdr; // Our real data comes after this header
	int ClientID; // our data (unique id from client)
    int data;     // our data
} my_data;

typedef struct
{
	struct _pulse hdr;  // Our real data comes after this header
    char buf[BUF_SIZE]; // Message we send back to clients to tell them the messages was processed correctly.
} my_reply;

// prototypes
int server();

int main(int argc, char *argv[])
{
	printf("Server running\n");

    int ret = 0;
    ret = server();

	printf("Main (Server) Terminated....\n");
	return ret;
}


/*** Server code ***/
int server()
{
   name_attach_t *attach;

   // Create a local name (/dev/name/...)
   if ((attach = name_attach(NULL, ATTACH_POINT, 0)) == NULL)
   {
       printf("\nFailed to name_attach on ATTACH_POINT: %s \n", ATTACH_POINT);
       printf("\n Possibly another server with the same name is already running !\n");
	   return EXIT_FAILURE;
   }

   printf("Server Listening for Clients on ATTACH_POINT: %s \n", ATTACH_POINT);

   	/*
	 *  Server Loop
	 */
   my_data msg;
   int rcvid=0, msgnum=0;  		// no message received yet
   int Stay_alive=1, living=0;	// server stays running (ignores _PULSE_CODE_DISCONNECT request)

   my_reply replymsg; // replymsg structure for sending back to client
   replymsg.hdr.type = 0x01;
   replymsg.hdr.subtype = 0x00;

   living =1;
   while (living)
   {
	   // Do your MsgReceive's here now with the chid
       rcvid = MsgReceive(attach->chid, &msg, sizeof(msg), NULL);

       if (rcvid == -1)  // Error condition, exit
       {
           printf("\nFailed to MsgReceive\n");
           break;
       }

       // did we receive a Pulse or message?
       // for Pulses:
       if (rcvid == 0)  //  Pulse received, work out what type
       {
           switch (msg.hdr.code)
           {
			   case _PULSE_CODE_DISCONNECT:
					// A client disconnected all its connections by running
					// name_close() for each name_open()  or terminated
				   if( Stay_alive == 0)
				   {
					   ConnectDetach(msg.hdr.scoid);
					   printf("\nServer was told to Detach from ClientID:%d ...\n", msg.ClientID);
					   living = 0; // kill while loop
					   continue;
				   }
				   else
				   {
					   printf("\nServer received Detach pulse from ClientID:%d but rejected it ...\n", msg.ClientID);
				   }
				   break;

			   case _PULSE_CODE_UNBLOCK:
					// REPLY blocked client wants to unblock (was hit by a signal
					// or timed out).  It's up to you if you reply now or later.
				   printf("\nServer got _PULSE_CODE_UNBLOCK after %d, msgnum\n", msgnum);
				   break;

			   case _PULSE_CODE_COIDDEATH:  // from the kernel
				   printf("\nServer got _PULSE_CODE_COIDDEATH after %d, msgnum\n", msgnum);
				   break;

			   case _PULSE_CODE_THREADDEATH: // from the kernel
				   printf("\nServer got _PULSE_CODE_THREADDEATH after %d, msgnum\n", msgnum);
				   break;

			   default:
				   // Some other pulse sent by one of your processes or the kernel
				   printf("\nServer got some other pulse after %d, msgnum\n", msgnum);
				   break;

           }
           continue;// go back to top of while loop
       }

       // for messages:
       if(rcvid > 0) // if true then A message was received
       {
		   msgnum++;

		   // If the Global Name Service (gns) is running, name_open() sends a connect message. The server must EOK it.
		   if (msg.hdr.type == _IO_CONNECT )
		   {
			   MsgReply( rcvid, EOK, NULL, 0 );
			   printf("\n gns service is running....");
			   continue;	// go back to top of while loop
		   }

		   // Some other I/O message was received; reject it
		   if (msg.hdr.type > _IO_BASE && msg.hdr.type <= _IO_MAX )
		   {
			   MsgError( rcvid, ENOSYS );
			   printf("\n Server received and IO message and rejected it....");
			   continue;	// go back to top of while loop
		   }

		   // A message (presumably ours) received

		   // put your message handling code here and assemble a reply message
		   sprintf(replymsg.buf, "Message %d received", msgnum);
		   printf("Server received data packet with value of '%d' from client (ID:%d), ", msg.data, msg.ClientID);
		   	   fflush(stdout);
		   	   sleep(1); // Delay the reply by a second (just for demonstration purposes)

		   printf("\n    -----> replying with: '%s'\n",replymsg.buf);
		   MsgReply(rcvid, EOK, &replymsg, sizeof(replymsg));
       }
       else
       {
		   printf("\nERROR: Server received something, but could not handle it correctly\n");
       }

   }

   // Remove the attach point name from the file system (i.e. /dev/name/local/<myname>)
   name_detach(attach, 0);

   return EXIT_SUCCESS;
}
