#include <iostream>

#include "utils.h"
using namespace std;

// External Tool Interface

extern "C" PHPROF_EXPORT void ShowHelp() {
  std::cout << "Usage: ./phoenixprof <target application> <args>" << std::endl;
}

extern "C" PHPROF_EXPORT int ProcessArgs(int argc, char *argv[]) { return 1; }

extern "C" PHPROF_EXPORT void PrepareEnv() {}

// Internal Tool Interface

void StartProfiling() {
  // std::cout << "StartProfiling called!\n";
}

void StopProfiling() {
  // std::cout << "StopProfiling called!\n";
}
