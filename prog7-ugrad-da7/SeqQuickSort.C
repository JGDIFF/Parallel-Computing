#include <string.h>
#include "SeqQuickSort.h"
#include "helpers.h"

/*
 * SeqQuickSort constructor
 * Initialize description.
 * Use constructor in parent Sorts class to initialize size and data.
 */
SeqQuickSort::SeqQuickSort(uint64_t size, int32_t * data):Sorts(size, data)
{
  description = "Sequential sort: quicksort recursively partitions list using a pivot point";
}

/*
 * sort
 * Takes as input an array of int32_t values and sorts them using 
 * a quicksort.
 * Input:
 * data - array of int32_t values
 * size - number of elements in the array
 * Modifies:
 * data - elements of data array in increasing sorted order
 */
double SeqQuickSort::sort()
{
  TIMERSTART(seq)
  quickSort(0, size - 1, data);
  TIMERSTOP(seq)
  /* return the amount of time taken */
  return GETTIME(seq);
}

/*
 * quickSort
 * Performs a sequential quicksort.
 * Inputs:
 * sIdx - starting index into the data to sort
 * eIdx - ending index into the data to sort
 * data - array of data to sort
 * Modifies:
 * data
 */
void SeqQuickSort::quickSort(int32_t sIdx, int32_t eIdx, int32_t * data)
{
  if (sIdx >= eIdx) return;

  //partition and return the index of the pivot
  int32_t mid = partition(sIdx, eIdx, data);
  quickSort(sIdx, mid - 1, data);
  quickSort(mid + 1, eIdx, data);
}

/*
 * partition
 * This function partitions the data in the array starting at index
 * sIdx and ending at eIdx into three sections.  One section contains
 * the values below a pivot. The middle section contains the single
 * pivot. The third section contains the values greater than the pivot.
 * At the end, the pivot is in the correct location in the final sorted array.
 * Inputs:
 * sIdx - starting index into the data to sort
 * eIdx - ending index into the data to sort
 * data - array of data to sort
 * Modifies:
 * data
 * Returns:
 * location of pivot
*/
int SeqQuickSort::partition(int32_t sIdx, int32_t eIdx, int32_t * data)
{
  //define a lambda expression to perform a swap
  auto swap = [](auto & a, auto & b)
  {
    auto t = a;
    a = b;
    b = t;
  };

  //define a lambda expression to choose the pivot
  auto choosePivotIndex = [](auto sIdx, auto eIdx, auto data)
  {
    if (sIdx == eIdx) return sIdx;
    int32_t mid = sIdx + (eIdx - sIdx)/2;
    if (data[sIdx] < data[mid] && data[mid] < data[eIdx])
      return mid;
    else if (data[mid] < data[sIdx] &&  data[sIdx] < data[eIdx])
      return sIdx;
    else
      return eIdx;
  };

  //choose a good pivot, get the index 
  int32_t pIdx = choosePivotIndex(sIdx, eIdx, data);

  //get the value of the pivot
  int32_t pivot = data[pIdx];

  //move pivot to end
  swap(data[pIdx], data[eIdx]);

  int32_t end = eIdx - 1;
  while (sIdx < end)
  {
    //from beginning skip over the values less than the pivot
    while (data[sIdx] < pivot && sIdx < end) sIdx++;
    //from end skip over the values greater than the pivot
    while (data[end] > pivot && sIdx < end) end--;
    //sIdx is pointing to a value greater than the pivot and
    //end is pointing to a value less than the pivot,
    //swap them
    if (sIdx < end)
    {
      swap(data[sIdx], data[end]);
      sIdx++;
      end--;
    }
  }
  //put the pivot in the right spot
  if (data[end] < pivot) end++;
  swap(data[end], data[eIdx]);
  return end;  //return location of pivot
}

