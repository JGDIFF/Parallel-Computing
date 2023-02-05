#ifndef HPC_HELPERS_H
#define HPC_HELPERS_H

#include <iostream>
#include <chrono>

//need to provide a SPEEDUP macro to this file

#define TIMERSTART(label)                                                  \
    std::chrono::time_point<std::chrono::system_clock> a##label, b##label; \
    a##label = std::chrono::system_clock::now();

#define TIMERSTOP(label)                                                   \
    b##label = std::chrono::system_clock::now();                           \
    std::chrono::duration<double> delta##label = b##label-a##label;        \
    std::cout << "# elapsed time ("<< #label <<"): "                       \
              << delta##label.count()  << "s" << std::endl;

#define SPEEDUP(label, labelx)                                             \
    double speedup##label = delta##labelx.count() / delta##label.count();  \
    std::cout << "SPEEDUP ("<< #label <<"/"<< #labelx <<") = "              \
    << speedup##label << std::endl; 
#endif
