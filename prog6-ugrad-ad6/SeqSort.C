#include <string.h>
#include "SeqSort.h"
#include "helpers.h"

/*
 * SeqSort constructor
 * Initialize description.
 * Use constructor in parent Sorts class to initialize size and data.
 */
SeqSort::SeqSort(uint64_t size, int32_t * data):Sorts(size, data)
{
  description = "Sequential sort: loop through elements calculating the destination in result";
}

/*
 * sort
 * Takes as input an array of int32_t values and sorts them using the
 * code in problem 2, page 218 in textbook. 
 * Input:
 * data - array of int32_t values
 * size - number of elements in the array
 * threadCt - number of threads to use to perform the sort
 * Modifies:
 * data - elements of data array in increasing sorted order
 */
double SeqSort::sort()
{
  TIMERSTART(seq)
  int32_t * tmp = new int32_t[size];
  uint64_t i, j, count;

  /* loop through each element in the array */
  for (i = 0; i < size; i++)
  {
    /* figure out where data[i] goes */
    count = 0;
    for (j = 0; j < size; j++)
      if (data[j] < data[i] || (data[j] == data[i] && j < i))
      {
        count++;
      }
    /* store data[i] in destination */
    tmp[count] = data[i];
  }
  memcpy(data, tmp, size * sizeof(int32_t));
  TIMERSTOP(seq)
  /* return the amount of time taken */
  return GETTIME(seq);
}

