#ifndef PHOENIXPROF_TOOL_H_
#define PHOENIXPROF_TOOL_H_
#include "utils.h"

// External Tool Interface
extern "C" void ShowHelp();
extern "C" int ProcessArgs(int argc, char *argv[]);
extern "C" void PrepareEnv();

// Internal Tool Interface
void StartProfiling();
void StopProfiling();

#endif  // PHOENIXPROF_TOOL_H_
