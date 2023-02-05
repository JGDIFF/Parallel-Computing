#include <iostream>
#include <chrono>
#include "SequentialBoggle.h"
#include "helpers.h"

/*
 * SequentialBoggle
 * Initialize the SequentialBoggle game.
 * Input:
 *   Pointer to a BoggleBoard object
 */
SequentialBoggle::SequentialBoggle(BoggleBoard * board):Boggle(board) 
{
  this->bestWord = "";
}

/* 
 * playGame
 * Solves the boggle game sequentially.  Each found word
 * is added to the solution vector.
 * Output:
 *    Time it takes to solve the game
 */
float SequentialBoggle::playGame()
{
  //start timing the solver (see helpers.h)
  TIMERSTART(sequential)

  //call the traverse function on each square in the board
  for (int i = 0; i < 16; i++)  
  {
    uint16_t visited = 1 << i;
    std::string word(1, this->board->getLetter(i));
    traverse(i, visited, word);
  }
  //stop timing the solver
  TIMERSTOP(sequential)

  //return the difference between the starting time
  //and the ending timing
  return GETTIME(sequential);
}

/*
 * updateBestWord
 * This updates the bestWord using the newWord parameter.
 * If the newWord is longer than the current bestWord
 * (and the length is <= 8) then bestWord is set to newWord.
 * If newWord and bestWord have the same length then bestWord
 * is set to newWord if newWord appears alphabetically
 * before bestWord.
 * Input:
 *   string to consider for the bestWord
 * Examples:
 *   if bestWord is "of" and newWord is "are", then bestWord is
 *   set to "are" because it is longer.
 *   if bestWord is "of" and newWord is "tennessee", then bestWord is
 *   left as "of" because "tennessee" is longer than 8 characters.
 *   if bestWord is "our" and newWord is "are", then bestWord is
 *   set to "are" because "are" appears in the dictionary before
 *   "our".
 */
void SequentialBoggle::updateBestWord(std::string newWord)
{
  bool update = false;
  //make sure newWord isn't longer than 8 characters
  if (newWord.length() <= 8)
  {
    //if newWord is longer then change bestWord
    if (newWord.length() > bestWord.length())
      update = true;
    //if same length then change bestWord if newWord is
    //alphabetically less than current bestWord
    else if (newWord.length() == bestWord.length() && newWord < bestWord)
      update = true;
  } 
  //update 
  if (update) bestWord = newWord;
}

/*
 * traverse
 * Recursive function that solves the game of boggle.
 * Inputs:
 *  boardIdx - index into the board for the letter just added to a word
 *  visited - indicates the characters in the board that have already been visited
 *  word - word to be added to
 */
void SequentialBoggle::traverse(int boardIdx, uint16_t visited, std::string word)
{
  //get a mask that indicates what board positions can be visited
  //from the current position
  //See Boggle.h
  uint16_t legalMoves = nextMoves[boardIdx];
  
  //consider each of the 16 squares on the board
  for (int j = 0; j < 16; j++)
  {
    //to consider moving from letter boardIdx to letter j, build
    //a mask by shifting 1 j bits: 00010...0
    uint16_t bmask = 1 << j;
    //build a new word using letter j but only if the move
    //is legal and j hasn't already been visited
    if (((legalMoves & bmask) != 0) && ((visited & bmask) == 0))
    {
      //build a new string
      std::string newWord = word + this->board->getLetter(j);
      //if the length is >= 3 and it is in the dictionary
      //then add it to the solution
      if (newWord.length() >= 3 && this->dict->isWord(newWord))
      {
         sols.push_back(newWord);  //add to solution vector
         updateBestWord(newWord);  //check if update to bestWord needed
      }
      //mark board position j as visited
      visited = visited | (1 << j); 
      //call traverse to build further from here
      traverse(j, visited, newWord);
    }
  }
}

/*
 * getBestWord
 * Returns the best word found by solving boggle
 */
std::string SequentialBoggle::getBestWord()
{
  return bestWord;
}  

