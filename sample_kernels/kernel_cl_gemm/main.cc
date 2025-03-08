//==============================================================
// Copyright (C) Intel Corporation
//
// SPDX-License-Identifier: MIT
// =============================================================

#include <math.h>
#include <string.h>

#include <chrono>
#include <iostream>

#include <CL/cl.h>

#include "utils_cl.h"

#define A_VALUE 0.128f
#define B_VALUE 0.256f
#define MAX_EPS 1.0e-4f

const char* kKernelSource =
  "__kernel void GEMM(__global float* a, __global float* b,\n"
  "                   __global float* c, unsigned size) {\n"
  "  int j = get_global_id(0);\n"
  "  int i = get_global_id(1);\n"
  "  float sum = 0.0f;\n"
  "  for (unsigned k = 0; k < size; ++k) {\n"
  "    sum += a[i * size + k] * b[k * size + j];\n"
  "  }\n"
  "  c[i * size + j] = sum;\n"
  "}";

static float Check(const std::vector<float>& a, float value) {
  ASSERT(value > MAX_EPS);

  float eps = 0.0f;
  for (size_t i = 0; i < a.size(); ++i) {
    eps += fabs((a[i] - value) / value);
  }

  return eps / a.size();
}

static float RunAndCheck(cl_kernel kernel, cl_command_queue queue,
                         const std::vector<float>& a,
                         const std::vector<float>& b,
                         std::vector<float>& c,
                         unsigned size, float expected_result) {
  ASSERT(kernel != nullptr && queue != nullptr);

  ASSERT(size > 0);
  ASSERT(a.size() == size * size);
  ASSERT(b.size() == size * size);
  ASSERT(c.size() == size * size);

  cl_int status = CL_SUCCESS;
  cl_context context = utils::cl::GetContext(kernel);
  ASSERT(context != nullptr);

  cl_mem dev_a = clCreateBuffer(context, CL_MEM_READ_ONLY,
                                a.size() * sizeof(float),
                                nullptr, &status);
  ASSERT(status == CL_SUCCESS && dev_a != nullptr);
  cl_mem dev_b = clCreateBuffer(context, CL_MEM_READ_ONLY,
                                b.size() * sizeof(float),
                                nullptr, &status);
  ASSERT(status == CL_SUCCESS && dev_b != nullptr);
  cl_mem dev_c = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
                                c.size() * sizeof(float),
                                nullptr, &status);
  ASSERT(status == CL_SUCCESS && dev_c != nullptr);

  status = clEnqueueWriteBuffer(queue, dev_a, CL_FALSE, 0,
                                a.size() * sizeof(float),
                                a.data(), 0, nullptr, nullptr);
  ASSERT(status == CL_SUCCESS);
  status = clEnqueueWriteBuffer(queue, dev_b, CL_FALSE, 0,
                                b.size() * sizeof(float),
                                b.data(), 0, nullptr, nullptr);
  ASSERT(status == CL_SUCCESS);

  status = clSetKernelArg(kernel, 0, sizeof(cl_mem), &dev_a);
  ASSERT(status == CL_SUCCESS);
  status = clSetKernelArg(kernel, 1, sizeof(cl_mem), &dev_b);
  ASSERT(status == CL_SUCCESS);
  status = clSetKernelArg(kernel, 2, sizeof(cl_mem), &dev_c);
  ASSERT(status == CL_SUCCESS);
  status = clSetKernelArg(kernel, 3, sizeof(unsigned), &size);
  ASSERT(status == CL_SUCCESS);

  size_t global_work_size[]{size, size};
  cl_event event = nullptr;
  status = clEnqueueNDRangeKernel(queue, kernel, 2, nullptr, global_work_size,
                                  nullptr, 0, nullptr, &event);
  ASSERT(status == CL_SUCCESS);
  status = clFinish(queue);
  ASSERT(status == CL_SUCCESS);

  status = clEnqueueReadBuffer(queue, dev_c, CL_TRUE, 0,
                               c.size() * sizeof(float),
                               c.data(), 0, nullptr, nullptr);
  ASSERT(status == CL_SUCCESS);

  status = clReleaseMemObject(dev_a);
  ASSERT(status == CL_SUCCESS);
  status = clReleaseMemObject(dev_b);
  ASSERT(status == CL_SUCCESS);
  status = clReleaseMemObject(dev_c);
  ASSERT(status == CL_SUCCESS);

  cl_ulong start = 0, end = 0;
  status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START,
                                   sizeof(cl_ulong), &start, nullptr);
  ASSERT(status == CL_SUCCESS);
  status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END,
                                   sizeof(cl_ulong), &end, nullptr);
  ASSERT(status == CL_SUCCESS);

  status = clReleaseEvent(event);
  ASSERT(status == CL_SUCCESS);

  double time = static_cast<double>(end - start) / NSEC_IN_SEC;
  std::cout << "Matrix multiplication time: " << time <<
    " sec" << std::endl;

  return Check(c, expected_result);
}

static void Compute(cl_device_id device, const std::vector<float>& a,
                    const std::vector<float>& b, std::vector<float>& c,
                    unsigned size, unsigned repeat_count,
                    float expected_result) {
  ASSERT(device != nullptr);
  cl_int status = CL_SUCCESS;

  cl_context context = clCreateContext(nullptr, 1, &device, nullptr,
                                       nullptr, &status);
  ASSERT(status == CL_SUCCESS && context != nullptr);

  cl_queue_properties props[] = { CL_QUEUE_PROPERTIES,
                                  CL_QUEUE_PROFILING_ENABLE, 0 };
  cl_command_queue queue = clCreateCommandQueueWithProperties(
    context, device, props, &status);
  ASSERT(status == CL_SUCCESS && queue != nullptr);

  cl_program program = clCreateProgramWithSource(context, 1, &kKernelSource,
                                                 nullptr, &status);
  ASSERT(status == CL_SUCCESS && program != nullptr);
  status = clBuildProgram(program, 1, &device, nullptr, nullptr, nullptr);
  ASSERT(status == CL_SUCCESS);

  cl_kernel kernel = clCreateKernel(program, "GEMM", &status);
  ASSERT(status == CL_SUCCESS && kernel != nullptr);

  for (unsigned i = 0; i < repeat_count; ++i) {
    if (i == 0) { // Enable data collection for the first iteration
      utils::SetEnv("PHPROF_ENABLE_COLLECTION", "1");
    }

    float eps = RunAndCheck(kernel, queue, a, b, c, size, expected_result);
    std::cout << "Results are " << ((eps < MAX_EPS) ? "" : "IN") <<
      "CORRECT with accuracy: " << eps << std::endl;

    if (i == 0) { // Disable data collection for the rest iterations
      utils::SetEnv("PHPROF_ENABLE_COLLECTION", "");
    }
  }

  status = clReleaseKernel(kernel);
  ASSERT(status == CL_SUCCESS);
  status = clReleaseProgram(program);
  ASSERT(status == CL_SUCCESS);
  status = clReleaseCommandQueue(queue);
  ASSERT(status == CL_SUCCESS);
  status = clReleaseContext(context);
  ASSERT(status == CL_SUCCESS);
}

std::vector<cl_device_id> get_target_devices_list(const char* device_types) {
  std::vector<cl_device_id> devices_list;

  if (strcmp(device_types, "cpu") == 0 || strcmp(device_types, "cpugpu") == 0) {
    cl_device_id device = utils::cl::GetIntelDevice(CL_DEVICE_TYPE_CPU);
    if (device == nullptr) {
      std::cout << "Unable to find target CPU device" << std::endl;
      ASSERT(false);
    }
    devices_list.push_back(device);
  }

  if (strcmp(device_types, "gpu") == 0 || strcmp(device_types, "cpugpu") == 0) {
    cl_device_id device = utils::cl::GetIntelDevice(CL_DEVICE_TYPE_GPU);
    if (device == nullptr) {
      std::cout << "Unable to find target GPU device" << std::endl;
      ASSERT(false);
    }
    devices_list.push_back(device);
  }

  return devices_list;
}

int run_on_target_device(cl_device_id device, int size, int repeat_count) {
  std::cout << "OpenCL Matrix Multiplication (matrix size: " << size <<
    " x " << size << ", repeats " << repeat_count << " times)" << std::endl;
  std::cout << "Target device: " << utils::cl::GetDeviceName(device) <<
    std::endl;

  std::vector<float> a(size * size, A_VALUE);
  std::vector<float> b(size * size, B_VALUE);
  std::vector<float> c(size * size, 0.0f);

  auto start = std::chrono::steady_clock::now();
  float expected_result = A_VALUE * B_VALUE * size;
  Compute(device, a, b, c, size, repeat_count, expected_result);
  auto end = std::chrono::steady_clock::now();
  std::chrono::duration<float> time = end - start;

  std::cout << "Total execution time: " << time.count() << " sec"
    << std::endl << std::endl;
  return 0;
}

int main(int argc, char* argv[]) {
  const char* device_types = "cpu";
  if (argc > 1) {
    device_types = argv[1];
  }
  auto target_devices_list = get_target_devices_list(device_types);

  unsigned size = 1024;
  if (argc > 2) {
    size = std::stoul(argv[2]);
  }

  unsigned repeat_count = 4;
  if (argc > 3) {
    repeat_count = std::stoul(argv[3]);
  }

  for (cl_device_id device: target_devices_list) {
    run_on_target_device(device, size, repeat_count);
  }

  return 0;
}
