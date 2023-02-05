#include <iostream>
#include <unistd.h>
#include <math.h>
#include <functional>
#include "mpi.h"
#include "check.h"

#define DEBUG 0
#define RESET   "\033[0m"
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */

/* helper functions that are local to this file */
static void printResult(std::string msg, bool allgood);
static bool checkMatch(int expected, int got, int index, int myId, std::string what);
static bool checkDest(int * dest, int myId);
static bool checkP0(int myId, std::string func);

//Prints type of test and whether it passed. 
void printResult(std::string msg, bool allgood)
{
    if (allgood)
        std::cout << msg << GREEN "passed." RESET "\n";
    else
        std::cout << msg << RED "failed." RESET "\n";
}

//Checks whether the value in a destination array matches the expected value.
//If it doesn't match and DEBUG is true, an error message is printed.
bool checkMatch(int expected, int got, int index, int myId, std::string what)
{
    bool good = (expected == got);
    if (!good && DEBUG)
        printf(RED "Error: " RESET "process %d received the wrong %s for index %d. %s received: %d. %s expected %d.\n", 
               myId, what.c_str(), index, what.c_str(), got, what.c_str(), expected); 
    return good;
}

//Checks to make sure the destination array is not NULL.
//If it is NULL and DEBUG is true, an error message is printed.
bool checkDest(int * dest, int myId)
{
    bool good = (dest != NULL);
    if (!good && DEBUG) printf(RED "Error: " RESET "process %d has a NULL dest array\n", myId); 
    return good;
}     

//Checks to make sure the calling process id is 0.
//If it is not 0 and DEBUG is true, an error message is printed.
bool checkP0(int myId, std::string func)
{
    bool good = (myId == 0);
    if (!good && DEBUG) 
        printf(RED "Error: " RESET "only process 0 should call %s\n", func.c_str());
    return good;
}

//If predicate is true, the value of lambda expression truefunc is returned, else
//the value of lambda expression falsefunc is returned.
int increment(bool predicate, std::function<int(void)> truefunc, std::function<int(void)> falsefunc)
{
    if (predicate)
        return truefunc();
    else
        return falsefunc();
}
   
//Checks to see if the array of size rows * cols is distributed among
//the numP processes in a block cyclic way.
//The array is assumed to have been initialized such that array[i] = i.
void checkDistributeRows(int * dest, int rows, int cols, int myId, int numP)
{
    bool good = true, allgood = true;
    int count = rows/numP * cols;
    int value = myId * count;
    int i;

    if (myId) good &= checkDest(dest, myId);
    if (myId && good)
    {
        for (i = 0; i < count && good; i++)
        {
            //see if value and dest[i] match
            good &= checkMatch(value, dest[i], i, myId, "value");
            value++;
        }
        if (DEBUG && good) printf("Process %d received the correct data.\n", myId);
    }

    //see if every process got the correct result 
    MPI::COMM_WORLD.Reduce(&good, &allgood, 1, MPI::BOOL, MPI::LAND, 0);
    if (!myId) printResult("Check of row distribution ", allgood);
}

//Checks to see if the array of size rows * cols is distributed among
//the numP processes in columns of size cols/numP.  
//The array is assumed to have been initialized such that array[i] = i.
void checkDistributeCols(int * dest, int rows, int cols, int myId, int numP)
{   
    bool good = true, allgood = true;
    int numCols = cols/numP;
    int count = numCols * rows;
    int rowValue = myId * numCols;
    int value;
    int i;

    //lambda expressions for calculating next value in dest
    std::function<int (void)> ifTrue = [&] ()
    {
        rowValue += cols;
        return rowValue;
    }; 
    std::function<int (void)> ifFalse = [&] () { return value + 1; };

    //make sure dest array is not NULL
    if (myId) good &= checkDest(dest, myId);

    if (myId && good)
    {   value = rowValue; 
        for (i = 0; i < count && good; i++)
        {   
            //check to see if the current value matches dest[i]
            good &= checkMatch(value, dest[i], i, myId, "value");
            //calculate the next value
            value = increment((((i+1) % numCols) == 0), ifTrue, ifFalse);
        }
        if (DEBUG && good) printf("Process %d received the correct data.\n", myId);
    }

    //check to see if every process received the correct values
    MPI::COMM_WORLD.Reduce(&good, &allgood, 1, MPI::BOOL, MPI::LAND, 0);
    if (!myId) printResult("Check of column distribution ", allgood);
}

//Checks to see if the array of size rows * cols is distributed among
//the numP processes in blocks of size rows/gridDim by cols/gridDim
//where gridDim is sqrt(numP).  
//The array is assumed to have been initialized such that array[i] = i.
void checkDistributeBlocks(int * dest, int rows, int cols, int myId, int numP)
{
    bool good = true, allgood = true;
    if (myId) good &= checkDest(dest, myId);
    int gridDim = sqrt(numP);
    int numCols = cols/gridDim;
    int numRows = rows/gridDim;
    int count = numCols * numRows;
    int rowIdx = myId / gridDim;
    int colIdx = myId % gridDim;
    int rowValue = rowIdx * numRows * cols + colIdx * numCols;
    int i, value;

    //lambda expressions for calculating next value in dest
    std::function<int (void)> ifTrue = [&] ()
    {
        rowValue += cols;
        return rowValue;
    };
    std::function<int (void)> ifFalse = [&] () { return value + 1; };

    if (myId && good)
    {   
        value = rowValue;
        for (i = 0; i < count && good; i++)
        {
            //make sure value matches dest[i]
            good &= checkMatch(value, dest[i], i, myId, "value");
            //calculate next value 
            value = increment((((i+1) % numCols) == 0), ifTrue, ifFalse);
        }
        if (DEBUG && good) printf("Process %d received the correct data.\n", myId);
    }

    //make sure every process got the correct values
    MPI::COMM_WORLD.Reduce(&good, &allgood, 1, MPI::BOOL, MPI::LAND, 0);
    if (!myId) printResult("Check of block distribution ", allgood);
}

//Checks to see if the array of size rows * cols is distributed among
//the numP processes in cyclic manner.  
//The array is assumed to have been initialized such that array[i] = i.
void checkDistributeCyclic(int * dest, int rows, int cols, int myId, int numP)
{
    bool good = true, allgood = true;
    int numRows = rows/numP;
    int i, value;
    int currRow = myId;
    int count = rows/numP * cols;

    //lambda expressions for calculating next value in array
    std::function<int (void)> ifTrue = [&] ()
    {   
        currRow += numP;
        return currRow * cols;
    };
    std::function<int (void)> ifFalse = [&] () { return value + 1; };

    if (myId) good &= checkDest(dest, myId);

    if (myId && good)
    {
        value = currRow * cols;
        for (i = 0; i < count && good; i++)
        {
            //make sure value matches dest[i]
            good &= checkMatch(value, dest[i], i, myId, "value");
            //calculate next value for value
            value = increment((((i+1) % cols) == 0), ifTrue, ifFalse);
        }
    }

    //make sure every process got the correct values
    MPI::COMM_WORLD.Reduce(&good, &allgood, 1, MPI::BOOL, MPI::LAND, 0);
    if (!myId) printResult("Check of cyclic distribution ", allgood);
}

//Checks to see if the array of size rows * cols is gathered at P0.
//The array is assumed to have been initialized such that array[i] = i.
//Each process initialized a portion of the array before the gather.
void checkGatherRows(int * dest, int rows, int cols, int myId)
{
    bool good = true;
    int i;

    //make sure only P0 calls this function
    good &= checkP0(myId, "checkGatherRows");
    //make sure the destination array is not NULL
    if (good) good &= checkDest(dest, myId);

    for (i = 0; i < rows * cols && good == true; i++)
    {
        if (dest[i] != i)
        {
            //make sure dest[i] is i
            good &= checkMatch(i, dest[i], i, myId, "value");
        }
    }
     
    if (!myId) printResult("Check of gather of rows ", good);
}

//Checks to see if the array of size rows * cols is gathered at process myId.
//The array is assumed to have been initialized such that array[i] = i.
//Each process initialized a portion of the array before the gather.
void checkAllGatherRows(int * dest, int rows, int cols, int myId)
{
    bool good = true, allgood = true;
    int i;

    //make sure destination array is not NULL
    good &= checkDest(dest, myId);

    for (i = 0; i < rows * cols && good == true; i++)
    {
        //make sure dest[i] is i
        if (dest[i] != i)
        {
            good &= checkMatch(i, dest[i], i, myId, "value");
        }
    }

    //make sure every process got the correct values
    MPI::COMM_WORLD.Reduce(&good, &allgood, 1, MPI::BOOL, MPI::LAND, 0);
    if (!myId) printResult("Check of all gather of rows ", allgood);
}

//Checks to see if process 0 has gathere a struct from every other process.
//The struct contains a 12 byte array consisting of c-the string "greetings!!"
//and a 4 byte integer value that is the process's rank.
void checkGatherStructs(int * dest, int myId, int numP)
{
    bool good = true;
    int i, j;
    char letters[12] = {'g', 'r', 'e', 'e', 't', 'i', 'n', 'g', 's', '!', '!', 0};

    //make sure only P0 called this function
    good &= checkP0(myId, "checkGatherStructs");
    //make sure dest array is not NULL
    if (good) good &= checkDest(dest, myId);

    for (i = 0; i < numP && good == true; i++)
    {
        char * msg = (char *) &dest[i * 4];
        for (j = 0; j < 12; j++) 
        {
            //make sure the received message is correct
            good &= checkMatch(letters[j], msg[j], j, myId, "letter");
        }
        //make sure the received rank is correct
        good &= checkMatch(i, dest[i*4 + 3], i*4 + 3, myId, "rank");
    }

    if (!myId) printResult("Check of gather of structs ", good);
}
