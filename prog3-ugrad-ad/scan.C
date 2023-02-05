#include <iostream>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include "SequentialScan.h"
#include "ThreadedScan.h"

//mininum array size is (1 << 4) == 2^4 == 16
#define MINSZ 4
//maximum array size is (1 << 33) == 2^33 == 8589934592
#define MAXSZ 33

static void parseArgs(int, char **, long int &, long int &);
static void usage();
static void init(std::vector<int> &, long int);

/*
 * scan -s <n> -t <m>
 * where 1 << <n> is the size of the array to scan
 * and <m> is the number of threads to use for the threaded version.
 */
int main(int argc, char * argv[])
{
  long int numThreads = 0, arraySize = 0;
  std::vector<int> initArray;

  //parse the command line arguments and get the number of threads
  //and the array size
  parseArgs(argc, argv, numThreads, arraySize);

  //initialize the vector to be used for each scan
  init(initArray, arraySize);

  //perform the sequential scan
  SequentialScan ss(initArray);
  printf("Performing a sequential scan of %ld integers.\n",
         arraySize);
  float ssTime = ss.performScan();

  //perform the threaded scan
  ThreadedScan ts(std::move(initArray), numThreads);
  printf("Performing a threaded scan with %ld threads.\n", numThreads);
  float tsTime = ts.performScan();

  //make sure the two vectors match
  if (!ss.compare(ts.get()))
  {
    printf("Scans match.\n");
    printf("Sequential scan time: %1.6f\n", ssTime);
    printf("Threaded scan time: %1.6f\n", tsTime);
    printf("Speedup: %.6f\n", ssTime/tsTime);
  }
}

/*
 * init
 * Initializes a vector so that it contains arraySize
 * ints between the values of -4 and 4.
 */
void init(std::vector<int> & array, long int arraySize)
{
  for (long int i = 0; i < arraySize; i++)
  {
     int num = random() % 5;
     array.push_back((random() % 2) ? num * -1: num);
  }
}

/*
 * parseArgs
 * Takes as input the command line arguments, parses them,
 * and sets numThreads and arraySize
 * Inputs: 
 * argc is count of command line arguments
 * argv[1] ... argv[argc - 1] are actual command line arguments
 * Returns:
 * arraySize is set 1 << numeric value following -s
 * numThreads is set to numeric value following -t
 */
void parseArgs(int argc, char * argv[], 
               long int & numThreads, long int & arraySize)
{
  int opt;
  while((opt = getopt(argc, argv, "s:t:h")) != -1)  
  {  
    switch(opt)  
    {  
      case 't': 
        numThreads = atoi(optarg);
        break;
      case 's':
        arraySize = (long)1 << (long)atoi(optarg);  //2^s
        break;
      default:
        usage();
        break;
    }
  }
  //number of threads must be greater than 1 and less than the
  //number of threads supported by the computer 
  if ((numThreads <= 1) || (numThreads > sysconf(_SC_NPROCESSORS_ONLN)))
  {
    printf("Bad number of threads.\n");
    usage(); 
  }
  //array size must be at least 2^MINSZ and not greater than 2^MAXSZ
  //Thus, the valid inputs are between MINSZ and MAXSZ inclusive
  if (arraySize < ((long)1 << MINSZ) || arraySize > ((long)1 << MAXSZ))
  {
    printf("Bad array size.\n");
    usage(); 
  }

  //size of array must be greater than number of threads
  if (arraySize < numThreads)
  {
    printf("Array size cannot be less than number of threads.\n");
    usage();
  }
}

/* 
 * usage
 * Prints usage information and exits.
 */
void usage()
{
  printf("usage: scan -s <n> -t <m>\n\n");
  printf("\tPerforms a prefix scan on a randomly generated array\n");
  printf("\tof ints. Compares the performance of a sequential version\n");
  printf("\tof the scan to a threaded version of the scan.\n\n");
  printf("\t<n>: 1 << <n> (e.g. 2^<n>) is size of array to scan\n");
  printf("\t<n> must be at least %d and not more than %d\n\n",
         MINSZ, MAXSZ);
  printf("\t<m> is the number of threads to use to perform the scan\n");
  printf("\t<m> must be greater than 1 and less than %ld\n",
         sysconf(_SC_NPROCESSORS_ONLN) + 1);
  exit(0);
}


