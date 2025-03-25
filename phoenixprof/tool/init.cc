#include "tool.h"

// Calls automatically when the tool library is was loaded
void __attribute__((constructor)) Load() { StartProfiling(); }

// Calls automatically before the tool library is being unloaded
void __attribute__((destructor)) Unload() { StopProfiling(); }
