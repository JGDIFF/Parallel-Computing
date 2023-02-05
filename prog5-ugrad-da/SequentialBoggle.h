#ifndef SEQUENTIALBOGGLE_H
#define SEQUENTIALBOGGLE_H

#include <cstdint>
#include <string>
#include "Boggle.h"
/*
 *  0  1  2  3
 *  4  5  6  7
 *  8  9 10 11
 * 12 13 14 15
 *
 */
class SequentialBoggle : public Boggle
{
  private:
    std::string bestWord;
    void traverse(int boardIdx, uint16_t visited, std::string word);
    void updateBestWord(std::string newWord);
  public:
    SequentialBoggle(BoggleBoard * board);
    float playGame();
    std::string getBestWord();
}; 
#endif
