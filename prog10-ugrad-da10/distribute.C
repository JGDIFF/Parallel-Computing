#include <iostream>
#include <unistd.h>
#include <math.h>
#include "mpi.h"
#include "check.h"
#include "distribute.h"

#define RESET   "\033[0m"
#define RED     "\033[31m"      /* Red */

//This function uses Send and Recv to distribute the rows of the data array
//in a blocked way using numP - 1 Sends.  Thus process 1 gets rows numP ... 2*numP - 1, 
//process 2 gets rows 2*numP ... 3*numP - 1, process 3 gets rows 3*numP .. 4*numP - 1,
//etc.
void distributeRowsSendRecv(int * data, int rows, int cols, int myId, int numP)
{
    int * dest = NULL;

    //TO DO:
    //Each process will receive its data in the array pointed to by dest.
    //You need to dynamically allocate an array that is big enough for
    //a process to hold its share of the data (but not too big).
    //Process 0 makes numP - 1 calls to Send.
    //All other processes make one call to Recv.
    int size, i = 0;
    //MPI::Init(data, rows, cols, myId, numP);
    size = (rows  * cols) / numP;
    if (myId)
	 dest =  (int *) malloc(sizeof(int) * size);

    if (myId == 0)
    {
	for (i = 1; i < numP; i++) {
	    MPI::COMM_WORLD.Send(&data[i * size], size, MPI::INT, i, 0);
	}
    } else {
        MPI::COMM_WORLD.Recv(dest, size, MPI::INT, 0, 0);
    }

    //Here's the check
    MPI::COMM_WORLD.Barrier();  //barrier used to try to get neater output 
    if (!myId) 
        std::cout << "Checking to see if the distribution of rows using Recv succeeded\n";
    checkDistributeRows(dest, rows, cols, myId, numP);

    //TO DO: Delete dynamically allocated data
    free(dest);
    //MPI::Finalize();
}

//This function uses Isend and Irecv to distribute the rows of the data array
//in a blocked way using numP - 1 Isends.  Thus process 1 gets rows numP ... 2*numP - 1, 
//process 2 gets rows 2*numP ... 3*numP - 1, process 3 gets rows 3*numP .. 4*numP - 1
//etc.
void distributeRowsIsendIrecv(int * data, int rows, int cols, int myId, int numP)
{
    int * dest = NULL;

    //TO DO:
    //Each process will receive its data in the array pointed to by dest.
    //You need to dynamically allocate an array that is big enough for
    //a process to hold its share of the data (but not too big).
    //Process 0 makes numP - 1 calls to Isend. 
    //All other processes make one call to Irecv and one call to Wait.
    int size, i = 0;
    size = (rows * cols) / numP;
    if (myId) dest = (int *) malloc(sizeof(int) * size);
    if (myId == 0) {
	for (i = 1; i < numP; i++) {
	    MPI::COMM_WORLD.Isend(&data[i * size], size, MPI::INT, i, 0);
	} 
    } else {
	MPI::COMM_WORLD.Irecv(dest, size, MPI::INT, 0, 0);
    }
    //Here's the check
    MPI::COMM_WORLD.Barrier();  //barrier used to try to get neater output in case you've added print statements
    if (!myId) 
        std::cout << "Checking to see if the distribution of rows using Irecv succeeded\n";
    checkDistributeRows(dest, rows, cols, myId, numP);

    //TO DO: Delete dynamically allocated data.
    free(dest);
}

//This function uses Scatter to distribute the rows of the data array
//in a blocked way.  Thus process 1 gets rows numP ... 2*numP - 1, process 2 
//gets rows 2*numP ... 3*numP - 1, process 3 gets rows 3*numP .. 4*numP - 1
//etc.
void distributeRowsScatter(int * data, int rows, int cols, int myId, int numP)
{
    int * dest = NULL;

    int size, i = 0;
    //MPI::Init(data, rows, cols, myId, numP);
    size = (rows  * cols) / numP;
    dest =  (int *) malloc(sizeof(int) * size);
    MPI::COMM_WORLD.Scatter(data, size, MPI::INT, dest, size, MPI::INT, 0);
    //TO DO:
    //Each process will receive its data in the array pointed to by dest.
    //You need to dynamically allocate an array that is big enough for
    //a process to hold its share of the data (but not too big).
    //Then call Scatter.

    //Here's the check
    MPI::COMM_WORLD.Barrier();  //barrier used to try to get neater output in case you've added print statements
    if (!myId) 
        std::cout << "Checking to see if the distribution of rows using Scatter succeeded\n";
    checkDistributeRows(dest, rows, cols, myId, numP);

    //TO DO: Delete dynamically allocated data.
    free(dest);
}

//This function uses Send and Recv to distribute the cols of the data array
//in a blocked way using numP - 1 Sends.  Thus process 1 gets columns numP ... 2*numP - 1,
//process 2 gets columns 2*numP ... 3*numP - 1, process 3 gets columns 3*numP .. 4*numP - 1
//etc.
//
//HINT: create a new MPI Datatype
void distributeColsSendRecv(int * data, int rows, int cols, int myId, int numP)
{   
    int * dest = NULL;
    
    //TO DO:
    //Each process will receive its data in the array pointed to by dest.
    //You need to dynamically allocate an array that is big enough for
    //a process to hold its share of the data (but not too big).
    //You need to define and create an MPI Datatype using Create_vector.
    //Process 0 needs to call Send numP - 1 times.
    //All other processes need to call Recv.
    int size, i = 0;
    size = (rows * cols) / numP;
    if (myId) dest = (int *) malloc(sizeof(int) * size);
    MPI::Datatype newType = MPI::INT.Create_vector(rows, cols/numP, cols);
    newType.Commit();
    if (myId == 0) {
        for (i = 1; i < numP; i++) {
            MPI::COMM_WORLD.Send(&data[i * cols / numP], 1, newType, i, 0);
        }
    } else {
        MPI::COMM_WORLD.Recv(dest, size, MPI::INT, 0, 0);
    } 
    //Here's the check
    if (!myId) 
        std::cout << "Checking to see if the distribution of columns using Recv succeeded\n"; 
    MPI::COMM_WORLD.Barrier();
    checkDistributeCols(dest, rows, cols, myId, numP);

    //TO DO: Delete dynamically allocated data.
    newType.Free();
    free(dest);
}

