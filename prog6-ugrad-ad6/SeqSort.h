#ifndef SEQSORT_H
#define SEQSORT_H
#include "Sorts.h"
class SeqSort : public Sorts
{
  private:
  public:
    SeqSort(uint64_t size, int32_t * data);
    double sort();
};
#endif
