#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "BoggleBoard.h"

/*
 * BoggleBoard 
 * Creates the 4 by 4 boggle board by generating
 * a letter for each square.  In order to generate better playing
 * boards, the probability of a letter being chosen is the frequency
 * of the letter's appearance in the English language.
 */
BoggleBoard::BoggleBoard()
{
  float cumulativeFrequencies[26];
  //Use the frequency of each letter to build the cumulative frequencies 
  cumulativeFrequencies[0] = 0;
  for (int j = 1; j < 26; j++)
  {
    cumulativeFrequencies[j] = cumulativeFrequencies[j - 1] + frequencies[j - 1];
  } 

  //Pause a second between building boggle boards to increase likelihood
  //of getting a different board from last time
  sleep(1); 

  //seed the random number generator
  srand (time(NULL));
  for (int i = 0; i < 16; i++)
  {
    //get a float between 0 and 100 up to six digits of significance
    float randomFrequency = random() % 100000000;
    randomFrequency = randomFrequency / 1000000;
    float set = false;
    for (int j = 0; j < 25; j++)
    {
      if (randomFrequency >= cumulativeFrequencies[j] &&
          randomFrequency < cumulativeFrequencies[j + 1]) 
      {
        board[i] = j + 'a'; 
        set = true;
        break;
      }
    }
    //if randomFrequency is greater than last frequency, the letter
    //is a 'z'
    if (!set) board[i] = 'z';
  }
}

/*
 * getLetter
 * Return the ith letter of the boggle board
 * Input:
 *   i between 0 and 15 is an index into the boggle board
 * Output:
 *   character in boggle board
 */
char BoggleBoard::getLetter(int i)
{
  if (i >= 0 && i <= 15)
    return board[i];
  else
    return 'a';   //just in case index is invalid
}

