#include <fstream>
#include <iostream>
#include <stdlib.h>
#include "Dict.h"

//Dict is a singleton 
//Here is the pointer to the single object
Dict * Dict::instance = NULL;

/*
 * Dict constructor
 * This private constructor initializes an unordered_set to
 * the words in the file /usr/share/dict/words
 */ 
Dict::Dict()
{
  //open the file
  std::string wordFilePath("/usr/share/dict/words");
  std::ifstream wordFile(wordFilePath);
  std::string word;
  if (!wordFile.is_open())
  {
     std::cout << "Open of " << wordFilePath << " failed.\n";
     exit(1);
  }
  //create an unordered_set
  dictionary = new std::unordered_set<std::string>();

  //read the file and insert each word in the unordered set
  while (getline(wordFile,word))
  {
    dictionary->insert(word);
  }
  //close file
  wordFile.close();
}

/*
 * isWord
 * Takes as input a string and checks to see if it is a word in the
 * dictionary.
 * Input:
 *   string
 * Output:
 *   true if the string is in the dictionary
 *   false otherwise
 */
bool Dict::isWord(std::string str)
{
  //returns an iterator to the string if found
  if (dictionary->find(str) == dictionary->end())
    return false;
  else
    return true;
}

/*
 * getInstance
 * Returns a pointer to the single instance of the Dict class.
 * If the instance doesn't exist, this method calls the Dict
 * constructor.
 * Returns:
 *   pointer to the single Dict instance
 */
Dict * Dict::getInstance()
{
  if (Dict::instance == NULL)
     Dict::instance = new Dict();
  return Dict::instance;
}

