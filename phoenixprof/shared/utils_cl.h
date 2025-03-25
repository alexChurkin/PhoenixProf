#ifndef PHPROF_UTILS_CL_H_
#define PHPROF_UTILS_CL_H_

#include "utils.h"

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
}
}  // namespace utils

#endif  // PHPROF_UTILS_CL_H_