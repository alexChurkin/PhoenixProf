#include <iostream>

#include "cl_api_collector.h"
#include "utils_cl.h"
using namespace std;

static ClApiCollector* cpu_collector = nullptr;
static ClApiCollector* gpu_collector = nullptr;
static std::chrono::steady_clock::time_point start;

// External Tool Interface

extern "C" PHPROF_EXPORT void ShowHelp() {
  std::cout << "Usage: ./phoenixprof <target application> <args>" << std::endl;
}

extern "C" PHPROF_EXPORT int ProcessArgs(int argc, char* argv[]) { return 1; }

extern "C" PHPROF_EXPORT void PrepareEnv() {}

// Internal Tool Interface

void StartProfiling() {
  // std::cout << "StartProfiling called!\n";
  cl_device_id cpu_device = utils::cl::GetIntelDevice(CL_DEVICE_TYPE_CPU);
  cl_device_id gpu_device = utils::cl::GetIntelDevice(CL_DEVICE_TYPE_GPU);
  if (cpu_device == nullptr && gpu_device == nullptr) {
    std::cerr << "[WARNING] Unable to find device for tracing" << std::endl;
    return;
  }

  if (gpu_device == nullptr) {
    std::cerr << "[WARNING] Unable to find GPU device for tracing" << std::endl;
  }
  if (cpu_device == nullptr) {
    std::cerr << "[WARNING] Unable to find CPU device for tracing" << std::endl;
  }

  if (cpu_device != nullptr) {
    cpu_collector = ClApiCollector::Create(cpu_device);
  }
  if (gpu_device != nullptr) {
    gpu_collector = ClApiCollector::Create(gpu_device);
  }

  start = std::chrono::steady_clock::now();
}

void StopProfiling() {
  if (cpu_collector != nullptr) {
    cpu_collector->DisableTracing();
  }
  if (gpu_collector != nullptr) {
    gpu_collector->DisableTracing();
  }

  auto cpu_function_calls = cpu_collector->GetFunctionCalls();
  auto gpu_function_calls = gpu_collector->GetFunctionCalls();

  std::cout << "!!! CPU Function Calls:" << std::endl;
  for (auto elem : cpu_function_calls) {
    std::cout << elem.function_name << " " << elem.start_time << " "
              << elem.end_time << endl;
  }

  std::cout << "!!! GPU Function Calls:" << std::endl;
  for (auto elem : gpu_function_calls) {
    std::cout << elem.function_name << " " << elem.start_time << " "
              << elem.end_time << endl;
  }

  if (cpu_collector != nullptr) {
    delete cpu_collector;
  }
  if (gpu_collector != nullptr) {
    delete gpu_collector;
  }
}
