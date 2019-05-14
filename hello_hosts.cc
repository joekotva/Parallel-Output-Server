/* Program:       hello_hosts.cc   
   Description:   This is an output server for MPI where the slaves can send a message
                  of arbitrary length. This is accomplished by using the 
                  MPI_Probe method to get information about the incoming message
                  from the slave before processing it. Information about the 
                  probed message is stored in a MPI_Status variable.
                  The relavant information retrieved from the status variable
                  is (1) the source process id, (2) the tag, and (3) the length
                  of the incoming message. The slave processes must follow the 
                  convention that each successive message they send will be sent with
                  the tag incremented by 1. This way, id and the tag together form a 
                  unique id for the message, which is necessary to ensure that
                  MPI_Recv processes the same message that was probed.

                  This program was tested on IUSB's lab computers.          

   Author:        Joseph Kotva (modified from original program by Dana Vrajitoru)                          
   Organization:  IUSB                                    
   Date:          December, 2018                            */

#include <mpi.h>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <climits>
using namespace std;

/**********************************************************/
/* The function that writes the message on the screen.    */
void Write_hello(int rank, char *name)
{
    cout << "Hello from process number " << rank 
         << " running on " << name << endl;
}

// The master receives messages from all the other processes and
// prints them.
void Master(int np, char *hostname) 
{
    MPI_Status status;
    int message_length;
    int tag;
    int flag = 0;
    int source;

    int i = 1;

    cout << "Process 0 is on " << hostname << endl;

    while(true)
    {
        // Probe for an incoming message from slave process
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        source = status.MPI_SOURCE;
        tag = status.MPI_TAG;

        // Get the length of the incoming message from the probe
        MPI_Get_count(&status, MPI_CHAR, &message_length);

        char* message = new char[message_length](); // dynamically create array with enough space for incoming message

        // receive message from source that was probed with the same tag
        MPI_Recv(message, message_length, MPI_CHAR, source, tag, MPI_COMM_WORLD, &status);

        cout << message << endl;

        delete message; // release memory for dynamically created array

        // Some end condition to terminate server.
        // Here it expects to receive the same number of messages as processes, but this could be
        // modified to a specific project's needs.
        i++;
        if (i == np)
            break;
    }
}

// The slave just sends a message
// In this simple example, each slave sends only a single message.
// However, as long as the slaves follow the convention that each
// successive message they send is sent with a different value for the tag--ensured
// by incrementing the value of the tag variable--then the master has a unique id for
// the message when combined with the slave process id.
void Slave(int id, char *hostname)
{    
	int tag = 0;
    char message[50];
    string s = "Hello from " + to_string(id) + " on " + hostname;
    // give a few different slaves very different message lengths for testing
    if (id == 1)
    {
        s = "Hello 1";
    }

    else if (id == 2)
    {
        s = "Hello from 2";
    }

    else if (id == 3)
    {
        s = "Hello from process 3";
    }
    
    strcpy(message, s.c_str());  
    
    MPI_Send(&message, strlen(message) + 1, MPI_CHAR, 0, tag, MPI_COMM_WORLD);
    tag++;	// we will use the convention that every successive message sent from a source increments the tag by 1
    
}

/**********************************************************/
/* Main                                                   */

int main(int argc, char **argv)
{
    double time;
    int mpirank, mpisize;
    
    /* Initialize MPI */
    MPI_Init (&argc, &argv);
    
    /* Get the rank of this process */
    MPI_Comm_rank (MPI_COMM_WORLD, &mpirank);
    
    /* Get nb of processes */
    MPI_Comm_size (MPI_COMM_WORLD, &mpisize);
    
    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);
    
    /* Each process writes hello and then waits for all of the others */
    time = MPI_Wtime();

    if (mpirank == 0)
        Master(mpisize, processor_name);
    else
        Slave(mpirank, processor_name);

    MPI_Barrier(MPI_COMM_WORLD);
    time = MPI_Wtime() - time;
    
    if (mpirank==0) 
        cout << "This program has run on " << mpisize
             << " processes in " << time << " seconds." << endl;
    
    /* Finalize MPI */
    MPI_Finalize ();
    return 0;
}

/* End of the program                                     */
/**********************************************************/
