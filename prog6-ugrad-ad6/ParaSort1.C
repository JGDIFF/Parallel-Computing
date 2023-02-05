#include <string.h>
#include "ParaSort1.h"
#include "helpers.h"

/*
 * ParaSort1 constructor
 * Initialize threadCt and description.
 * Use constructor in parent Sorts class to initialize size and data.
 */
ParaSort1::ParaSort1(uint64_t size, int32_t * data, int32_t threadCt):Sorts(size, data)
{
  this->threadCt = threadCt;
  description = "Parallel Sort 1: determine destinations of a group of elements of the array in parallel.\n";
}

/*
 * sort
 * Sorts an array of int32_t values using OpenMP threads. 
 * Input:
 * data - array of int32_t values
 * size - number of elements in the array
 * threadCt - number of threads to use to perform the sort
 * Modifies:
 * data - elements of data array in increasing sorted order
 */
double ParaSort1::sort()
{
  /* This code is the most straightforward modification of the code in
   * problem 2, page 218, to make it parallel. However. an array of int32_t 
   * types are used instead of a vector. You'll need to dynamically allocate
   * a tmp array and then copy that into data after the sort. Use a memcpy.
   */
  TIMERSTART(para)

  uint64_t i, j, count, N = size; 
  int32_t * tmp = new int32_t[size];

  //Use a parallel for pragma with threadCt threads to parallelize the outermost loop
  //Need to be careful about not sharing variables.  
  #pragma omp parallel for num_threads(threadCt) 
  for (i = 0; i < N; i++) {
    count = 0;
    for (j = 0; j < N; j++) {
      if (data[j] < data[i] || (data[j] == data[i] && j < i)) {
        count++;
      }
    }
    tmp[count] = data[i];
  }
  memcpy(data, tmp, size * sizeof(int32_t));
  
  TIMERSTOP(para)
  return GETTIME(para);
}

