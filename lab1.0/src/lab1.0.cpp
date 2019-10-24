#include <cstdlib>
#include <iostream>

#include <unistd.h>      // used for Cpp version of program
#include <string>
#include <sys/syspage.h> // for number of CPUs

using namespace std;

int main(int argc, char *argv[])
{
	cout << "Welcome to the QNX Momentics IDE" << endl;

	// This is a sample 'QNX C++ Project'

	// It will compile for only x86 VM targets.

	// To run the code on this Live image you will need to start VMware player
	// either within this system (see ReadMe file on desktop for further info)
	// or have an external VM or hardware target running on another machine which is
	// on the same network subnet as this machine. That is Momentics and the target need
	// to be on the same IP subnet (eg. 192.168.201.xxx in the RMIT Laboratory).

	// Once the target/s are running you can launch this program by pressing the small down
	// arrow next to the green play button and selecting Run Configuration. See the
	// 'QNX_Quickstart_Guide.pdf' for detailed explanation.

	// If the target you wish to launch is not present in the Project Explorer panel
	// (which is located on left panel) then you will need to add using the IP address
	// of the target. To do this select: Window->Open Perspective->QNX System Information
	// Then right click in the left hand panel and select: 'New QNX Target...'
	// Enter the IP address of the target you want to deploy too.
	// You can find the IP address of the target by using the command ifconfig

	// If you want to compile for ARM processors then change the properties of the
	// project by right clicking on the project name in the Project Explorer and selecting
	// Properties. Then select QNX C/C++ projects -> Build variants (tab), select ARMv7

	// Remember to press the stop button and the 'x' button to clear the gdb console (bottom panel)
	// after each lunch to the target. Else the thread might still be running.

	// Also if you are working off a Live DVD or Live USB, you will need to save your files
	// on another USB key before shutting down as your work will be lost.
	   // note: Every time you start QNX Momentics, it might be better to create a new
	   // directory workspace each time. That way you simply have a single workspace directory
	   // to backup when you finish.

	// After you are finished, please remember to shut down each of the targets your started
	// by typing 'shutdown -b' at the prompt. If the target is a VM you can power it down by
	// selecting Power off selection from the drop down menu:
	// Virtual Machine -> Power -> Power Off Guest

	// Now for some test code
	cout << endl; // print a blank line to the console

	// get the number of CPUs
	int num_cpus;
	num_cpus = _syspage_ptr -> num_cpu;
	cout << "--> Number of CPUs available is: "<< num_cpus << endl;

	cout << "--> The pid of this process is " << getpid() << endl;

	char hostnm[100];
    memset(hostnm, '\0', 100);
    hostnm[99] = '\n';
    gethostname(hostnm, sizeof(hostnm));

	cout << "--> Machine hostname is: " << hostnm << endl;

	cout << endl;

	sleep(10); // wait here for 10 seconds

	// Your test code can go here

	int i = 0;
	for (i = 1; i <= 10; i++){
		printf("Loop iteration %d\n", i);
		sleep(2);
	}

	// end program

	string str = "All good. Main Terminating.";

	cout << str << endl;

	return EXIT_SUCCESS;
}

