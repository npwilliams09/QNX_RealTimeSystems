/*
 *  mqsend.c
 *  Simple example of using mqueue messaging between processes
 *
 *  This code requires that the mqueue service is started on the target.
 *  The mqueue manager implements POSIX 1003.1b message queues.
 *  By default it is not available on QNX 7 VM target - To make start the
 *  server you will need to copy the appropriate mqueue binary file from the
 *  QNX 7 target directory to the targets /sbin folder. i.e. copy:
 *  C:\qnx700\target\qnx7\x86_64\sbin\mqueue   (from Software Center install location)
 *    to
 *  /sbin     (on which would be on an x86_64 Target
 *
 *  Then type 'mqueue' at the terminal prior to starting running this code.
 *  For more info, read: Why use POSIX message queues?
 *  http://www.qnx.com/developers/docs/7.0.0/index.html#com.qnx.doc.neutrino.sys_arch/topic/ipc_Message_queues.html
 *
 *  The this program (mqsend) will send 5 messages to the queue. To recieve the messages you
 *  need to run the mqreceive in another terminal window before this program (mqsend) finishes executing.
 *
*/
#include <stdlib.h>
#include <stdio.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <fcntl.h>

#define  MESSAGESIZE 1000

#define Q_FLAGS O_RDWR | O_CREAT | O_EXCL
#define Q_Mode S_IRUSR | S_IWUSR
/*
 * For oflag overview see: http://www.qnx.com/developers/docs/7.0.0/index.html#com.qnx.doc.neutrino.lib_ref/topic/m/mq_open.html
 * O_RDWR	- send-receive   (others: O_RDONLY (receive-only), O_WRONLY (send-only))
 * O_CREAT	- instruct the server to create a new message queue
 *
 * Mode flags overview see: http://www.qnx.com/developers/docs/7.0.0/index.html#com.qnx.doc.neutrino.lib_ref/topic/s/stat_struct.html
 * S_IRUSR	- Read permission
 * S_IWUSR	- Write permission
 */

int main(int argc, char *argv[])
{
	printf("Welcome to the QNX Momentics mqueue send process\n");

    mqd_t	qd;
    int		i;
    char	buf[MESSAGESIZE] = {};

    struct  mq_attr  attr;

    attr.mq_maxmsg = 100;
    attr.mq_msgsize = MESSAGESIZE;
    attr.mq_flags = 0;
    attr.mq_curmsgs = 0;
    attr.mq_sendwait = 0;
	attr.mq_recvwait = 0;

    struct mq_attr * my_attr = &attr;


	// example using the default path notation.
	const char * MqueueLocation = "/net/iMX6_Lab/dev/mqueue/test_queue";	/* will be located /dev/mqueue/test_queue  */
   //const char * MqueueLocation = "/net/QNX7_VM_Target01/dev/mqueue/test_queue"; // (when Bridged use: QNX7_x86_Target_8xxxxxx.sece-lab.rmit.edu.au)
    /* Use the above line for networked (qnet) MqueueLocation
	 * the command 'hostname <name>' to set hostname. here it is 'M1'
	 * You mast also have qnet running. to do this execute the following
	 * command: mount -T io-pkt lsm-qnet.so
	 */

    qd = mq_open(MqueueLocation, Q_FLAGS, Q_Mode, my_attr);			 // full path will be: <host_name>/dev/mqueue/test_queue
    //qd = mq_open(MqueueLocation, Q_FLAGS, Q_Mode, NULL);

	mq_getattr( qd, my_attr );
	printf("my_attr->mq_maxmsg: '%ld'\n", my_attr->mq_maxmsg);
	printf("my_attr->mq_msgsize: '%ld'\n", my_attr->mq_msgsize);
	printf("my_attr->mq_flags: '%ld'\n", my_attr->mq_flags);
	printf("my_attr->mq_curmsgs: '%ld'\n", my_attr->mq_curmsgs);
	printf("my_attr->mq_sendwait: '%ld'\n", my_attr->mq_sendwait);
	printf("my_attr->mq_recvwait: '%ld'\n", my_attr->mq_recvwait);

    if (qd != -1)
    {
		for (i=1; i <= 5; ++i)
        {
			sprintf(buf, "message %d", i);			//put the message in a char[] so it can be sent
			printf("queue: '%s'\n", buf); 			//print the message to this processes terminal
			mq_send(qd, buf, MESSAGESIZE, 0);		//send the mqueue
			sleep(2);
		}
		mq_send(qd, "done", 5, 0);					// send last message so the receive process knows
													// not to expect any more messages. 5 char long because
													// of '/0' char at end of the "done" string
		printf("\nAll Messages sent to the queue\n");


		printf("\nWait here for 10 seconds before closing mqueue\n");
		sleep(10);

		// as soon as this code executes the mqueue data will be deleted
		// from the /dev/mqueue/test_queue  file structure
		mq_close(qd);
		mq_unlink(MqueueLocation);
    }
    else
    {
    	printf("\nThere was an ERROR opening the message queue!");
    	printf("\nHave you started the 'mqueue' process on the VM target?\n");
    	printf("\nHave you started the 'qnet' process on the VM target?\n");
    }

	printf("\nmqueue send process Exited\n");
	return EXIT_SUCCESS;
}
