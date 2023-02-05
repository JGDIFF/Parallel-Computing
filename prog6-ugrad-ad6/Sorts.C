#include <stdio.h>
#include "Sorts.h"

/*
 * Sorts constructor
 * Takes as input an array of size int32_t values and dynamically
 * allocates an array of the same size. It then initializes the
 * dynamically allocated array to the values in the input array.
 * Inputs:
 * size_: size of the input array
 * input: pointer to the input array
 */
Sorts::Sorts(uint64_t size_, int32_t * input)
{
  size = size_;
  data = new int32_t[size];
  for (int64_t i = 0; i < (long int) size; i++)
  { 
    data[i] = input[i];
  }
}

/*
 * increasing
 * This can be used after a sort is performed to see if the data array is
 * in increasing order. (Increasing means the elements cannot all be the
 * same.  For example, an array of all 0s would not be increasing.)
 * Output:
 * bool: true if the data array is in increasing order
 *       false, otherwise
 */
bool Sorts::increasing()
{
  /* This is used to catch whether the sorted results are all 0, which
   * means the sort failed.  A better test is to compare to the known
   * result using the match function.
   */
  bool increasingFlag = false;
  for (int64_t i = 1; i < (long int) size; i++)
  { 
    if (data[i-1] < data[i]) 
    {
      increasingFlag = true;
    } 
  }
  if (increasingFlag == false)
  {
    printf("array is not in increasing order\n");
    return false;
  }
  return true;
}  

/* 
 * getDescription
 * Returns the description associated with the sort object.
 */
std::string Sorts::getDescription()
{
  return description;
}

/*
 * match
 * Takes as input a pointer to a sorts objects and compares its data
 * array to the data of this object.  Returns true if they match
 * and false otherwise.
 * Input:
 * sortObj: pointer to a Sorts object
 * Output:
 * bool: true if sortObj->data contents and size matches this->data
*/
bool Sorts::match(Sorts * sortObj)
{
  if (sortObj->size != size)
  {
    printf("arrays have different sizes: %ld != %ld\n", size, sortObj->size);
    return false;
  } 
  for (int64_t i = 0; i < (long int) size; i++)
  { 
    if (data[i] != sortObj->data[i])
    {
      printf("sort results have different values at index %ld: %d != %d\n", 
             i, data[i], sortObj->data[i]);
      return false;
    }
  }
  return true;
}

/*
 * Sorts destructor
 * Used to delete the dynamically allocated data array
 */
Sorts::~Sorts()
{
  delete [] data;
}
