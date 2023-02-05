
class SequentialScan
{
  private:
    std::vector<int> nums;
  public:
    SequentialScan(std::vector<int> nums);
    float performScan();
    int compare(std::vector<int> & inputNums);
};   
