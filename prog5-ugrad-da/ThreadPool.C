#include "ThreadPool.h"

/*
 * before_task_hook
 * This function is called by a thread after a task is removed
 * from the task queue and before that task is executed.
 */
void ThreadPool::before_task_hook() 
{
  //increase the number of active threads
  active_threads++;
}

/*
 * after_task_hook
 * This function is called by a thread after a task is completed.
 * If there are no more tasks to execute, the function sets
 * stop_pool to true and signals the thread blocked on cv_wait
 * condition variable in the wait_and_stop function.
 */
void ThreadPool::after_task_hook() 
{
  //reduce active threads
  active_threads--;
  if (num_tasks == 0 && active_threads == 0 && tasks.empty()) 
  {
    stop_pool = true;
    //signal main thread executing wait_and_stop function
    cv_wait.notify_one();
  }
}

/*
 * ThreadPool constructor
 * Initializes the thread pool object and creates capacity threads.
 * The threads execute the wait_loop lambda expression embedded in
 * this constructor.
 * Input:
 *   capacity - number of threads to create
 */
ThreadPool::ThreadPool(uint64_t capacity_, uint64_t num_tasks_) :
  stop_pool(false),     // pool is running
  active_threads(0),    // no work to be done
  num_tasks(num_tasks_), // number of tasks for the pool to handle
  capacity(capacity_)  // remember size
{        
  // this function is executed by the threads
  auto wait_loop = [this] ( ) -> void 
  {

   // wait forever
    while (true) 
    {
      // this is a placeholder task
      std::function<void(void)> task;
      { // lock this section for waiting
        std::unique_lock<std::mutex> unique_lock(mutex);
        auto predicate = [this] ( ) -> bool 
        {
          return (stop_pool) || !(tasks.empty());
        };

        //wait until there is a task to perform or
        //stop_pool is true
        cv.wait(unique_lock, predicate);
        if (stop_pool && tasks.empty()) return;

        task = std::move(tasks.front());
        tasks.pop();                    
        before_task_hook();   //before calling task
      } // here we release the lock

      // execute the task in parallel
      task();

      {   // adjust the thread counter
        std::lock_guard<std::mutex> lock_guard(mutex);
        after_task_hook();  //after calling task
      } // here we release the lock
    }
  };

  // initially spawn capacity many threads
  for (uint64_t id = 0; id < capacity; id++) threads.emplace_back(wait_loop);
}

/*
 * ThreadPool destructor
 * Sets stop_pool to true and wakes up all threads blocked on cv
 * so that they will terminate. Finally, calls join to wait until
 * each thread terminates.
 */
ThreadPool::~ThreadPool() 
{
  { // acquire a scoped lock
    std::lock_guard<std::mutex> lock_guard(mutex);
    stop_pool = true;
  } // here we release the lock

  // signal all threads
  cv.notify_all();

  // finally join all threads
  for (auto& thread : threads) thread.join();
}

/*
 * wait_and_stop
 * Called by the main thread so that it waits until all tasks
 * it added to the thread pool have been completed.
 */
void ThreadPool::wait_and_stop() 
{
  // wait for pool being set to stop
  std::unique_lock<std::mutex> unique_lock(mutex);
  auto predicate = [&] () -> bool 
  {
    return stop_pool;
  };
  //wait for signal from after_task_hook        
  cv_wait.wait(unique_lock, predicate);
}

