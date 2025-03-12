#include "tool.h"

void __attribute__((constructor)) Load() {
  StartProfiling();
}

void __attribute__((destructor)) Unload() {
  StopProfiling();
}
