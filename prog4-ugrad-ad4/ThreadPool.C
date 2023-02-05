#include "ThreadPool.h"

/*
 * ThreadPool constructor
 * Creates capacity threads that "forever" execute removing
 * a task from the task queue and executing it.
 */
ThreadPool::ThreadPool(uint64_t capacity_) :
  stopPool(false),     // pool is running
  activeThreads(0),    // no work to be done
  capacity(capacity_)  // number of threads
{ 
  numTasks = 0;

  // this function is executed by the threads
  auto waitLoop = [this] ( ) -> void 
  {
    // loop forever
    while (true) 
    {
      // this is a placeholder task
      std::function<void(void)> task;
      { 
        // lock this section for waiting
        std::unique_lock<std::mutex> uniqueLock(mutex);
        auto predicate = [this] ( ) -> bool 
        {
          return  (stopPool) || !(tasks.empty());
        };

        //wait to be waken up on when there is a task
        //to complete or the thread pool has been shut down
        taskCV.wait(uniqueLock, predicate);

        if (stopPool && tasks.empty()) return;
        task = std::move(tasks.front());
        tasks.pop();
        beforeTaskHook();
      } // here we release the lock

      // execute the task in parallel
      task();

      { // adjust the number of active threads and tasks to complete
        std::lock_guard<std::mutex> lockGuard(mutex);
        afterTaskHook();
      } // here we release the lock
    }
  };

  // initially spawn capacity many threads to execute waitLoop
  for (uint64_t id = 0; id < capacity; id++) threads.emplace_back(waitLoop);
}

/*
 * beforeTaskHook
 * Increment number of active threads
 */
void ThreadPool::beforeTaskHook()
{ 
  activeThreads++;
}

/*
 * afterTaskHook
 * Decrement number of active threads.
 * Decrement number of tasks to be completed.
 * Signal a thread that is waiting for the number of tasks
 * to be completed is 0 using the zeroTasksCV condition
 * variable.
 */
void ThreadPool::afterTaskHook()
{ 
  activeThreads--;

  /* WORK TO DO HERE */
  //Decrease the number of tasks completed (numTasks) and signal the
  //thread waiting for the number of tasks to become 0 if numTasks is 0.
  //Use the zeroTasksCV condition variable in ThreadPool.h. 
  //Note that mutex is used in the code that calls this method to prevent 
  //multiple threads from executing this code simultaneously.  This is 
  //needed because numTasks is shared data. Thus, you don't need to use 
  //mutex here.  
  //FYI: The numTask variable is incremented in the enqueue method. 
  //You don't need to add that code.
  numTasks--; 
  if (numTasks == 0) 
  {
		zeroTasksCV.notify_one();
  }
  //zeroTasksCV.notify_one();
}

/*
 * waitForZeroTask
 * Sleep until the number of tasks to be completed becomes
 * zero.
 */
void ThreadPool::waitForZeroTasks()
{
  /* WORK TO DO HERE */
  //The thread that calls this function will wait until
  //the number of tasks to complete becomes 0 (numTasks)
  //Since numTasks is shared data, you'll need a lock
  //using the mutex data member in ThreadPool.h to access it 
  //mutually exclusively. You'll need to use zeroTasksCV 
  //condition variable in ThreadPool.h to cause the thread
  //to wait.
  //bool wait;
  auto predicate = [this] ( ) -> bool {
	  return ((stopPool) || !(tasks.empty()));
  };  
  std::unique_lock<std::mutex> 
      unique_lock(mutex);
  zeroTasksCV.wait(unique_lock, predicate);
}

/*
 * ThreadPool destructor
 * Sets stopPool to true and wakes up all threads so they
 * will see stopPool is set to true and terminate.
 * Calls join on each thread to wait for each to terminate.
 * The destructor is automatically called when the ThreadPool
 * is deleted. 
 */
ThreadPool::~ThreadPool() 
{
  { 
    // acquire a scoped lock
    std::lock_guard<std::mutex> lockGuard(mutex);
    stopPool = true;
  } // here we release the lock

  // signal all threads
  taskCV.notify_all();

  // finally join all threads
  for (auto & thread : threads) thread.join();
}

