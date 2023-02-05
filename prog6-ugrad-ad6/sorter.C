#include <iostream>
#include <unistd.h>
#include <cstdint>
#include <chrono>
#include <time.h>
#include <functional>
#include <omp.h>
#include "SeqSort.h"
#include "ParaSort1.h"
#include "ParaSort2.h"
#include "ParaSort3.h"

/* headers for functions in this file */
static void parseArgs(int32_t argc, char * argv[], uint64_t & size, 
                      int32_t & threadCt, bool & runSeq, bool * runPara);
static void usage();
static int32_t * createSortData(int64_t size);

/* To run code:
 * ./sorter -n <n> -t <t> [-s] [-1] [-2] [-3]
 * size of the array: 1 << <n>; e.g. 2 to the power of <n>
 * number of threads: <t>
 * if -s option is provided, sequential version is executed
 * if -1 option is provided, parallel version 1 is executed
 * if -2 option is provided, parallel version 1 is executed
 * if -3 option is provided, parallel version 1 is executed
 */
int32_t main(int32_t argc, char * argv[])
{
  uint64_t size = 0;                        //amount of data to sort
  int32_t threadCt = 8;                     //default number of threads
  bool runSeq = false;                      //perform sequential sort
  bool runPara[3] = {false, false, false};  //which parallel sort?
  Sorts * paraPtrs[3] = {0, 0, 0};          //pointers to parallel sort objects
  bool runParallel = false;                 //perform any parallel sort?
  int * data;                               //pointer to data to sort

  //create an array of pointers to functions that create parallel sort objects
  auto makeParaSort1 = [&] (){ return new ParaSort1(size, data, threadCt); };
  auto makeParaSort2 = [&] (){ return new ParaSort2(size, data, threadCt); };
  auto makeParaSort3 = [&] (){ return new ParaSort3(size, data, threadCt); };
  std::function<Sorts *()> makeSort[3] = {makeParaSort1, makeParaSort2, makeParaSort3};

  /* parse command line arguments to get array size, thread count */
  /* and which sorts to run */
  parseArgs(argc, argv, size, threadCt, runSeq, runPara);
  for (int32_t i = 0; i < 3; i++) runParallel = runParallel || runPara[i];

  printf("Sorting an array of size %ld.\n", size);
  if (runParallel)
    printf("Parallel versions use %d threads.\n", threadCt);

  /* create data to sort */
  data = createSortData(size);

  SeqSort * seq = NULL;               //pointer to sequential sort object
  double seqTime;                     //amount of time that sequential sort takes
  double guessTime = size/(1 << 14);  //estimated time for sequential sort
  guessTime = guessTime * guessTime * .835;
  if (runSeq)
  {
    /* Run the sequential sort. This is used to check for correctness */
    seq = new SeqSort(size, data);
    if (size > (1 << 15)) 
      printf("Warning sequential sort (-O2 optimizations) will take approximately %.3fs\n", guessTime);
    seqTime = seq->sort(); 
    printf("\n%s\n", seq->getDescription().c_str());
    printf("Time: %2.6f\n", seqTime);
  } else
  {
    seqTime = guessTime;
  }

  for (int32_t i = 0; i < 3; i++)
  {
     if (runPara[i])  //run parallel sort i?
     {
       //create parallel sort object and run sort
       paraPtrs[i] = makeSort[i]();   
       double paraTime = paraPtrs[i]->sort();
       printf("\n%s", paraPtrs[i]->getDescription().c_str());

       //if sequential sort was performed compare the two results
       if (runSeq == true && !paraPtrs[i]->match(seq)) exit(1);

       //if the sequential sort was not performed make sure the result is in increasing order
       if (!runSeq && !paraPtrs[i]->increasing()) exit(1);

       //output information about parallel sort
       printf("Time: %2.6f\n", paraTime);
       printf("Speedup: %2.6f\n", seqTime/paraTime);
     }
  }

  //delete the objects
  if (runSeq) delete seq;
  for (int32_t i = 0; i < 3; i++) if (runPara[i]) delete paraPtrs[i];
}

/*
 * createSortData
 * Dynamically allocates space for size int32_t values and initializes those
 * values
 * Input:
 * size - number of elements to be allocated 
 * Output:
 * pointer to the allocated data
*/
int32_t * createSortData(int64_t size)
{
  int32_t * data = new int32_t[size];
  srand(time(NULL));
  for (int32_t i = 0; i < size; i++)
  {
    data[i] = random() % 10000;
  }
  return data;
}

/*
 * parseArgs
 * Takes as input the command line arguments, parses them,
 * and sets size and threadCt
 * Inputs: 
 * argc is count of command line arguments
 * argv[1] ... argv[argc - 1] are actual command line arguments
 * Returns:
 * size - size of the array to be generated 
 * threadCt - number of threads to use in the parallel sort
 * runSeq - set to true if the sequential version is to be executed
 */
void parseArgs(int32_t argc, char * argv[], uint64_t & size, int32_t & threadCt, 
               bool & runSeq, bool * runPara)
{
  if (argc < 4) usage();  //must include: sorter -n <n> and at least one of -s -1 -2 -3
  int32_t opt;
  while((opt = getopt(argc, argv, "n:t:s123")) != -1)
  {
    switch(opt)
    {
      case 'n':
        if (atoi(optarg) <= 3) //in case a negative value is entered
           size = 0;
        else
           size = (1 << atoi(optarg));
        break;
      case 't':
        threadCt = atoi(optarg);
        break;
      case 's':
        runSeq = true;
        break;
      case '1':
        runPara[0] = true;
        break;
      case '2':
        runPara[1] = true;
        break;
      case '3':
        runPara[2] = true;
        break;
      default:
        usage();
    }
  }
  /* make sure size is big enough */
  if (size <= 32) 
  {
    printf("-s argument must be greater than 5.\n"); 
    usage();
  }  
  /* use at least two threads and not more than the number of cores */
  if (threadCt < 2 || threadCt > sysconf(_SC_NPROCESSORS_ONLN)) 
  {
    printf("-t argument must be greater than 1 and less than %ld.\n", 
           sysconf(_SC_NPROCESSORS_ONLN) + 1); 
    usage();
  }  
  bool runParallel = false;
  for (int32_t i = 0; i < 3; i++) runParallel = runParallel || runPara[i];
  if (!runSeq && !runParallel)
  {
    printf("At least one sort needs to be performed (-s, -1, -2, and/or -3).\n");
    usage();
  }
}

/*
 * usage
 * Prints usage information and exits.
 */
void usage()
{
  printf("usage: sorter  -n <n> -t <t> [-s] [-1] [-2] [-3]\n\n");
  printf("\tRandomly generates an array of size (1 << <n>) integers and sorts the\n");
  printf("\tarray using up to four different techniques. If the -s option is provided,\n");
  printf("\ta sequential sort is performed. Three different parallel sorts are available,\n");
  printf("\twhich are performed when -1, -2 and/or -3 options are provided.\n");
  printf("\tParallel speedups are provided using the actual sequential sort\n");
  printf("\tor an estimated sequential sort time.\n\n");
  printf("\t<n> must be greater than 5. The size of the array to sort will be \n");
  printf("\t2 to the power of <n> (1 << <n>).\n\n");
  printf("\t<t> must be greater than 1 and less than %ld.\n\n", 
         sysconf(_SC_NPROCESSORS_ONLN) + 1);
  printf("\t-s causes sequential sort to be performed and parallel results to\n");
  printf("\tbe compared to sequential result.\n\n");
  printf("\t-1 causes parallel sort one to be performed\n\n");
  printf("\t-2 causes parallel sort two to be performed\n\n");
  printf("\t-3 causes parallel sort three to be performed\n\n");
  exit(0);
}

