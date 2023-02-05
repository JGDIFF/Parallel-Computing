class ThreadedScan
{
  private:
    std::vector<int> nums;
    uint64_t subarraySize;
    uint64_t numThreads;
    uint64_t activeThreads;
    std::condition_variable wait;
    std::mutex mutex;
    ThreadPool * pool;
  public:
    ThreadedScan(std::vector<int>, uint64_t, uint64_t);
    float performScan();
    std::vector<int> & get();
};
     
