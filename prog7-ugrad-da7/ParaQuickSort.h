#ifndef PARAQUICKSORT_H
#define PARAQUICKSORT_H
#include "Sorts.h"
class ParaQuickSort : public Sorts
{
  private:
    int32_t threadCt;
    int partition(int32_t sIdx, int32_t eIdx, int32_t * data);
    void quickSort(int32_t sIdx, int32_t eIdx, int32_t * data);
  public:
    ParaQuickSort(uint64_t size, int32_t * data, int32_t threadCt);
    double sort();
};
#endif
