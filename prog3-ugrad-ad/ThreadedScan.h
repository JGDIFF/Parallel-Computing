class ThreadedScan
{
  private:
    std::vector<int> nums;
    long int subarraySize;
    long int numThreads;
  public:
    ThreadedScan(std::vector<int> nums, long int numThreads);
    float performScan();
    std::vector<int> & get();
};
     
