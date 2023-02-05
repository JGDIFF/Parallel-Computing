#include <string.h>
#include "ParaSort2.h"
#include "helpers.h"

/*
 * ParaSort2 constructor
 * Initialize threadCt and description.
 * Use constructor in parent Sorts class to initialize size and data.
 */
ParaSort2::ParaSort2(uint64_t size, int32_t * data, int32_t threadCt):Sorts(size, data)
{
  this->threadCt = threadCt;
  description = "Parallel Sort 2: create one task for each element\n";
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
double ParaSort2::sort()
{

  /* The parallelization in this code is similar to the parallelization
   * of the sort in ParaSort1 (which is based on problem 2, page 218). However,
   * instead of using a parallel for, it should create a task to handle
   * one iteration of the outer loop.  Thus, it will create size tasks.
   * In addition, threadCt threads will be created to handle the size tasks.
   * Task parallelism is covered in section 6.6.
   *
   * Use a lambda expression for clean code.  The lambda should be executed
   * by the task.
   */
  TIMERSTART(para)

  uint64_t i, N = size;
  int32_t * tmp = new int32_t[size];

  auto myFunction = [&] (uint64_t i, uint64_t N, int32_t * tmp) {
    uint64_t count = 0;
    uint64_t j = 0;
    for (j = 0; j < N; j++)
    {
      if (data[j] < data[i] || (data[j] == data[i] && j < i)) {
        count++;
      }
    }
    tmp[count] = data[i];
  };

  #pragma omp parallel num_threads(threadCt)
  #pragma omp single
  #pragma omp taskloop
  for (i = 0; i < N; i++) {
    myFunction(i, N, tmp);
  }
  memcpy(data, tmp, size * sizeof(int32_t));	




/**	
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
**/







  TIMERSTOP(para)
  return GETTIME(para);
}

