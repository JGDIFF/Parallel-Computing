#include <string.h>
#include "ParaSort3.h"
#include "helpers.h"

/*
 * ParaSort3 constructor
 * Initialize threadCt and description.
 * Use constructor in parent Sorts class to initialize size and data.
 */
ParaSort3::ParaSort3(uint64_t size, int32_t * data, int32_t threadCt):Sorts(size, data)
{
  this->threadCt = threadCt;
  description = "Parallel Sort 3: Parallelizes the calculation of the destination of an element\n";
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

double ParaSort3::sort()
{
  /* The first and second versions of the sort parallelized the outer most
   * loop.  However, the innermost loop can also be parallelized.  That is,
   * instead of using only one thread to determine the destination of data[i],
   * multiple threads can be doing that and their counts can be added together
   * (a reduction).
   * In this version, you'll parallelize the outermost for loop and create 
   * threadCt threads to do it.  And, you'll parallelize the innermost for loop
   * by creating 8 threads to do that (that is, 8 threads per each iteration of
   * the i loop). Treat the array so that it is divided into 8 chunks so that each
   * of the threads calculates count for one chunk of the array. Those counts need
   * to be added together (a reduction) to calculate the destination of data[i].  
   * (Basically, you'll want to put the j loop inside of another loop that is executed 
   * 8 times.  But the j loop indices won't range from 0 to size - 1.  They'll only iterate through
   * 1/8th of the array. 
   * Use a lambda expression for clean code. That lambda expression will be executed
   * by a thread to figure out the value of count for some X[i] and 1/8 of the
   * array.  (All counts are added together to determine the final value for count.)
   *
   */

  TIMERSTART(para)

  uint64_t i, j, count, N = size;
  int32_t * tmp = new int32_t[size];
  j = 0;
  count = 0;

  auto myFunction = [&] (uint64_t i, uint64_t j, uint64_t N, int32_t * tmp) {
    if (data[j] < data[i] || (data[j] == data[i] && j < i)) {
      count++;
    }
  };


  #pragma omp parallel num_threads(threadCt)
  for (i = 0; i < N; i++) 
  {
    myFunction(i, j, N, tmp);
    count = 0;
     #pragma omp parallel num_threads(8) for reduction(+:count) 
      for (j = 0; j < N/8; j++)
      {
        myFunction(i, j, N, tmp);
      }
      tmp[count] = data[i];
  }
  memcpy(data, tmp, size * sizeof(int32_t));

/**
  #pragma omp parallel num_threads(threadCt)
  for (i = 0; i < N; i++) {
    myFunction(i, j,  N, tmp);
    count = 0;
    #pragma omp parallel num_threads(8)
    #pragma omp task shared(count)
    #pragma omp taskloop
    for (j = 0; j < N/8; j++)
    {
      myFunction(i, j, N, tmp);
    }
    tmp[count] = data[i];

  }
  memcpy(data, tmp, size * sizeof(int32_t));
  **/

  TIMERSTOP(para)
  return GETTIME(para);
}

