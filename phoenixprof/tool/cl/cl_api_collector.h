#ifndef PHPROF_CL_API_COLLECTOR_H_
#define PHPROF_CL_API_COLLECTOR_H_

#include <chrono>
#include <iomanip>
#include <iostream>
#include <map>
#include <mutex>
#include <set>

#include "cl_api_tracer.h"
#include "utils.h"

struct ClFunctionCall {
  std::string function_name;
  uint64_t start_time;
  uint64_t end_time;
};

class ClApiCollector {
 public:  // User Interface
  static ClApiCollector* Create(cl_device_id device) {
    ASSERT(device != nullptr);

    ClApiCollector* collector = new ClApiCollector();
    ASSERT(collector != nullptr);

    ClApiTracer* tracer = new ClApiTracer(device, Callback, collector);
    if (tracer == nullptr || !tracer->IsValid()) {
      std::cerr << "[WARNING] Unable to create OpenCL tracer "
                << "for target device" << std::endl;
      if (tracer != nullptr) {
        delete tracer;
      }
      delete collector;
      return nullptr;
    }

    collector->EnableTracing(tracer);
    return collector;
  }

  ~ClApiCollector() {
    if (tracer_ != nullptr) {
      delete tracer_;
    }
  }

  void DisableTracing() {
    ASSERT(tracer_ != nullptr);
    bool disabled = tracer_->Disable();
    ASSERT(disabled);
  }

  ClApiCollector(const ClApiCollector& copy) = delete;
  ClApiCollector& operator=(const ClApiCollector& copy) = delete;

  std::vector<ClFunctionCall> GetFunctionCalls() {
    return function_calls_;
  }

 private:  // Implementation Details
  ClApiCollector() {}

  void EnableTracing(ClApiTracer* tracer) {
    ASSERT(tracer != nullptr);
    tracer_ = tracer;

    for (int id = 0; id < CL_FUNCTION_COUNT; ++id) {
      bool set = tracer_->SetTracingFunction(static_cast<cl_function_id>(id));
      ASSERT(set);
    }

    bool enabled = tracer_->Enable();
    ASSERT(enabled);
  }

  uint64_t GetTimestamp() const {
    std::chrono::duration<uint64_t, std::nano> timestamp =
        std::chrono::steady_clock::now() - base_time_;
    return timestamp.count();
  }

  void AddFunctionCallItem(const std::string& name, uint64_t start_time,
                           uint64_t end_time) {
    const std::lock_guard<std::mutex> lock(lock_);
    function_calls_.push_back(ClFunctionCall{name, start_time, end_time});
  }

 private:  // Callbacks
  static void Callback(cl_function_id function, cl_callback_data* callback_data,
                       void* user_data) {
    ClApiCollector* collector = reinterpret_cast<ClApiCollector*>(user_data);
    ASSERT(collector != nullptr);
    ASSERT(callback_data != nullptr);
    ASSERT(callback_data->correlationData != nullptr);

    if (callback_data->site == CL_CALLBACK_SITE_ENTER) {
      uint64_t& start_time =
          *reinterpret_cast<uint64_t*>(callback_data->correlationData);
      start_time = collector->GetTimestamp();
    } else {
      uint64_t end_time = collector->GetTimestamp();
      uint64_t& start_time =
          *reinterpret_cast<uint64_t*>(callback_data->correlationData);
      collector->AddFunctionCallItem(callback_data->functionName, start_time,
                                     end_time);
    }
  }

 private:  // Data
  ClApiTracer* tracer_ = nullptr;
  std::chrono::time_point<std::chrono::steady_clock> base_time_;
  std::vector<ClFunctionCall> function_calls_;

  std::mutex lock_;
};

#endif  // PHPROF_CL_API_COLLECTOR_H_