#ifndef BOGGLEBOARD_H
#define BOGGLEBOARD_H

class BoggleBoard
{
  private:
    float frequencies[26] = 
    {
      8.4966,  // a
      2.0720,  // b
      4.5388,  // c
      3.3844,  // d     
      11.1607, // e
      1.8121,  // f
      2.4705,  // g
      3.0034,  // h
      7.5448,  // i
      0.1965,  // j
      1.1016,  // k
      5.4893,  // l
      3.0129,  // m
      6.6544,  // n
      7.1635,  // o
      3.1671,  // p
      0.1962,  // q
      7.5809,  // r
      5.7351,  // s
      6.9509,  // t
      3.6308,  // u
      1.0074,  // v
      1.2899,  // w
      0.2902,  // x
      1.7779,  // y
      0.2722   // z
    };
    char board[16];
  public:
    BoggleBoard();
    char getLetter(int);
};
#endif

