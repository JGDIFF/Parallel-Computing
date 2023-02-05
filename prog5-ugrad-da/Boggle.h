#ifndef BOGGLE_H
#define BOGGLE_H
#include <cstdint>
#include <string>
#include <vector>
#include "Dict.h"
#include "BoggleBoard.h"
/*
 *  0  1  2  3
 *  4  5  6  7
 *  8  9 10 11
 * 12 13 14 15
 *
 */
class Boggle
{
  protected:
    std::vector<std::string> sols;
    uint16_t nextMoves[16] = {
      //to letter 0, we can add the letter at 1, 4, or 5
      1<<1 | 1<<4 | 1<<5,  /* 0 */
      1<<0 | 1<<2 | 1<<4 | 1<<5 | 1<<6, /* 1 */
      1<<1 | 1<<3 | 1<<5 | 1<<6 | 1<<7, /* 2 */
      1<<2 | 1<<6 | 1<<7, /* 3 */
      1<<0 | 1<<1 | 1<<5 | 1<<8 | 1<<9, /* 4 */
      1<<0 | 1<<1 | 1<<2 | 1<<4 | 1<<6 | 1<<8 | 1<<9 | 1<<10, /* 5 */
      1<<1 | 1<<2 | 1<<3 | 1<<5 | 1<<7 | 1<<9 | 1<<10 | 1<<11, /* 6 */
      1<<2 | 1<<3 | 1<<6 | 1<<10 | 1<<11, /* 7 */
      1<<4 | 1<<5 | 1<<9 | 1<<12 | 1<<13, /* 8 */
      1<<4 | 1<<5 | 1<<6 | 1<<8 | 1<<10 | 1<<12 | 1<<13 | 1<<14, /* 9 */
      1<<5 | 1<<6 | 1<<7 | 1<<9 | 1<<11 | 1<<13 | 1<<14 | 1<<15, /* 10 */
      1<<6 | 1<<7 | 1<<10 | 1<<14 | 1<<15, /* 11 */
      1<<8 | 1<<9 | 1<<13, /* 12 */
      1<<8 | 1<<9 | 1<<10 | 1<<12 | 1<<14, /* 13 */
      1<<9 | 1<<10 | 1<<11 | 1<<13 | 1<<15, /* 14 */
      1<<10 | 1<<11 | 1<<14, /* 15 */
    };
    Dict * dict;
    BoggleBoard * board;
  public:
    Boggle(BoggleBoard *);
    void printSolutions();
    bool equal(Boggle &);
    //virtual functions need to be implemented in the derived classes
    virtual float playGame() = 0;
    virtual std::string getBestWord() = 0;
}; 
#endif
