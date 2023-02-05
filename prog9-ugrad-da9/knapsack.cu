#include <stdio.h>
#include <stdlib.h>
#include "helpers.h"
#include "h_knapsack.h"
#include "d_knapsack.h"
#include "wrappers.h"

//If you set this to 1, the program will print the weights, values, and result.
//You probably don't want to do this with large arrays.
#define DEBUG 0

//default values for parameters
#define WHICH_DEFAULT NAIVE
#define BLKDIM_DEFAULT 256 
#define NUMOBJS_DEFAULT 11
#define CAPACITY_DEFAULT 11
#define BLKDIMARR 6
#define NUMOBJARR 22 
#define CAPACITYARR 21

//legal dimensions for the block, number of objects, and capacity
static int blkDims[BLKDIMARR] = {32, 64, 128, 256, 512, 1024};
static int numObjs[NUMOBJARR] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24};
static int capacities[CAPACITYARR] = {4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24};

//prototypes for functions in this file
static void initArray(int * array, int length, int maxVal);
static void parseArgs(int argc, char * argv[], int * numObjs, int * capacity,
                      int * blkDim, int * which, bool * doTime);
static int check(const char * label, int * values, int len, int what);
static void compare(int * result1, int * result2, int size);
static void printUsage();
static void printHeaders(int numObjs, int capacity, int maxWeight, int maxValue, 
                         int blkDim, int which);

//helper function for debugging
static void printResults(const char * header, int * result, int * weights, int * values, 
                         int numObjs, int capacity);

/*
   driver for the knapsack program.  
*/
int main(int argc, char * argv[])
{
    //parameters for GPU version
    int numObjs, capacity, which, blkDim;
    bool doTime;

    //parse the command line arguments
    parseArgs(argc, argv, &numObjs, &capacity, &blkDim, &which, &doTime);

    //dynamically allocate the matrices
    int * weights = (int *) Malloc(sizeof(int) * numObjs);
    int * values  = (int *) Malloc(sizeof(int) * numObjs);
    int * h_result = (int *) Malloc(sizeof(int) * (capacity + 1));
    int * d_result = (int *) Malloc(sizeof(int) * (capacity + 1));
    int maxWeight, maxValue = 10;;

    //CPU time, GPU time, speedup
    float h_time, d_time, speedup;

    //initialize arrays
    //base the maxWeight of an individual object on the capacity
    maxWeight = capacity/numObjs; 
    if (maxWeight <= 1) maxWeight = 1;
    initArray(weights, numObjs, maxWeight);
    initArray(values, numObjs, maxValue);

    //print about information
    printHeaders(numObjs, capacity, maxWeight, maxValue, blkDim, which);

    //solve the knapsack problem on the CPU
    h_time = h_knapsack(h_result, weights, values, numObjs, capacity);
    if (DEBUG)
        printResults("CPU RESULTS", h_result, weights, values, numObjs, capacity);

    //perform the knapsack of the matrix on the GPU
    d_time = d_knapsack(d_result, weights, values, numObjs, capacity, blkDim, which);
    if (DEBUG)
        printResults("GPU RESULTS", d_result, weights, values, numObjs, capacity);

    //compare GPU and CPU results 
    compare(h_result, d_result, capacity + 1);
    printf("GPU result is correct.\n");
    printf("Value of knapsack is equal to %d.\n", h_result[capacity]);

    //run the GPU version multiple times to get somewhat accurate timing
    if (doTime == true)
    {
        //Because the GPU time varies greatly, we will run the GPU code
        //multiple times and compute an average.
        //In addition, ignore the first couple of times since it takes
        //time for the GPU to "warm-up."
        printf("Timing the kernel. This may take a bit.\n");
        d_time = d_knapsack(d_result, weights, values, numObjs, capacity, blkDim, which);
        d_time = 0;
        int i;
        for (i = 0; i < 5; i++)
            d_time += d_knapsack(d_result, weights, values, numObjs, capacity, blkDim, which);
        d_time = d_time/5.0;

        //Output the times and the speedup
        printf("\nTiming\n");
        printf("------\n");
        printf("CPU: \t\t\t%f msec\n", h_time);
        printf("GPU: \t\t\t%f msec\n", d_time);
        speedup = h_time/d_time;
        printf("Speedup: \t\t%f\n", speedup);
    }

    //free dynamically allocated data
    free(h_result);
    free(d_result);
    free(weights);
    free(values);
}    

/*
 * printResults
 * Outputs the weights, values, number of objects, capacity and the result array.
 * This function can be used for debugging.
 * header - header to be output
 * result - contains the result of the knapsack problem
 * weights - pointer to weights of the objects
 * values - pointer to values of the objects
 * numObjs - number of objects
 * capacity - capacity of the knapsack
*/
void printResults(const char * header, int * result, int * weights, int * values, 
                  int numObjs, int capacity)
{
   int i;
   char buffer[10];
   printf("%s\n", header);
   printf("Number of objects: %d\n", numObjs);
   printf("Capacity of knapsack: %d\n", capacity);
   printf("Weights and values:");
   for (i = 0; i < numObjs; i++)
   {
      if (i % 10 == 0) printf("\n");
      if (snprintf(buffer, 10, "(%d) %d,%d ", i, weights[i], values[i]) < 0) 
         printf("snprintf error in printResults\n");
      else
         printf("%10s", buffer); 
   }
   printf("\nResult:");
   for (i = 0; i < capacity + 1; i++)
   {
      if (i % 10 == 0) printf("\n");
      if (snprintf(buffer, 10, "(%d) %d ", i, result[i]) < 0)
         printf("snprintf error in printResults\n");
      else
         printf("%10s", buffer); 
   }
   printf("\n");
}

/*
 * printHeaders
 * Output information about the knapsack problem to be solved.
 * numObjs - number of objects
 * capacity - capacity of the knapsack
 * maxWeight - maximum weight of any individual object
 * maxValue - maximum value of any individual object
 * blkDim - number of threads used to solve the problem on the GPU
 * which - which GPU kernel is executed (NAIVE, OPT)
*/
void printHeaders(int numObjs, int capacity, int maxWeight, int maxValue, 
                  int blkDim, int which)
{
    printf("0-1 knapsack applied to %d objects and a knapsack capacity of %d.\n",
           numObjs, capacity);
    printf("Weights range from 0 to %d.\n", maxWeight);
    printf("Values range from 0 to %d.\n", maxValue);
    if (which == NAIVE)
    {
        printf("Using naive kernel with a single block of %d threads.\n", blkDim);
    } else if (which == OPT)
    {
        printf("Using optimized kernel with a single block of %d threads.\n", blkDim);
    }
}

/* 
    parseArgs
    This function parses the command line arguments to obtain
    the parameters used to solve the knapsack problem.
    If the parameters are in error then this function displays
    usage information and exits.
    Inputs:
    argc - count of the number of command line arguments
    argv - array of command line arguments
    numObjsP - pointer to an int to be set to the number of objects 
    capacityP - pointer to an int to be set to the capacity of the knapsack
    blkDimP - pointer to an int to be set to the block dimensions
    whichP - which kernel to execute
    doTimeP - pointer to a bool that is set to true or false if timing
              is to be performed
*/
void parseArgs(int argc, char * argv[], int * numObjsP, int * capacityP,
              int * blkDimP, int * whichP, bool * doTimeP)
{
    int i;
    //set the parameters to their defaults
    int numObjsExp = NUMOBJS_DEFAULT;
    int capacityExp = CAPACITY_DEFAULT;
    int blkDim = BLKDIM_DEFAULT;
    int which = WHICH_DEFAULT;
    bool doTime = false;

    //loop through the command line arguments
    for (i = 1; i < argc; i++)
    {
       if (i < argc - 1 && strcmp(argv[i], "-n") == 0)
       {
          numObjsExp = atoi(argv[i+1]);
          i++;   //skip over the argument after the -n
       }
       else if (i < argc - 1 && strcmp(argv[i], "-b") == 0)
       {
          blkDim = atoi(argv[i+1]);
          i++;   //skip over the argument after the -b
       }
       else if (i < argc - 1 && strcmp(argv[i], "-c") == 0)
       {
          capacityExp = atoi(argv[i+1]);
          i++;   //skip over the argument after the -c
       }
       else if (strcmp(argv[i], "-naive") == 0)
          which = NAIVE;
       else if (strcmp(argv[i], "-opt") == 0)
          which = OPT;
       else if (strcmp(argv[i], "-time") == 0)
          doTime = true;
       else
          printUsage();
    }

    //check if the provided parameters are correct
    if (!check("block dimensions", blkDims, BLKDIMARR, blkDim)) printUsage();
    if (!check("number of objects", numObjs, NUMOBJARR, numObjsExp)) printUsage();
    if (!check("capacity", capacities, CAPACITYARR, capacityExp)) printUsage();

    if (numObjsExp + capacityExp > 30)
    {
        printf("Sum of -n option and -c option must be less than 31 or malloc fails.\n");
        printUsage();
    } 

    (*blkDimP) = blkDim;
    (*capacityP) = 1 << capacityExp;
    (*numObjsP) = 1 << numObjsExp;
    (*whichP) = which;
    (*doTimeP) = doTime;
}

/*
 * check
 * Takes as input an array of size len containing ints and
 * the int value what.  If what is in the array, the function returns 1.
 * Otherwise, the function prints an error message and returns 0.
 * This function is used to check command line arguments. A valid
 * input can be found in the values array. 
 * Inputs
 * label - printed with the error message
 * values - array of legal values
 * len - length of array 
 * what - value to search for in values array
 * Returns
 * 1 - legal what value
 * 0 - illegal what value
*/
int check(const char * label, int * values, int len, int what)
{
   int i;
   for (i = 0; i < len; i++)
      if (values[i] == what) return 1;
   printf("Invalid %s - valid values are: ", label);
   for (i = 0; i < len; i++) printf("%d ", values[i]);
   printf("\n");
   return 0;
}

/*
    printUsage
    prints usage information and exits
*/
void printUsage()
{
    printf("\nThis program performs the knapsack of an array\n");
    printf("of size n by n using the CPU and one of two CUDA routines\n");
    printf("usage: knapsack [-n <n> | -b <b> | -c <c> | -naive | -opt | -time] \n");
    printf("       2**<n> is the number of objects\n");
    printf("              default <n> is %d\n", NUMOBJS_DEFAULT);
    printf("       <b> is the size of number of threads to use to solve the problem\n");
    printf("              default <b> is %d\n", BLKDIM_DEFAULT);
    printf("       2**<c> is the capacity of the knapsack\n");
    printf("              default <c> is %d\n", CAPACITY_DEFAULT);
    printf("       -naive use the naive CUDA version\n");
    printf("       -opt use the optimized CUDA version\n");
    printf("       -time implementation and output speedup\n");
    exit(EXIT_FAILURE);
}

/* 
    initArray
    Initializes an array of int of size
    length to random values between 1 and maxValue, inclusive.
    Inputs:
    array - pointer to the array to initialize
    length - length of array
    maxValue - maximum value for an element
*/
void initArray(int * array, int length, int maxValue)
{
    int i;
    for (i = 0; i < length; i++)
    {
        array[i] = (rand() % maxValue) + 1;
    }
}

/*
    compare
    Compares the values in two arrays and outputs an
    error message and exits if the values do not match.
    Inputs
    result1, result2 - arrays to be compared
    n - number of elements in each array 
*/
void compare(int * result1, int * result2, int n)
{
    int i;
    for (i = 0; i < n; i++)
    {
        if (result1[i] != result2[i])
        {
            printf("GPU knapsack does not match CPU results.\n");
            printf("cpu result[%d] = %d, gpu result[%d] = %d\n", 
                   i, result1[i], i, result2[i]);
            exit(EXIT_FAILURE);
        }
    }
}

