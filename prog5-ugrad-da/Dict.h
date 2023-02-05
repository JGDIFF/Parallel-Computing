#ifndef DICT_H
#define DICT_H

#include <unordered_set>
#include <string>
class Dict
{
  private:
    std::unordered_set<std::string> * dictionary;
    Dict();
    static Dict * instance;
  public:
    static Dict * getInstance();
    bool isWord(std::string str);
};
#endif
