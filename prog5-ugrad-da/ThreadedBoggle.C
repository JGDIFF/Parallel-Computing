#include <iostream>
#include "ThreadedBoggle.h"
#include "helpers.h"

/* Your work is in this file and ThreadedBoggle.h. */
/* You'll need more data members and possibly more methods. */

/*
 * ThreadedBoggle
 * Initialize the ThreadedBoggle game.
 * Input:
 *   board - pointer to the BoggleBoard object containing the 4 by 4 board
 *   threadPoolSize - number of threads in the ThreadPool
 */
ThreadedBoggle::ThreadedBoggle(BoggleBoard * board, int threadPoolSize):Boggle(board)
{
  //Initialize the thread pool and the bestWord. A pointer
  //to a ThreadPool object is declared in
  //ThreadedBoggle.h. You need to add a data member that is an atomic
  //to ThreadedBoggle.h. It needs to store 8 characters
  //for the best word. Remember you can use an atomic to encapsulate 
  //a user defined data type.  A union is helpful here so you can access
  //it as either a uint64_t or a char array.
  this->TP = new ThreadPool(threadPoolSize, 16);
  utype initWord;
  initWord.num = 0;
  /*for (int i = 0; i < 8; i++)
  {
    initWord.bestW[i] = 0;
  }*/
  this->word.store(initWord);
  //You can initialize an atomic with a store.  Google it.
  //
  //The ThreadPool constructor needs to be passed the number of
  //threads in the pool (threadPoolSize) and the number of
  //tasks that are going to created (the number of times spawn
  //will be called -- 16 for this program). 
  //ThreadPool(threadPoolSize, 16);
	
  
  //Note: this second parameter is needed to prevent
  //a race condition.  There's an error in the book code where
  //a thread might set stop_pool to true when the main thread still
  //has tasks to add.
} 

/*
 * updateSolution
 * Adds a new word to the solution vector.  Only one thread should be
 * able to modified the vector at a time.
 * 
 * Inputs:
 *   newWord - word to add to the solution vector
 */
void ThreadedBoggle::updateSolution(std::string newWord) {
  mutex.lock();
  sols.push_back(newWord);
  mutex.unlock();
}

/*
 * updateBestWord
 * Updates the std::atomic bestWord.
 *
 */
void ThreadedBoggle::updateBestWord(std::string newWord)
{
  //Get the up to 8 characters that make up the bestWord
  //out of the 64 bit atomic variable using a load
  //You'll need to add a declaration of the atomic variable to
  //ThreadedBoggle.h
  //
    //convert the 8 characters into a string (Write a method or
    //lambda expression to do this.)

    //compare the current best word string to the newWord string
    //to see if the best word should be updated to the newWord
    //current word should be updated if:
    //1) it is less than 9 characters in length and longer than 
    //   best word
    //2) it is the same length as best word and alphabetically less
    //   than best word.  For example, "are" < "our"
    //See SequentialBoggle code for assistance.

	/*
	load from the atomic
	do {
	//Build string from Union within atomic
	//if (new word is not better than best word)
		//return;
	//build union from the new word (opposite of when we built the word from the union)
	} while (!bestWord.compare_exchange_weak(bestword Union, newWord Union)
	*/


  utype curBest = word.load();
  utype newBest;
  do {
    std::string bestWord = "";
    bool update = false;
    newBest.num = 0;
    for (int i = 0; i < 8; i++) 
    {
      if (curBest.bestW[i] != '\0' || curBest.bestW[i] != 0) 
      {
        bestWord += curBest.bestW[i];
      }
    }
    if (newWord.length() <= 8)
    {
      if (newWord.length() > bestWord.length()) 
      {
        update = true;   
      }
      else if (newWord.length() == bestWord.length() && newWord < bestWord)
      {
        update = true;
      }
    if (!update)
      return;
    for (uint i = 0; i < newWord.length(); i++)
    {
      newBest.bestW[i] = newWord[i];
    }
    }
  }
    while (!word.compare_exchange_weak(curBest, newBest));

    //if the bestword should be updated with the newWord then
    //build a 64-bit unsigned out of the newWord and try to update
    //it.  Note: multiple threads may be trying to do this at the
    //same time! You need to use a compare and exchange so that 
    //only one will succeed. The others might try again assuming their
    //word is better than the thread that succeeded.
    //See book for how to do this.
}

/* 
 * char to string
 * Converts char array to a string and returns the string
 */
std::string ThreadedBoggle::charToString(char * a) {
  std::string s = a;
  return s;
}

/*
 * playGame
 * Determines the solution to the boggle board.  The threadpool
 * is used to build the solution and spawn is called for each 
 * of the 16 squares in the boggle board.
*/
float ThreadedBoggle::playGame()
{

  //start timing the solver (see helpers.h)

  TIMERSTART(parallel)
  //See code in SequentialBoggle.C
  //Call spawn on each of the 16 squares
  //However the traverse function can not be a member of
  //the ThreadedBoggle class.  It can be a lambda expression
  //or a function that is not part of a class.
  std::function <void(int, uint16_t, std::string)> traverse;
  
  traverse = [&](int boardIdx, uint16_t visited, std::string word) -> void {
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
           updateSolution(newWord);  //add to solution vector
           updateBestWord(newWord);  //check if update to bestWord needed
        }
        //mark board position j as visited
        visited = visited | (1 << j); 
        //call traverse to build further from here
        traverse(j, visited, newWord);
      }
    }
  };

  for (int i = 0; i < 16; i++)  
  {
    uint16_t visited = 1 << i;
    std::string word(1, this->board->getLetter(i));
    TP->spawn(traverse, i, visited, word);
  }

  //Wait until the Thread Pool is empty
  TP->wait_and_stop();

  //stop timing the solver
  TIMERSTOP(parallel)

  //delete the ThreadPool
  delete TP;
  //return the difference between the starting time
  //and the ending timing
  return GETTIME(parallel);
}

/*
 * getBestWord
 * Get the best word out of the 64-bit atomic, build a string
 * out of it, and return it.
 */
std::string ThreadedBoggle::getBestWord()
{
  std::string ret = "";
  utype bestRet = word.load();
  for (int i = 0; i < 8; i++) 
  {
    if (bestRet.bestW[i] == '\0' || bestRet.bestW[i] == '0')
      break;
    ret += bestRet.bestW[i];
  }
  return ret;
}

