#include <string.h>
#include <omp.h>
#include "ParaMergeSort.h"
#include "helpers.h"

/*
 * ParaMergeSort constructor
 * Initialize threadCt and description.
 * Use constructor in parent Sorts class to initialize size and data.
 */
ParaMergeSort::ParaMergeSort(uint64_t size, int32_t * data, int32_t threadCt):Sorts(size, data)
{
  this->threadCt = threadCt;
  description = "Parallel sort: parallelizes mergesort by creating tasks for recursive calls and merge";
}

/*
 * sort
 * Sorts an array of int32_t values using OpenMP tasks. 
 * Inputs:
 * data - array of int32_t values
 * size - number of elements in the array
 * threadCt - number of threads to use to perform the sort
 * Modifies:
 * data - elements of data array in increasing sorted order
 */
double ParaMergeSort::sort()
{
  //The sequential version is provided here. You need to
  //parallelize this version.  
  //1) You'll need to create threadCt threads. Use only one of those threads
  //   to make the first call to mergeSort.
  //2) You'll need a bigger tmp array because you will have multiple threads
  //   writing to the tmp array simultaneously. Each thread should have its
  //   own chunk of size elements in the tmp array.
  TIMERSTART(para)

  //You'll need to make changes to this code.  See above. 

  int32_t * tmp = new int32_t[size * threadCt];
  #pragma omp parallel num_threads(threadCt)
  #pragma omp single
  mergeSort(0, size-1, data, tmp);
  delete [] tmp;
  TIMERSTOP(para)

  return GETTIME(para);
}

/* 
 * mergeSort
 * Performs a parallel mergeSort.
 * Inputs:
 * sIdx - starting index into data of chunk to be sorted
 * eIdx - ending index into data of the chunk to be sorted
 * data - contains data be sorted
 * tmp - array passed to the merge to be used for merging
 * Modfies:
 * data array
*/
void ParaMergeSort::mergeSort(int32_t sIdx, int32_t eIdx, int32_t * data, int32_t * tmp)
{

  //3) Create a task for the recursive calls to mergeSort and the call to merge. 
  //   However, you don't want to create a task for every recursive call or 
  //   you'll have a huge number of tasks with very little to do. Instead, keep
  //   track of the number of recursive calls to mergeSort and use that to decide
  //   whether a task should be created or not. Don't create more
  //   tasks for the recursive calls than double the number of threads. If you don't 
  //   create a task to do the mergeSort, then the calling thread will need to do it.
  //   (Feel to play around and determine a better measurement than "double.")
  //4) You'll have multiple threads updating the task count.  Prevent a race.
  //5) You need to make sure that the merge isn't performed until 
  //   after the two calls to mergeSort are complete. A taskwait helps with this.
  //6) You need to make a mergeSort wait for its descendant mergeSorts to complete.
  //   A taskgroup can provide that. (Google this.)
  //
  if (sIdx >= eIdx) return;
  
  //You'll need to make changes to the code below.  See above.
  
  static int taskCt = 0;  //Static will make it shared among all threads (uncomment declaration when ready)
  int32_t count = (eIdx - sIdx + 1);
  int32_t half = count >> 1;
  int32_t mid = sIdx + half;

  //Need an if here based upon the task count.
  //The calls to mergeSort and merge will be in both the if and the else
  //but one of those will create a taskgroup. Inside the task group pragma
  //will be task pragmas.
  if (taskCt < (2 * threadCt)) 
  {
    #pragma omp taskgroup
    {
	    #pragma omp atomic
		    taskCt+=2;
      #pragma omp task
      {
      	mergeSort(sIdx, sIdx + (half - 1), data, tmp);
      }
      #pragma omp task
      {
      	mergeSort(sIdx + half, eIdx, data, tmp);
      }
      #pragma omp taskwait

      #pragma omp task
        merge(sIdx, mid, eIdx, data, tmp);
    }
  } else {
      mergeSort(sIdx, sIdx + half - 1, data, tmp);
      mergeSort(sIdx + half, eIdx, data, tmp);
      merge(sIdx, mid, eIdx, data, tmp);
    }
 
 
  
}

/*
 * merge
 * Takes as input indices into an array that defines two sections of
 * the array that are in sorted order. Overwrites those two sections
 * of the array with the merge of those two sections.
 * Inputs:
 * sIdx1 - lower index into first section (ends at mid - 1)
 * mid - lower index into second section
 * eIdx2 - end index of second section
 * data - array containing the two sections to be merged
 * tmp - array to use to temporarily hold the merged result
 * Modifies:
 * data - elements in the range sIdx1 to eIdx2 will be in sorted order
*/
void ParaMergeSort::merge(int32_t sIdx1, int32_t mid, int32_t eIdx2, 
                          int32_t * data, int32_t * tmp)
{

  //This code requires slight modification to use a larger tmp array.
  //Each thread will be using its own chunk within the tmp array to hold
  //the merged results.
  //
  int32_t count = (eIdx2 - sIdx1) + 1;
  int32_t destIdx = sIdx1;
  int32_t sIdx2 = mid;
  int32_t threadIdx = omp_get_thread_num();  ///get the thread number
  
  //There is a way to parallelize the merge, but it requires more
  //significant modifications. We're not going to do that.
  for (int32_t i = 0; i < count; i++ )
  {
    if (sIdx1 == mid) //reached end of first chunk
    {
      tmp[i + (threadIdx * size)] = data[sIdx2];
      sIdx2++;
    } else if (sIdx2 > eIdx2) //reached end of second chunk
    {
      tmp[i + (threadIdx * size)] = data[sIdx1];
      sIdx1++;
    }
    else if (data[sIdx1] < data[sIdx2])
    {
      tmp[i + (threadIdx * size)] = data[sIdx1];
      sIdx1++;
    } else
    {
      tmp[i + (threadIdx * size)] = data[sIdx2];
      sIdx2++;
    }
  }
  //copy the merged results back to data; this may change depending 
  //upon how you get a thread to access its portion of the tmp
  //array (You can do that by modifying tmp or adding an offset to tmp)
  memcpy(&data[destIdx], tmp+ (threadIdx * size), count * sizeof(int));
}
   
