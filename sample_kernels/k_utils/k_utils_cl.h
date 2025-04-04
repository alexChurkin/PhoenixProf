#ifndef K_UTILS_CL_H_
#define K_UTILS_CL_H_

#include <string.h>

#include <string>
#include <vector>

#include <CL/cl.h>

#include "k_utils.h"

#define CL_KERNEL_MAX_SUB_GROUP_SIZE_FOR_NDRANGE_KHR 0x2033

namespace utils {
namespace cl {

inline std::string GetDeviceVendor(cl_device_id device) {
  char vendor[MAX_STR_SIZE] = { 0 };
  cl_int status = clGetDeviceInfo(
      device, CL_DEVICE_VENDOR, MAX_STR_SIZE, vendor, nullptr);
  ASSERT(status == CL_SUCCESS);
  return vendor;
}

inline std::vector<cl_device_id> GetDeviceList(cl_device_type type) {
  cl_int status = CL_SUCCESS;

  cl_uint platform_count = 0;
  status = clGetPlatformIDs(0, nullptr, &platform_count);
  if (status != CL_SUCCESS || platform_count == 0) {
    return std::vector<cl_device_id>();
  }

  std::vector<cl_platform_id> platform_list(platform_count, nullptr);
  status = clGetPlatformIDs(platform_count, platform_list.data(), nullptr);
  ASSERT(status == CL_SUCCESS);

  std::vector<cl_device_id> result;
  for (cl_uint i = 0; i < platform_count; ++i) {
    cl_uint device_count = 0;

    status = clGetDeviceIDs(
        platform_list[i], type, 0, nullptr, &device_count);
    if (status != CL_SUCCESS || device_count == 0) continue;

    std::vector<cl_device_id> device_list(device_count, nullptr);
    status = clGetDeviceIDs(
        platform_list[i], type, device_count, device_list.data(), nullptr);
    ASSERT(status == CL_SUCCESS);

    for (cl_uint j = 0; j < device_count; ++j) {
      if (GetDeviceVendor(device_list[j]).find("Intel") != std::string::npos) {
        result.push_back(device_list[j]);
      }
    }
  }

  return result;
}

inline std::vector<cl_device_id> CreateSubDeviceList(cl_device_id device) {
  ASSERT(device != nullptr);

  cl_int status = CL_SUCCESS;
  cl_device_partition_property props[] = {
    CL_DEVICE_PARTITION_BY_AFFINITY_DOMAIN,
    CL_DEVICE_AFFINITY_DOMAIN_NEXT_PARTITIONABLE,
    0};

  cl_uint sub_device_count = 0;
  status = clCreateSubDevices(device, props, 0, nullptr, &sub_device_count);
  ASSERT(status == CL_SUCCESS || status == CL_DEVICE_PARTITION_FAILED);

  if (status == CL_DEVICE_PARTITION_FAILED || sub_device_count == 0) {
    return std::vector<cl_device_id>();
  }

  std::vector<cl_device_id> sub_device_list(sub_device_count);
  status = clCreateSubDevices(
      device, props, sub_device_count, sub_device_list.data(), nullptr);
  ASSERT(status == CL_SUCCESS);

  return sub_device_list;
}

inline void ReleaseSubDeviceList(
    const std::vector<cl_device_id>& sub_device_list) {
  for (auto sub_device : sub_device_list) {
    cl_int status = clReleaseDevice(sub_device);
    ASSERT(status == CL_SUCCESS);
  }
}

inline cl_device_id GetIntelDevice(cl_device_type type) {
  cl_device_id target = nullptr;

  std::vector<cl_device_id> device_list = GetDeviceList(type);
  for (auto device : device_list) {
    if (GetDeviceVendor(device).find("Intel") != std::string::npos) {
      target = device;
      break;
    }
  }

  return target;
}

inline cl_device_id GetDeviceParent(cl_device_id device) {
  ASSERT(device != nullptr);

  cl_device_id parent = nullptr;
  cl_int status = clGetDeviceInfo(
      device, CL_DEVICE_PARENT_DEVICE, sizeof(cl_device_id), &parent, nullptr);
  ASSERT(status != CL_SUCCESS);

  return parent;
}

inline std::string GetDeviceName(cl_device_id device) {
  ASSERT(device != nullptr);

  char name[MAX_STR_SIZE] = { 0 };
  cl_int status = clGetDeviceInfo(
      device, CL_DEVICE_NAME, MAX_STR_SIZE, name, nullptr);
  ASSERT(status == CL_SUCCESS);

  return name;
}

inline cl_device_type GetDeviceType(cl_device_id device) {
  ASSERT(device != nullptr);

  cl_device_type type = CL_DEVICE_TYPE_ALL;
  cl_int status = clGetDeviceInfo(
      device, CL_DEVICE_TYPE, sizeof(type), &type, nullptr);
  ASSERT(status == CL_SUCCESS);
  ASSERT(type != CL_DEVICE_TYPE_ALL);

  return type;
}

inline cl_program GetProgram(cl_kernel kernel) {
  ASSERT(kernel != nullptr);

  cl_int status = CL_SUCCESS;
  cl_program program = nullptr;

  status = clGetKernelInfo(kernel, CL_KERNEL_PROGRAM, sizeof(cl_program),
                           &program, nullptr);
  ASSERT(status == CL_SUCCESS);

  return program;
}

inline cl_context GetContext(cl_kernel kernel) {
  ASSERT(kernel != nullptr);

  cl_int status = CL_SUCCESS;
  cl_context context = nullptr;

  status = clGetKernelInfo(kernel, CL_KERNEL_CONTEXT, sizeof(cl_context),
                           &context, nullptr);
  ASSERT(status == CL_SUCCESS);

  return context;
}

inline std::vector<cl_device_id> GetDeviceList(cl_program program) {
  ASSERT(program != nullptr);

  cl_int status = CL_SUCCESS;
  cl_uint device_count = 0;
  status = clGetProgramInfo(program, CL_PROGRAM_NUM_DEVICES, sizeof(cl_uint),
                            &device_count, nullptr);
  ASSERT(status == CL_SUCCESS);
  if (device_count == 0) {
    return std::vector<cl_device_id>();
  }

  std::vector<cl_device_id> device_list(device_count);
  status = clGetProgramInfo(program, CL_PROGRAM_DEVICES,
                            device_count * sizeof(cl_device_id),
                            device_list.data(), nullptr);
  ASSERT(status == CL_SUCCESS);

  return device_list;
}

inline cl_command_queue GetCommandQueue(cl_event event) {
  ASSERT(event != nullptr);

  cl_int status = CL_SUCCESS;
  cl_command_queue queue = nullptr;
  status = clGetEventInfo(event, CL_EVENT_COMMAND_QUEUE,
                          sizeof(cl_command_queue), &queue, nullptr);
  ASSERT(status == CL_SUCCESS);

  return queue;
}

inline const char* GetErrorString(cl_int error) {
  switch (error) {
    case CL_SUCCESS:
      return "CL_SUCCESS";
    case CL_DEVICE_NOT_FOUND:
      return "CL_DEVICE_NOT_FOUND";
    case CL_DEVICE_NOT_AVAILABLE:
      return "CL_DEVICE_NOT_AVAILABLE";
    case CL_COMPILER_NOT_AVAILABLE:
      return "CL_COMPILER_NOT_AVAILABLE";
    case CL_MEM_OBJECT_ALLOCATION_FAILURE:
      return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
    case CL_OUT_OF_RESOURCES:
      return "CL_OUT_OF_RESOURCES";
    case CL_OUT_OF_HOST_MEMORY:
      return "CL_OUT_OF_HOST_MEMORY";
    case CL_PROFILING_INFO_NOT_AVAILABLE:
      return "CL_PROFILING_INFO_NOT_AVAILABLE";
    case CL_MEM_COPY_OVERLAP:
      return "CL_MEM_COPY_OVERLAP";
    case CL_IMAGE_FORMAT_MISMATCH:
      return "CL_IMAGE_FORMAT_MISMATCH";
    case CL_IMAGE_FORMAT_NOT_SUPPORTED:
      return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
    case CL_BUILD_PROGRAM_FAILURE:
      return "CL_BUILD_PROGRAM_FAILURE";
    case CL_MAP_FAILURE:
      return "CL_MAP_FAILURE";
    case CL_MISALIGNED_SUB_BUFFER_OFFSET:
      return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
    case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:
      return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
    case CL_COMPILE_PROGRAM_FAILURE:
      return "CL_COMPILE_PROGRAM_FAILURE";
    case CL_LINKER_NOT_AVAILABLE:
      return "CL_LINKER_NOT_AVAILABLE";
    case CL_LINK_PROGRAM_FAILURE:
      return "CL_LINK_PROGRAM_FAILURE";
    case CL_DEVICE_PARTITION_FAILED:
      return "CL_DEVICE_PARTITION_FAILED";
    case CL_KERNEL_ARG_INFO_NOT_AVAILABLE:
      return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";
    case CL_INVALID_VALUE:
      return "CL_INVALID_VALUE";
    case CL_INVALID_DEVICE_TYPE:
      return "CL_INVALID_DEVICE_TYPE";
    case CL_INVALID_PLATFORM:
      return "CL_INVALID_PLATFORM";
    case CL_INVALID_DEVICE:
      return "CL_INVALID_DEVICE";
    case CL_INVALID_CONTEXT:
      return "CL_INVALID_CONTEXT";
    case CL_INVALID_QUEUE_PROPERTIES:
      return "CL_INVALID_QUEUE_PROPERTIES";
    case CL_INVALID_COMMAND_QUEUE:
      return "CL_INVALID_COMMAND_QUEUE";
    case CL_INVALID_HOST_PTR:
      return "CL_INVALID_HOST_PTR";
    case CL_INVALID_MEM_OBJECT:
      return "CL_INVALID_MEM_OBJECT";
    case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
      return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
    case CL_INVALID_IMAGE_SIZE:
      return "CL_INVALID_IMAGE_SIZE";
    case CL_INVALID_SAMPLER:
      return "CL_INVALID_SAMPLER";
    case CL_INVALID_BINARY:
      return "CL_INVALID_BINARY";
    case CL_INVALID_BUILD_OPTIONS:
      return "CL_INVALID_BUILD_OPTIONS";
    case CL_INVALID_PROGRAM:
      return "CL_INVALID_PROGRAM";
    case CL_INVALID_PROGRAM_EXECUTABLE:
      return "CL_INVALID_PROGRAM_EXECUTABLE";
    case CL_INVALID_KERNEL_NAME:
      return "CL_INVALID_KERNEL_NAME";
    case CL_INVALID_KERNEL_DEFINITION:
      return "CL_INVALID_KERNEL_DEFINITION";
    case CL_INVALID_KERNEL:
      return "CL_INVALID_KERNEL";
    case CL_INVALID_ARG_INDEX:
      return "CL_INVALID_ARG_INDEX";
    case CL_INVALID_ARG_VALUE:
      return "CL_INVALID_ARG_VALUE";
    case CL_INVALID_ARG_SIZE:
      return "CL_INVALID_ARG_SIZE";
    case CL_INVALID_KERNEL_ARGS:
      return "CL_INVALID_KERNEL_ARGS";
    case CL_INVALID_WORK_DIMENSION:
      return "CL_INVALID_WORK_DIMENSION";
    case CL_INVALID_WORK_GROUP_SIZE:
      return "CL_INVALID_WORK_GROUP_SIZE";
    case CL_INVALID_WORK_ITEM_SIZE:
      return "CL_INVALID_WORK_ITEM_SIZE";
    case CL_INVALID_GLOBAL_OFFSET:
      return "CL_INVALID_GLOBAL_OFFSET";
    case CL_INVALID_EVENT_WAIT_LIST:
      return "CL_INVALID_EVENT_WAIT_LIST";
    case CL_INVALID_EVENT:
      return "CL_INVALID_EVENT";
    case CL_INVALID_OPERATION:
      return "CL_INVALID_OPERATION";
    case CL_INVALID_GL_OBJECT:
      return "CL_INVALID_GL_OBJECT";
    case CL_INVALID_BUFFER_SIZE:
      return "CL_INVALID_BUFFER_SIZE";
    case CL_INVALID_MIP_LEVEL:
      return "CL_INVALID_MIP_LEVEL";
    case CL_INVALID_GLOBAL_WORK_SIZE:
      return "CL_INVALID_GLOBAL_WORK_SIZE";
    case CL_INVALID_PROPERTY:
      return "CL_INVALID_PROPERTY";
    case CL_INVALID_IMAGE_DESCRIPTOR:
      return "CL_INVALID_IMAGE_DESCRIPTOR";
    case CL_INVALID_COMPILER_OPTIONS:
      return "CL_INVALID_COMPILER_OPTIONS";
    case CL_INVALID_LINKER_OPTIONS:
      return "CL_INVALID_LINKER_OPTIONS";
    case CL_INVALID_DEVICE_PARTITION_COUNT:
      return "CL_INVALID_DEVICE_PARTITION_COUNT";
    case CL_INVALID_PIPE_SIZE:
      return "CL_INVALID_PIPE_SIZE";
    case CL_INVALID_DEVICE_QUEUE:
      return "CL_INVALID_DEVICE_QUEUE";
    default:
      break;
  }
  return "UNKNOWN";
}

} // namespace cl
} // namespace utils

#endif // K_UTILS_CL_H_