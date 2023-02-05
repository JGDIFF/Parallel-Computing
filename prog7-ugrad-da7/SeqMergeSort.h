#ifndef SEQMERGESORT_H
#define SEQMERGESORT_H
#include "Sorts.h"
class SeqMergeSort : public Sorts
{
  private:
    void mergeSort(int32_t sIdx, int32_t eIdx, int32_t * data, int32_t * tmp);
    void merge(int32_t sIdx, int32_t mid, int32_t eIdx, int32_t * data, int32_t * tmp);
  public:
    SeqMergeSort(uint64_t size, int32_t * data);
    double sort();
};
#endif
