#include <iostream>
#include <getopt.h>
#include <unistd.h>
#include <string>
#include <cstdlib>
#include <bits/stdc++.h> 

//mininum array size is (1 << 4) == 2^4 == 16
#define MINSZ 4
//maximum array size is (1 << 33) == 2^33 == 8589934592
#define MAXSZ 33

static void parseArgs(int, char **, long int &, 
                      long int &, long int &, long int &);
static void usage();

/*
 * runScan -s <n> -t <m> -c <k> -r <j>
 * where 1 << <n> is the size of the array to scan
 * where 1 << <n> is the subarray size
 * where <m> is the number of threads to create in the thread pool
 * and <j> is the number of runs of the program to perform. 
 * The program outputs an average.
 */
int main(int argc, char * argv[])
{
  long int numThreads, arraySizeExp, subarraySizeExp, numRuns;
  FILE * fp;
  char output[1000];
  float speedup, averageSpeedup = 0;

  //parse the command line arguments and get the number of threads
  //and the array size
  parseArgs(argc, argv, numThreads, arraySizeExp, subarraySizeExp, numRuns);

  std::string command = "./scan -t " + std::to_string(numThreads) +
    " -s " + std::to_string(arraySizeExp) + " -c " +
    std::to_string(subarraySizeExp);
  std::string commandPlusGrep = command + " | grep \"Speedup:\"";

  for (long int i = 0; i < numRuns; i++)
  {
    std::cout << "Running: " << command << std::endl;
    fp = popen(commandPlusGrep.c_str(), "r");
    bool error = (fgets(output, sizeof(output), fp) == NULL);
    if (!error) 
    {
      error = error || (sscanf(output, "Speedup: %f", &speedup) != 1);
      averageSpeedup += speedup;
    } 
    if (error)
    {
      printf("Unable to read output of command\n");
      usage();
      exit(1);
    }
    pclose(fp);
  }
  printf("Average speedup: %f\n", averageSpeedup/(float)numRuns);

}

/*
 * parseArgs
 * Takes as input the command line arguments, parses them,
 * and sets numThreads, arraySizeExp, subarraySizeExp, and
 * numRuns.
 * Inputs: 
 * argc is count of command line arguments
 * argv[1] ... argv[argc - 1] are actual command line arguments
 * Returns:
 * arraySizeExp is set to numeric value following -s
 * numThreads is set to numeric value following -t
 * subArraySizeExp is set to numeric value following -c 
 * numRuns is set to numeric value following -r
 */
void parseArgs(int argc, char * argv[], long int & numThreads, 
               long int & arraySizeExp, long int & subarraySizeExp,
               long int & numRuns)
{
  int opt;
  long int arraySize = 0, subarraySize = 0;
  while((opt = getopt(argc, argv, "r:s:t:c:h")) != -1)  
  {  
    switch(opt)  
    {  
      case 't': 
        numThreads = atoi(optarg);
        break;
      case 's':
        arraySizeExp = (long)atoi(optarg);
        arraySize = (long)1 << arraySizeExp;  //2^s
        break;
      case 'c':
        subarraySizeExp = (long)atoi(optarg); 
        subarraySize = (long)1 << subarraySizeExp;  //2^k
        break;
      case 'r':
        numRuns = (long)atoi(optarg);  //2^k
        break;
      default:
        usage();
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

  if (arraySize < subarraySize)
  {
    printf("Array size must be greater than the subarray size.\n");
    usage();
  }

  if (numRuns <= 0)
  {
    printf("The number of runs must be greater than 0.\n");
    usage();
  }
}

/* 
 * usage
 * Prints usage information and exits.
 */
void usage()
{
  printf("usage: runScan -s <n> -t <m> -c <k> -r <j>\n\n");
  printf("\tInvokes the scan program with the -s, -t, and -c\n");
  printf("\targuments. It does this <j> times and then reports\n");
  printf("\tthe average speedup.\n");
  printf("\t<n>: 1 << <n> (e.g. 2^<n>) is size of array to scan\n");
  printf("\t<n> must be at least %d and not more than %d\n\n",
         MINSZ, MAXSZ);
  printf("\t<m> is the number of threads to use to perform the scan\n");
  printf("\t<m> must be greater than 1 and less than %ld\n\n",
         sysconf(_SC_NPROCESSORS_ONLN) + 1);
  printf("\t<k>: 1 << <k> (e.g. 2^>k>) is the size of the subarray that\n");
  printf("\teach thread will operate on.  It cannot be greater than <n>\n\n");
  printf("\t<j> is the number of runs to perform. It must be greater\n");
  printf("\tthan 0.\n\n");
  exit(0);
}


