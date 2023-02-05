#ifndef SEQQUICKSORT_H
#define SEQQUICKSORT_H
#include "Sorts.h"
class SeqQuickSort : public Sorts
{
  private:
    void quickSort(int32_t sIdx, int32_t eIdx, int32_t * data);
    int partition(int32_t sIdx, int32_t eIdx, int32_t * data);
  public:
    SeqQuickSort(uint64_t size, int32_t * data);
    double sort();
};
#endif
