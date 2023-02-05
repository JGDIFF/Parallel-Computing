#include <string.h>
#include "SeqMergeSort.h"
#include "helpers.h"

/*
 * SeqMergeSort constructor
 * Initialize description.
 * Use constructor in parent Sorts class to initialize size and data.
 */
SeqMergeSort::SeqMergeSort(uint64_t size, int32_t * data):Sorts(size, data)
{
  description = "Sequential sort: mergesort recursively divides list in half and merges";
}

/*
 * sort
 * Takes as input an array of int32_t values and sorts them using 
 * a mergesort.
 * Input:
 * data - array of int32_t values
 * size - number of elements in the array
 * threadCt - number of threads to use to perform the sort
 * Modifies:
 * data - elements of data array in increasing sorted order
 */
double SeqMergeSort::sort()
{
  TIMERSTART(seq)

  //This array is used for the merge process
  int32_t * tmp = new int32_t[size];
  mergeSort(0, size-1, data, tmp);
  delete [] tmp;
  TIMERSTOP(seq)
  /* return the amount of time taken */
  return GETTIME(seq);
}

/* 
 * mergeSort
 * Performs a sequential mergeSort.
 * Inputs:
 * sIdx - starting index into data of chunk to be sorted
 * eIdx - ending index into data of the chunk to be sorted
 * data - contains data be sorted
 * tmp - array passed to the merge to be used for merging
 * Modfies:
 * data array
*/
void SeqMergeSort::mergeSort(int32_t sIdx, int32_t eIdx, int32_t * data, int32_t * tmp)
{
  if (sIdx >= eIdx) return;

  int32_t half = (eIdx - sIdx + 1) >> 1;
  int32_t mid = sIdx + half;
  //sort each half of the array
  mergeSort(sIdx, sIdx + half - 1, data, tmp);
  mergeSort(sIdx + half, eIdx, data, tmp);
  //merge the two sorted halves
  merge(sIdx, mid, eIdx, data, tmp);
}

/*
 * merge
 * Takes as input indices into an array that defines two sections of
 * the array that are in sorted order. Overwrites those two sections
 * of the array with the merge of those two sections.
 * Inputs:
 * sIdx1 - lower index into first section (section ends at mid - 1)
 * mid - lower index into second section
 * eIdx2 - end index of second section
 * data - array containing the two sections to be merged
 * tmp - array to use to temporarily hold the merged result
 * Modifies:
 * data - elements in the range sIdx1 to eIdx2 will be in sorted order
*/
void SeqMergeSort::merge(int32_t sIdx1, int32_t mid, int32_t eIdx2, 
                         int32_t * data, int32_t * tmp)
{
  int32_t count = (eIdx2 - sIdx1) + 1;  //number of values to merge
  int32_t destIdx = sIdx1;              //destination in data array
  int32_t sIdx2 = mid;                  //starting index in second section

  //tmp is used to temporarily hold the merged results
  for (int32_t i = 0; i < count; i++ )
  {
    if (sIdx1 == mid) //reached end of first chunk
    {
      tmp[i] = data[sIdx2];
      sIdx2++;
    } else if (sIdx2 > eIdx2) //reached end of second chunk
    {
      tmp[i] = data[sIdx1];
      sIdx1++;
    }
    else if (data[sIdx1] < data[sIdx2])
    {
      tmp[i] = data[sIdx1];
      sIdx1++;
    } else
    {
      tmp[i] = data[sIdx2];
      sIdx2++;
    }
  }
  //copy the merged results back to data
  memcpy(&data[destIdx], tmp, count * sizeof(int));
}
     
   
