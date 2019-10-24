/*
 * see: Protecting Your Data in a Multi-Threaded App, by John Fehr
 * http://www.qnx.com/developers/articles/article_301_2.html
 *
 * overlap2.c
 *
 * See "Protecting Your Data in a Multi-Threaded App"
 *
 * Uses mutual exclusion (mutex) to ensure safety
 *
 * Result is correct
 *
*/
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>

 typedef struct {
  int a;
  int b;
  int result;
  int result2;
  int use_count;
  int use_count2;
  int max_use;
  int max_use2;
  sem_t * sem; // add a mutex object into the structure we want to protect
} app_data;

void *user_thread(void *data)
{
	 int uses=0;
	 app_data *td=(app_data*)data;
	 while(uses<td->max_use)
	 {
		 sem_wait(td->sem);	//lock the function to make sure the variables are protected
		 if (td->a==5)
		 {
			 td->result+=(td->a+td->b);
			 td->use_count++;
			 uses++;
		 }
		 sem_post(td->sem);	//unlock the functions to release the variables for use by other functions
		 usleep(1);
	 }
	 return 0;
}

void *changer_thread(void *data)
{
	 app_data *td=(app_data*)data;
	 while ((td->use_count+td->use_count2)<(td->max_use+td->max_use2))
	 {
		 sem_wait(td->sem);	//lock the function to make sure the variables are protected
		 if (td->a==5)
		 {
			 td->a=50;
			 td->b=td->a+usleep(1000);		//fake a CPU-intensive calculation for a with a usleep(1000) call
		 }
		 else
		 {
			 td->a=5;
			 td->b=td->a+usleep(1000);		//fake a CPU-intensive calculation for b with a usleep(1000) call
		 }
		 sem_post(td->sem);	//unlock the functions to release the variables for use by other functions
		 usleep(1);
	 }
	 return 0;
}

int main(int argc, char *argv[]) {
	printf("Welcome to the QNX Momentics IDE\n");

	pthread_t ct,ut,st;
	app_data td={5,5,0,0,0,0,100,0};
	void *retval;

	// Create the mutex
	td.sem = sem_open("/dev/sem/tmp/sem244",O_CREAT, S_IRWXG, 1);

	pthread_create(&ut,NULL,user_thread,&td);
	pthread_create(&ct,NULL,changer_thread,&td);

	// wait for all threads to finish
	pthread_join(ct,&retval);
	pthread_join(ut,&retval);

	// Destroy the mutex
	sem_unlink("/dev/sem/tmp/sem244");
	sem_close(td.sem);

	// print results
	printf("result should be %d, is %d\n",td.max_use*(5+5),td.result);

	printf("\nMain Terminating....");
	return EXIT_SUCCESS;
}
