#ifndef PARAMERGESORT_H
#define PARAMERGESORT_H
#include "Sorts.h"
class ParaMergeSort : public Sorts
{
  private:
    int32_t threadCt;
    void mergeSort(int32_t sIdx, int32_t eIdx, int32_t * data, int32_t * tmp);
    void merge(int32_t sIdx, int32_t mid, int32_t eIdx, int32_t * data, int32_t * tmp);
  public:
    ParaMergeSort(uint64_t size, int32_t * data, int32_t threadCt);
    double sort();
};
#endif

