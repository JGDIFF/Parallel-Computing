#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cuda_runtime.h>
#include "helpers.h"
#include "d_knapsack.h"

//prototypes for kernels in this file
__global__ 
void d_knapsackNaiveKernel(int * d_best, int * d_weight, int * d_values, int numObjs,
                           int capacity);

__global__ 
void d_knapsackOptKernel(int * d_best, int * d_weight, int * d_values, int numObjs,
                           int capacity);

/*  d_knapsack
    This function prepares and invokes a kernel to solve the 0-1 knapsack problem
    on the GPU. The input to the knapsack problem is a set of objects and a 
    knapsack capacity.  Each object has a weight and a value. The solution chooses a subset 
    of the objects that maximums the overall value while not exceeding the capacity.
    Inputs:
    result - points to an array to hold the knapsack result
    weights - points to an array that holds the weights of the objects
    values - points to an array that holds the values of the objects
    numObjs - number of objects (size of values and weights arrays)
    capacity - the capacity of the knapsack
    blkDim - the number of threads in the block of threads used to solve the problem
    which - indicates which kernel to use to solve the problem (NAIVE, OPT)
*/
float d_knapsack(int * result, int * weights, int * values, int numObjs, 
                 int capacity, int blkDim, int which)
{
    int * d_best, * d_weights, * d_values;  //pointers to arrays for GPU
   
    //CUERR is a macro in helpers.h that checks for a Cuda error 
    //Begin the timing (macro in helpers.h) 
    TIMERSTART(gpuTime)

    //Allocate space in GPU memory for weights array 
    cudaMalloc((void **)&d_weights, sizeof(int) * numObjs);             CUERR
    //Copy weights from CPU memory to GPU memory
    cudaMemcpy(d_weights, weights, sizeof(int) * numObjs, H2D);         CUERR

    //Allocate space in GPU memory for values array 
    cudaMalloc((void **)&d_values, sizeof(int) * numObjs);              CUERR
    //Copy values from CPU memory to GPU memory
    cudaMemcpy(d_values, values, sizeof(int) * numObjs, H2D);           CUERR

    //Launch the appropriate kernel
    if (which == NAIVE)
    {
        //Allocate space in GPU memory for best array
        int bestSz = (numObjs + 1) * (capacity + 1);
        cudaMalloc((void **)&d_best, sizeof(int) * bestSz);             CUERR
        //set the best matrix to 0
        cudaMemset((void *)d_best, 0, bestSz * sizeof(int));            CUERR
        //define the block and the grid and launch the naive kernel
        dim3 block(blkDim, 1, 1);
        dim3 grid(1, 1, 1);
        d_knapsackNaiveKernel<<<grid, block>>>(d_best, d_weights, d_values,
                                               numObjs, capacity);     CUERR
        //copy last row of d_best array into result
        cudaMemcpy(result, 
                   &d_best[numObjs * (capacity + 1)], sizeof(int) * (capacity + 1),
                   D2H);                                               CUERR
    } else if (which == OPT)
    {
        //TO DO
        //Provide the code that is missing to execute the optimized kernel
        //1) Allocate space for the best array. 
        //   Note best array size is not the same naive best array.
        //2) Set best array elements to 0
        //3) Define block and grid
        //4) Launch the kernel
        //5) Copy best matrix into CPU result array

	//Allocate space in GPU memory for best array
        int bestSz = (2) * (capacity + 1);
        cudaMalloc((void **)&d_best, sizeof(int) * bestSz);             CUERR
        //set the best matrix to 0
        cudaMemset((void *)d_best, 0, bestSz * sizeof(int));            CUERR
        //define the block and the grid and launch the naive kernel
        dim3 block(blkDim, 1, 1);
        dim3 grid(1, 1, 1);
        d_knapsackOptKernel<<<grid, block>>>(d_best, d_weights, d_values,
                                               numObjs, capacity);     CUERR
        //copy last row of d_best array into result
        cudaMemcpy(result, 
                   &d_best[0], sizeof(int) * (capacity + 1),
                   D2H);                                               CUERR

    }
    //free dynamically  allocated memory
    cudaFree(d_best);                                                 CUERR
    cudaFree(d_values);                                               CUERR
    cudaFree(d_weights);                                              CUERR

    //stop the timer
    TIMERSTOP(gpuTime)
    return TIMEELAPSED(gpuTime)
}

/*  
    d_knapsackNaiveKernel
    This kernel solves the knapsack problem using a naive kernel.
    Inputs:
    best - pointer to the array in which the result is stored
    weights - points to an array that holds the weights of the objects
    values - points to an array that holds the values of the objects
    numObjs - number of objects (size of values and weights arrays)
    capacity - the capacity of the knapsack
*/

__global__
void d_knapsackNaiveKernel(int * best, int * weights, int * values, 
                           int numObjs, int capacity)
{
    //TO DO

    //You should base this implementation on the CPU version in h_knapsack.cu. 
    //1) Do not allocate and initalize the best array here. That was done before
    //   the kernel launch.
    //2) Each thread will loop through the number of rows (just like CPU version)
    //3) In the inner, loop cyclic partitioning will be used to divide up the 
    //   the row elements (the columns) among the threads.  For example, 
    //   thread 0 will calculate the 0th element, the blockDim.x element, 
    //   the 2*blockDim.x element, etc in the ith row of best. 
    //   (Note that best is a 1D array holding 2D data.)
    //   Thus, all threads of a block will cooperate in producing one row (i) 
    //   of results.
    //4) Block synchronization is needed so the threads in a block won't continue
    //   onto next row until all threads are finished with the current row.
    int i, j;
    int nRows = numObjs + 1;
    int nCols = capacity + 1;
    
    for (i = 1; i < nRows; i++) 
    {
	for (j = threadIdx.x; j < nCols; j += blockDim.x) 
	{
	    if (weights[i - 1] <= j)
	    {
	    	best[i * nCols + j] = MAX(best[(i - 1) * nCols + j],
                                          best[(i - 1) * nCols  + j - weights[i - 1]] + values[i - 1]);
            } else {
	        best[i * nCols + j] = best[(i - 1) * nCols + j];
	    }
	}
	__syncthreads();
    }
}

/*  
    d_knapsackOptKernel
    This kernel solves the knapsack problem using an optimized kernel.
    Inputs:
    best - pointer to the array in which the result is stored
    weights - points to an array that holds the weights of the objects
    values - points to an array that holds the values of the objects
    numObjs - number of objects (size of values and weights arrays)
    capacity - the capacity of the knapsack
*/
__global__ 
void d_knapsackOptKernel(int * best, int * weights, int * values, int numObjs,
                         int capacity)
{
    //TO DO

    //For this one, you'll start with the naive kernel code and improve it.
    //1) Note all of the accesses to weights[i-1] and values[i-1] that occur in
    //   in the inner most loop.  Those are all accesses to global memory.  You want
    //   to change those so that they are either accesses to the faster
    //   shared memory or to registers. (This is a really easy fix and one that
    //   programmers should always do whether for CPU code or GPU code.)
    //2) Use less global memory for the best array.  Note that the last row
    //   of the best array is what is copied into result. And each row is computed
    //   using the values in the previous row. Thus, this code can
    //   be implemented with just two rows in the best array.

    int i, j;
    int nRows = numObjs + 1;
    int nCols = capacity + 1;
    int srcI = 0;
    int dstI = 1;
    int tmp = 0; 
    
    for (i = 1; i < nRows; i++) 
    {
	for (j = threadIdx.x; j < nCols; j += blockDim.x) 
	{
	    if (weights[i - 1] <= j)
	    {
	    	best[dstI * nCols + j] = MAX(best[(srcI) * nCols + j],
                                          best[(srcI) * nCols  + j - weights[i - 1]] + values[i - 1]);
            } else {
	        best[dstI * nCols + j] = best[(srcI) * nCols + j];
	    }
	}
	tmp = srcI; 
	srcI = dstI;
	dstI = tmp;
	__syncthreads();
    }
}

