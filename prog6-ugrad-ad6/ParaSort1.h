#ifndef PARASORT1_H
#define PARASORT1_H
#include "Sorts.h"
class ParaSort1 : public Sorts
{
  private:
    int32_t threadCt;
  public:
    ParaSort1(uint64_t size, int32_t * data, int32_t threadCt);
    double sort();
};
#endif
