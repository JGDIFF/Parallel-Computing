#ifndef PARASORT2_H
#define PARASORT2_H
#include "Sorts.h"
class ParaSort2 : public Sorts
{
  private:
    int32_t threadCt;
  public:
    ParaSort2(uint64_t size, int32_t * data, int32_t threadCt);
    double sort();
};
#endif
