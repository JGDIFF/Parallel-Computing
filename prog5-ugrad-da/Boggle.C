#include <iostream>
#include <string>
#include <getopt.h>
#include <unistd.h>
#include "SequentialBoggle.h"
#include "ThreadedBoggle.h"

#define MAXGAMES 20
//functions only for use in this file
static void parseArgs(int argc, char * argv[], int & threadPoolSize,
               int & gameCount);
static void usage();

int main(int argc, char ** argv)
{
  double stimeTotal = 0;     //total sequential boggle times
  double ttimeTotal = 0;     //total threaded boggle times
  float stime;               //individual sequential boggle time
  float ttime;               //individual threaded boggle time
  int threadPoolSize = 0;    //size of thread pool used by threaded version
  int gameCount = 0;         //number of boggle boards to solve

  //parse command line arguments
  parseArgs(argc, argv, threadPoolSize, gameCount);

  for (int i = 0; i < gameCount; i++)
  {
    //create the board to use for this iteration
    BoggleBoard * board = new BoggleBoard();
    SequentialBoggle sequential(board);
    stime = sequential.playGame();
    stimeTotal += stime;  //add time to total
    //If you decide to look at the solution, be forewarned that some
    //of the words in the Linux dictionary that is used for verifying
    //words are suspect.
    //sequential.printSolutions();

    ThreadedBoggle threaded(board, threadPoolSize);
    ttime = threaded.playGame();
    ttimeTotal += ttime;  //add time to total
    //threaded.printSolutions();

    //check to see of the sequential version solution is the
    //same as the threaded version solution
    if (!threaded.equal(sequential))
    {
      printf("Threaded Solution does not match Sequential Solution\n");
      return 0;
    }
    else
    {
      printf("\nSolutions match.\n");
      printf("Sequential best word: %s\n", sequential.getBestWord().c_str());
      printf("Threaded best word: %s\n", threaded.getBestWord().c_str());
      printf("Sequential boggle time: %1.6f\n", stime);
      printf("Threaded boggle time: %1.6f\n", ttime);
    }
    delete board;
  }
  printf("\nAverage speedup (Sequential/Threaded): %.3f\n", stimeTotal/ttimeTotal);
}

/*
 * parseArgs
 * Takes as input the command line arguments, parses them,
 * and sets threadPoolSize and gameCount
 * Inputs: 
 * argc is count of command line arguments
 * argv[1] ... argv[argc - 1] are actual command line arguments
 * Returns:
 * threadPoolSize - size of the thread pool for the threaded version
 * gameCount - number of boggle games to perform
 */
void parseArgs(int argc, char * argv[], int & threadPoolSize,
               int & gameCount)
{
  if (argc != 5) usage();
  int opt;
  while((opt = getopt(argc, argv, "p:g:h")) != -1)
  {
    switch(opt)
    {
      case 'p':
        threadPoolSize = atoi(optarg);
        break;
      case 'g':
        gameCount = atoi(optarg);
        break;
      default:
        usage();
    }
  }
  //number of threads must be greater than 1 and less than the
  //number of threads supported by the computer 
  if ((threadPoolSize <= 1) || (threadPoolSize > sysconf(_SC_NPROCESSORS_ONLN)))
  {
    printf("Bad number of threads.\n");
    usage();
  }
  if (gameCount < 1 || gameCount > MAXGAMES)
  {
    printf("Bad number of games.\n");
    usage();
  }
}

/*
 * usage
 * Prints usage information and exits.
 */
void usage()
{
  printf("usage: boggle -p <n> -g <m>\n\n");
  printf("\tRandomly generates a boggle board and determines the\n");
  printf("\tsolution. Compares the performance of a sequential version\n");
  printf("\tof the boggle solver to a threaded version of the scan.\n\n");
  printf("\t<n> is the number of threads to create\n");
  printf("\t<n> must be greater than 1 and less than %ld\n\n",
         sysconf(_SC_NPROCESSORS_ONLN) + 1);
  printf("\t<m> is the number of runs of boggle to perform. It\n");
  printf("\tmust be greater than one and less than %d\n\n", MAXGAMES + 1);
  exit(0);

}
