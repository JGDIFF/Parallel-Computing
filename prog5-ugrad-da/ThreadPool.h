#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <stdio.h>
#include <cstdint>
#include <future>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

class ThreadPool 
{
  private:
    // storage for threads and tasks
    std::vector<std::thread> threads;
    std::queue<std::function<void(void)>> tasks;

    // primitives for signaling
    std::mutex mutex;
    std::condition_variable cv, cv_wait;

    // the state of the thread, pool
    bool stop_pool;
    std::atomic<uint32_t> active_threads;
    std::atomic<uint32_t> num_tasks;
    const uint32_t capacity;

    //template functions must be in the header file
    /*
     * make_task
     * Takes as input a function and it arguments and creates
     * and returns a packaged task that contains it, allowing
     * it to be called later.
     * Input:
     *   func - name of a function
     *   args - zero or more arguments to the function
     * Output:
     *   packaged task encapsulating the function and it arguments
     */
    template <typename Func, typename ... Args, 
              typename Rtrn=typename std::result_of<Func(Args...)>::type>
    auto make_task(Func && func, Args && ...args) -> 
                   std::packaged_task<Rtrn(void)> 
    {
      auto aux = std::bind(std::forward<Func>(func), 
                           std::forward<Args>(args)...);
      return std::packaged_task<Rtrn(void)>(aux);
    }

    void before_task_hook();
    void after_task_hook();

  public:
    ThreadPool(uint64_t capacity_, uint64_t num_tasks_);
    ~ThreadPool();
    //void no_more_tasks();
    void wait_and_stop();

    //template functions must be in the header file
    /*
     * enqueue
     * Takes as input a function and its arguments, calls make_task
     * to encapsulate it in a packaged_task, adds it to the task queue,
     * and returns the future object associated with the packaged_task.
     * notify_one is called to wake up one thread to handle the task.
     * Input:
     *   func - name of a function
     *   args - zero or more arguments to the function
     * Output:
     *   future object associated the packaged task that is created
     */
    template <typename Func, typename ... Args, typename Pair=Func(Args...),
              typename Rtrn=typename std::result_of<Pair>::type>
    auto enqueue(Func && func, Args && ... args) -> std::future<Rtrn> 
    {
      auto task = make_task(func, args...);
      auto future = task.get_future();
      auto task_ptr = std::make_shared<decltype(task)> (std::move(task));

      {   // lock the scope
        std::lock_guard<std::mutex> lock_guard(mutex); 
        // you cannot reuse pool after being stopped    
        if (stop_pool) 
          throw std::runtime_error("enqueue on stopped ThreadPool");
        auto payload = [task_ptr] ( ) -> void 
        {
          task_ptr->operator()();
        };

        // append the task to the queue
        tasks.emplace(payload);
      }

      // tell one thread to wake-up
      cv.notify_one();
      return future;
    }

    //template functions must be in the header file
    /*
     * spawn
     * Takes as input a function and its arguments. If there are idle threads
     * then the enqueue function is called to add it to the task queue.
     * If there are no idle threads then the function is simply called. Thus,
     * in that case, the function will be executed by the calling thread.
     * Input:
     *   func - name of a function
     *   args - zero or more arguments to the function
     */
    template <typename Func, typename ... Args>
    void spawn(Func && func, Args && ... args) 
    {
      num_tasks--;
      // enqueue if idling threads
      if (active_threads < capacity)
        enqueue(func, args...);
      // else process sequential
      else
        func(args...);
    }
};

#endif
