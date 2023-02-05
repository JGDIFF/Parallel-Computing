#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <cstdint>
#include <future>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

class ThreadPool 
{
  private:
    // storage for threads and tasks
    std::vector<std::thread> threads;
    std::queue<std::function<void(void)>> tasks;

    // primitives for signaling
    std::mutex mutex;   //to prevent simultaneous access to shared data
    std::condition_variable taskCV;      //to wake up a thread waiting for a task
    std::condition_variable zeroTasksCV; //to wake up the main thread when all tasks are done

    // the state of the thread, pool
    bool stopPool;
    uint32_t activeThreads;
    const uint32_t capacity;
    uint32_t numTasks;

    /* makeTask
     * Takes as input a function and it argument. Creates a packaged_task
     * that is a container for the function and arguments and returns
     * it.
     * makeTask needs to be in the header file instead of the source file
     * since it is a template.
     */
    template <
        typename     Func,
        typename ... Args,
        typename Rtrn=typename std::result_of<Func(Args...)>::type>
    auto makeTask(Func &&    func, Args && ...args) -> 
                  std::packaged_task<Rtrn(void)>
    {
      //Create a wrapper for the function and its arguments
      auto aux = std::bind(std::forward<Func>(func),
                           std::forward<Args>(args)...);
      //Package the wrapper to get a future
      return std::packaged_task<Rtrn(void)>(aux);
    }

    void beforeTaskHook();   
    void afterTaskHook(); 

  public:
    ThreadPool(uint64_t capacity_);
    ~ThreadPool();
    void waitForZeroTasks();

    /*
     * enqueue
     * Takes as input a function and its arguments and calls the makeTask function
     * to get a packaged_task containing the function and arguments. That is then
     * wrapped in a void function, which is inserts into the tasks queue.
     * enqueue needs to be in the header file instead of the source file
     * since it is a template.
     */
    template <
        typename     Func,
        typename ... Args,
        typename Pair=Func(Args...),
        typename Rtrn=typename std::result_of<Pair>::type>
    auto enqueue(Func && func, Args && ... args) -> std::future<Rtrn>
    {
      // create the task, get the future and wrap task in a shared pointer
      auto task = makeTask(func, args...);
      auto future = task.get_future();
      auto taskPtr = std::make_shared<decltype(task)> (std::move(task));

      {   
        // lock the scope since shared data is being access
        std::lock_guard<std::mutex> lockGuard(mutex);

        // you cannot reuse pool after being stopped
        if (stopPool) throw std::runtime_error("enqueue on stopped ThreadPool");

        // wrap the task in a generic void function
        auto payload = [taskPtr] ( ) -> void 
        {
           // use pointer to call wrapped function 
           taskPtr->operator()();
        };

        // append the task to the queue
        numTasks++;
        tasks.emplace(payload);
      }

      // tell one thread to wake-up to complete the task
      taskCV.notify_one();

      return future;
    }
};

#endif
