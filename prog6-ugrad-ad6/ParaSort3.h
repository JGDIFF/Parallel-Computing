#ifndef PARASORT3_H
#define PARASORT3_H
#include "Sorts.h"
class ParaSort3 : public Sorts
{
  private:
    int32_t threadCt;
  public:
    ParaSort3(uint64_t size, int32_t * data, int32_t threadCt);
    double sort();
};
#endif
