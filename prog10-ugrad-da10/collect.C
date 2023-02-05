#include <iostream>
#include <unistd.h>
#include <math.h>
#include "mpi.h"
#include "check.h"
#include "collect.h"

//Each process that calls this function, creates an array of
//that contains rows/numP rows. Each row has cols elements.
//These are the source arrays.  In addition, Process 0 creates
//a destination array that will be used to gather all rows.
//Each process needs to initialize their source array so that
//when the Gather is called, each element in the process 0 
//destination array will be equal to its index: dest[i] = i
void gatherRows(int rows, int cols, int myId, int numP)
{
    int * dest = NULL;  //Used by process 0
    int * src = NULL;   //Used by all processes
    int size = (rows * cols) / numP;
    src = (int *) malloc(sizeof(int) * (size));
    if (myId == 0)
    {
	dest = (int *) malloc(sizeof(int) * (rows * cols)); 
    }
    for (int i = 0; i < size; i++)
    {
	src[i] = size * myId + i;
    }
    MPI::COMM_WORLD.Gather(src, size, MPI::INT, dest, size, MPI::INT, 0);

    //TO DO:
    //Process 0 needs to create a dest array that is big enough to hold
    //all rows. All of the other processes will need to create a src array
    //that is rows/numP rows. (You need to figure out the number of ints and
    //allocate arrays of ints.)
    //All processes will need to initialize their src array.
    //All processes will need to execute a Gather so the src array values
    //end up on process 0.

    //Process 0 checks to see if the gather worked
    if (!myId)
    {
        std::cout << "Checking to see if Gather of rows succeeded\n";
        checkGatherRows(dest, rows, cols, myId);
    }
    free(dest);
    free(src);
    //TO DO: delete dynamically allocated data
}

//Each process that calls this function, creates an array of
//that contains rows/numP rows. Each row has cols elements.
//These are the source arrays.  In addition, each process creates
//a destination array that will be used to gather all rows.
//Each process needs to initialize their source array so that
//when the Allgather is called, each element in the 
//destination array will be equal to its index: dest[i] = i
void allGatherRows(int rows, int cols, int myId, int numP)
{
    int * dest = NULL;  //Used by all processes
    int * src = NULL;   //Used by all processes


    int size = (rows * cols) / numP;
    src = (int *) malloc(sizeof(int) * (size));
    dest = (int *) malloc(sizeof(int) * (rows * cols));
    for (int i = 0; i < size; i++)
    {
        src[i] = size * myId + i;
    }
    MPI::COMM_WORLD.Allgather(src, size, MPI::INT, dest, size, MPI::INT);

    //TO DO:
    //All processes will need to create a dest array to hold all rows.
    //All processes will need to create a src array to hold rows/numP rows.
    //All processes will initialize their src array.
    //All processes will execute an AllGather

    //here's the check
    if (!myId)
    {   
        std::cout << "Checking to see if All Gather of rows succeeded\n";
    }
    //every process should have received an identical array
    checkAllGatherRows(dest, rows, cols, myId);

    //TO DO: delete dynamically allocated data
    free(src);
    free(dest);
}

//This function will call Gather so that process 0 will collect the structs
//of all processes.
//
//Hint: You'll need to create an MPI Datatype
void gatherStructs(int myId, int numP)
{
    int * dest = NULL; 
    int i;
    char letters[12] = {'g', 'r', 'e', 'e', 't', 'i', 'n', 'g', 's', '!', '!', 0};
    struct sendStruct
    {
        char sletters[12];
        int rank;
    } sendThis;

    for (i = 0; i < 12; i++) sendThis.sletters[i] = letters[i];
    sendThis.rank = myId;

    //TO DO:
    //Process 0 needs to create a dest array that is large enough to hold all structs
    //from all processes. Note that the size of the struct is 16 bytes.
    //All processes need to use Create_struct to create an MPI data type.  See
    //example on page 343 (Listing 9.16; explained in slides).
    //All processes need to call Gather.
    if (myId == 0)
    {
	dest = (int *) malloc(sizeof(sendStruct) * 16);
    }
    int blockLengths[2] =  {12, 1};
    MPI::Aint disp[2] = {0, 12};
    MPI::Datatype types[2] = {MPI::CHAR, MPI::INT};
    MPI::Datatype newType = MPI::INT.Create_struct(2, blockLengths, disp, types);
   // What is the first parameter? MPI::COMM_WORLD.Gather(sendThis, 1, newType, dest, 1, newType, 0);
    //All of the struct values need to be at process 0
    if (!myId)
    {
        std::cout << "Checking to see if Gather of structs succeeded\n";
       // checkGatherStructs(dest, myId, numP);
    }

    //TO DO: delete dynamically allocated data 
    free(dest);
}

