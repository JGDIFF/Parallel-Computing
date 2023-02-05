#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cuda_runtime.h>
#include "helpers.h"
#include "h_knapsack.h"
#include "wrappers.h"

//for printing out the entire best array
#define DEBUG 0

//prototype for function local to this file
static void knapsackOnCPU(int * result, int * weights, int * values, int numObjs, int capacity); 

/*  h_knapsack
    This function prepares and invokes a kernel to solve the 0-1 knapsack problem
    on the CPU. The input to the knapsack problem is a set of objects and a 
    knapsack capacity.  Each object has a weight and a value. The solution chooses a subset 
    of the objects that maximums the overall value while not exceeding the capacity.
    Inputs:
    result - points to an array to hold the knapsack result
    weights - points to an array that holds the weights of the objects
    values - points to an array that holds the values of the objects
    numObjs - number of objects (size of values and weights arrays)
    capacity - the capacity of the knapsack
*/
float h_knapsack(int * result, int * weights, int * values, int numObjs, int capacity) 
{
    cudaEvent_t start_cpu, stop_cpu;
    float cpuMsecTime = -1;

    //CUERR is a macro defined in helpers.h that checks for a CUDA error
    //Use CUDA functions to do the timing 
    //Create event objects
    cudaEventCreate(&start_cpu);                       CUERR
    cudaEventCreate(&stop_cpu);                        CUERR
    //record the starting time
    cudaEventRecord(start_cpu);                        CUERR
    
    //call function that does the actual work
    knapsackOnCPU(result, weights, values, numObjs, capacity);
   
    //record the ending time and wait for event to complete
    cudaEventRecord(stop_cpu);                              CUERR
    cudaEventSynchronize(stop_cpu);                         CUERR

    //calculate the elapsed time between the two events 
    cudaEventElapsedTime(&cpuMsecTime, start_cpu, stop_cpu); CUERR 
    return cpuMsecTime;
}

/*  knapsackOnCPU
    This function performs the 0-1 knapsack on the CPU.  
    Inputs:
    result - points to an array to hold the knapsack result
    weights - points to an array that holds the weights of the objects
    values - points to an array that holds the values of the objects
    numObjs - number of objects (size of values and weights arrays)
    capacity - the capacity of the knapsack
*/
void knapsackOnCPU(int * result, int * weights, int * values, int numObjs, int capacity) 
{
    int i, j; 
    int nRows = numObjs + 1;
    int nCols = capacity + 1;

    //allocate space for the best array`
    int bestArraySz = (nRows * nCols);
    int * best = (int *) Malloc(sizeof(int) * bestArraySz);

    //initialize it 
    memset(best, 0, sizeof(int) * bestArraySz);

    //solve the knapsack problem using dynamic programming
    for (i = 1; i < nRows; i++) 
    {
        for (j = 0; j < nCols; j++)
        {
            if (weights[i - 1] <= j)
            {
                best[i * nCols + j] = MAX(best[(i - 1) * nCols + j],
                                          best[(i - 1) * nCols  + j - weights[i - 1]] + values[i - 1]); 
            } else
            {
                best[i * nCols + j] = best[(i - 1) * nCols + j];
            }
        }
    }
    if (DEBUG)
    {
        for (i = 0; i < nRows; i++) 
        {
            for (j = 0; j < nCols; j++)
            {
                printf("%4d ", best[i * nCols + j]);
            }
            printf("\n");
        }
    }
    
    //copy last row into result array
    for (i = 0; i < nCols; i++) result[i] = best[numObjs * nCols + i];
}
