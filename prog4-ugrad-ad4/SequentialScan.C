#include <stdio.h>
#include <vector>
#include <chrono>
#include "helpers.h"
#include "SequentialScan.h"

/*
 * SequentialScan
 * A copy of the input vector is made and those resources
 * are moved to this->nums
 */
SequentialScan::SequentialScan(std::vector<int> nums)
{
  /* This prevents another copy of the vector from being */
  /* made. The resources of nums are moved to this->nums. */
  this->nums = std::move(nums);
}

/* 
 * performScan
 * Perform a prefix scan on nums, modifying the nums vector.
 * Return the time it takes to perform the scan.
 * Example: if nums is initially
 * {1, -1, 2, 1, 3}
 * then after the scan nums is
 * {1, 0, 2, 3, 6}
 */
float SequentialScan::performScan()
{
  //start timing the scan (see helpers.h)
  TIMERSTART(sequential)

  for (long int i = 1; i < (long int) nums.size(); i++)
    nums[i] = nums[i] + nums[i - 1];

  //stop timing the scan
  TIMERSTOP(sequential)

  //return the difference between the starting time
  //and the ending timing
  return GETTIME(sequential);
}

/*
 * compare
 * Compares an input vector to the member vector.
 * Outputs an error message and returns 1 if they are
 * are not identical.  If they are identical, returns 0.
 */
int SequentialScan::compare(std::vector<int> &inputNums)
{
  //make sure they are the same size
  if (inputNums.size() != nums.size())
  {
    printf("Arrays have different sizes: %ld != %ld\n", nums.size(), 
            inputNums.size());
    return 1;
  }

  //make sure they contain the same values
  for (long int i = 0; i < (long int) nums.size(); i++)
  {
    if (inputNums[i] != nums[i])
    {
      printf("mismatch: sequentialArray[%ld] = %d, ",
             i, nums[i]);
      printf("threadedArray[%ld] = %d\n",
             i, inputNums[i]);
      return 1;
    }
  }

  //the vectors match
  return 0;
}

