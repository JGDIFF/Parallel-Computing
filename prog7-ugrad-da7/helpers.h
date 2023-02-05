#ifndef HELPERS_H
#define HELPERS_H

#include <iostream>
#include <cstdint>
#include <chrono>

#define TIMERSTART(label)                                                  \
   std::chrono::time_point<std::chrono::system_clock> a##label, b##label; \
   a##label = std::chrono::system_clock::now();

#define TIMERSTOP(label)                                                   \
    b##label = std::chrono::system_clock::now();                           \
    std::chrono::duration<double> delta##label = b##label-a##label;        

#define GETTIME(label) delta##label.count()

#endif
