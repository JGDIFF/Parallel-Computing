#include <vector>
#include <math.h>
#include <chrono>
#include "helpers.h"
#include "ThreadPool.h"
#include "ThreadedScan.h"

/*
 * ThreadedScan
 * Initializes the private data members of the ThreadedScan object.
 */ 
ThreadedScan::ThreadedScan(std::vector<int> nums, uint64_t numThreads, 
                           uint64_t subarraySize)
{
  this->numThreads = numThreads;
  this->subarraySize = subarraySize;
  //Use std::move to prevent another copy of nums being made.
  //Moves the nums resources to this->nums.
  this->nums = std::move(nums);
  this->pool = new ThreadPool(numThreads);
  this->activeThreads = 0;
}

/*
 * performScan
 * This function performs the parallel prefix scan that is
 * described in the textbook on pages 25 and 26 (Listing 2.2).
 * It uses the ThreadPool implementation to complete the
 * various tasks.
 *
 * You should write some helper lambda functions.
 */
float ThreadedScan::performScan()
{ 

  /* WORK TO DO IN THIS FUNCTION */

	  printf("nums before anything: \n");
  for (int x : nums)
  	 std::cout << x << " ";
  printf("\n");

  TIMERSTART(threaded)

  //Function 1 for step 1
  auto func1 = [&] (uint64_t id) {
    for (uint64_t j = 1; j < subarraySize; j++) {
      nums[id * subarraySize + j] += nums[id * subarraySize + j - 1];
    }
  };

//  printf("nums after 1 \n");
//  for (int x : nums)
//    std::cout << x << " ";
//  printf("\n");

  //Fiunction 2 for step 3
  auto func2 = [&] (uint64_t id) {
    for (uint64_t j = 0; j < subarraySize - 1; j++) {
      nums[(id) * subarraySize + j] += nums[id * subarraySize - 1]; 
    }
  };

  //Step 1:
  //Create the tasks to be performed and add them to the thread
  //pool using the enqueue function.
  //Ignore the future object returned by enqueue since it isn't needed
  for (uint64_t i = 0; i < nums.size()/subarraySize; i++) {
    pool->enqueue(func1, i);
    //Help
  } 

  //wait until the number of tasks to be completed becomes 0
  //You'll need to implement this function in the ThreadPool class.
  pool->waitForZeroTasks();

	  printf("nums after 1 \n");
  for (int x : nums)
    std::cout << x << " ";
  printf("\n");

  //Step 2:
  //Main thread performs prefix scan of right most values in each subarray
  for (uint64_t p = 1; p <= numThreads; p++) {
    nums[p * subarraySize - 1] += nums[(p - 1) * subarraySize - 1];
  }  

  printf("nums after 2 \n");
  for (int x : nums)
    std::cout << x << " ";
  printf("\n");

 
  //Step 3:
  //Create the tasks to be performed so that each tasks adds the value computed by 
  //task i - 1 in the last step to each element of subarray i and
  //add each task to the thread pool.
  for (uint64_t k = 0; k < numThreads; k++) {
    pool->enqueue(func2, k);
  }
  //wait until the number of tasks to be completed becomes 0
  pool->waitForZeroTasks();

  printf("nums after 3 \n");
  for (int x : nums)
    std::cout << x << " ";
  printf("\n");

  TIMERSTOP(threaded)

  delete pool;

  return GETTIME(threaded);
}

/*
 * get
 * Returns a reference to the private vector.
 */
std::vector<int> & ThreadedScan::get()
{
  return nums;
}

