/*
 *  mqreceive.c
 *
 *  Simple example of using mqueue messaging between processes
 *
 *  Run mqsend in one terminal window, then after a few messages
 *  are queued (less than 5) run mqreceive in another terminal window
 *
*/

#include <stdlib.h>
#include <stdio.h>
#include <mqueue.h>

#define  MESSAGESIZE 1000

int main(int argc, char *argv[])
{
	printf("Welcome to the QNX Momentics mqueue receive process\n");

	mqd_t	qd;
	char	buf[MESSAGESIZE]= {};  // buffer needs to be bigger or same size as message being sent

	struct	mq_attr	attr;

	// example using the default path notation.
	const char * MqueueLocation = "/net/iMX6_Lab/dev/mqueue/test_queue";	/* will be located /dev/mqueue/test_queue  */
    //const char * MqueueLocation = "/net/QNX7_VM_Target01/dev/mqueue/test_queue"; // (when Bridged use: QNX7_x86_Target_8xxxxxx.sece-lab.rmit.edu.au)
    /* Use the above line for networked (qnet) MqueueLocation
	 * the command 'hostname <name>' to set hostname. here it is 'M1'
	 * You mast also have qnet running. to do this execute the following
	 * command: mount -T io-pkt lsm-qnet.so
	 */



	qd = mq_open(MqueueLocation, O_RDONLY);		//MqueueLocation should be opened on the node where the queue was established
    if (qd != -1)
	{
		mq_getattr(qd, &attr);
		printf ("max. %ld msgs, %ld bytes; waiting: %ld\n", attr.mq_maxmsg, attr.mq_msgsize, attr.mq_curmsgs);

		while (mq_receive(qd, buf, attr.mq_msgsize, NULL) > 0)  //wait for the messages
		{
			printf("dequeue: '%s'\n", buf);					//print out the messages to this terminal
			if (!strcmp(buf, "done"))			//once we get this message we know not to expect any more mqueue data
				break;
		}
		mq_close(qd);
	}

	printf("mqueue receive process exited\n");
	return EXIT_SUCCESS;
}
