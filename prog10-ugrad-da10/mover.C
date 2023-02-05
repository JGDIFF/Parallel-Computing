#include <iostream>
#include <unistd.h>
#include <math.h>
#include "mpi.h"
#include "collect.h"
#include "distribute.h"

/*
 * This file contains the code that parses the command line arguments, creates
 * and initialize the array, calls the distribute and collect functions, and
 * checks whether the distribution and collection functions produced the
 * correct results.
 *
 * Usage: mover rows cols
 *        The number of processes must be a multiple of the rows and the columns
 *        Thus, rows % numP and cols % numP must both be 0
 *        Also, to test the code that distributes blocks, the number of processes
 *        must be square. 
*/
static bool checkArgs(int argc, char * argv[], int numP, int myId, int & rows, int & cols);
static void printUsage(int numP);
static int * initData(int rows, int cols);

int main (int argc, char *argv[])
{
    int * data = NULL;
    int rows, cols;

    //Initialize MPI
    MPI::Init(argc,argv);

    //Get the number of processes
    int numP=MPI::COMM_WORLD.Get_size();

    //Get the ID of the process
    int myId=MPI::COMM_WORLD.Get_rank();

    bool good = checkArgs(argc, argv, numP, myId, rows, cols);

    MPI::COMM_WORLD.Barrier();
    if (good)
    {
        //Only process 0 initializes the data array
        if (!myId) data = initData(rows, cols);

        //int wait = 1;
        //while (wait && myId == 0);


        //Call the distribution and collect functions 
        if (!myId) std::cout << "\nTesting distribution of rows using Send and Recv.\n";
        distributeRowsSendRecv(data, rows, cols, myId, numP);

        MPI::COMM_WORLD.Barrier();
        if (!myId) std::cout << "\nTesting distribution of rows using Isend and Irecv.\n";
        distributeRowsIsendIrecv(data, rows, cols, myId, numP);

        MPI::COMM_WORLD.Barrier();
        if (!myId) std::cout << "\nTesting distribution of rows using Scatter.\n";
        distributeRowsScatter(data, rows, cols, myId, numP);

        MPI::COMM_WORLD.Barrier();
        if (!myId) std::cout << "\nTesting distribution of columns using Send and Recv.\n";
        distributeColsSendRecv(data, rows, cols, myId, numP);

        MPI::COMM_WORLD.Barrier();
        if (!myId) std::cout << "\nTesting gather of rows using Gather.\n";
        gatherRows(rows, cols, myId, numP);

        MPI::COMM_WORLD.Barrier();
        if (!myId) std::cout << "\nTesting all gather of rows using Allgather.\n";
        allGatherRows(rows, cols, myId, numP);

        MPI::COMM_WORLD.Barrier();
        if (!myId) std::cout << "\nTesting gather of structs using Gather.\n";
        gatherStructs(myId, numP);

        if (!myId) delete[] data;
    }

    //Terminate MPI
    MPI::Finalize();
    return 0;
}

//Initializes data to be an int array of size rows * cols where
//data[i] is set to i.
int * initData(int rows, int cols)
{
    int * data = new int[rows * cols];
    if (data == NULL)
    {
       printf("new failed in initData\n");
       exit(0);
    }
    int i, j;
    for (i = 0; i < rows; i++)
        for (j = 0; j < cols; j++) data[i * cols + j] = i * cols + j;
    return data;
}

//Checks the command line arguments. Process 0 prints usage information if
//they are incorrect.
bool checkArgs(int argc, char * argv[], int numP, int myId, int & rows, int & cols)
{
    if (argc < 3)
    {
        if (!myId) printUsage(numP);
        return false;
    }
    rows = atoi(argv[1]);
    cols = atoi(argv[2]);
    if (rows <= 0 || cols <= 0)
    {
        if (!myId) printUsage(numP);
        return false;
    }
    if ((rows % numP) || (cols % numP))
    {
        if (!myId) printUsage(numP);
        return false;
    }
    if (numP <= 1)
    {
        if (!myId) printUsage(numP);
        return false;
    }
    long size = (long) rows * (long) cols * 4;
    if (size > ((long) 1 << 31) - 1)
    {
       if (!myId)
       {
          printf("Size of data array is too large. Use smaller parameters.\n");
          printUsage(numP);
       }
       return false;
    }
    return true;
}

//prints usage information
void printUsage(int numP)
{
    std::cout << "usage: mpirun -np <p> ./mover <r> <c>\n";
    std::cout << "\tCreates an array of size <r> by <c>";
    std::cout << " where <r> is the number\n";
    std::cout << "\tof rows and <c> is the number of columns.\n";
    std::cout << "\t<p> (> 1) is the number of processes to create.\n";
    std::cout << "\t<r> must be a multiple of the number of processes.\n";
    std::cout << "\t<c> must be a multiple of the number of processes.\n";
}
