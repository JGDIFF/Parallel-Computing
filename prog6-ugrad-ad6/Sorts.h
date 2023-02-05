#ifndef SORTS_H
#define SORTS_H
#include <cstdint>
#include <string>
class Sorts
{
  protected:
    int32_t * data;
    uint64_t size;
    std::string description;
  public:
    Sorts(uint64_t size, int32_t * input);
    bool match(Sorts * sptr);
    bool increasing();
    std::string getDescription();
    virtual double sort() = 0;
    virtual ~Sorts();
};
#endif
