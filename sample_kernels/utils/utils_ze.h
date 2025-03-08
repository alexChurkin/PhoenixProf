//==============================================================
// Copyright (C) Intel Corporation
//
// SPDX-License-Identifier: MIT
// =============================================================

#ifndef UTILS_ZE_H_
#define UTILS_ZE_H_

#include <string.h>

#include <chrono>
#include <string>
#include <thread>
#include <vector>

#include <level_zero/ze_api.h>
#include <level_zero/zet_api.h>

#include "utils.h"

namespace utils {
namespace ze {

inline std::vector<ze_driver_handle_t> GetDriverList() {
  ze_result_t status = ZE_RESULT_SUCCESS;

  uint32_t driver_count = 0;
  status = zeDriverGet(&driver_count, nullptr);
  ASSERT(status == ZE_RESULT_SUCCESS);

  if (driver_count == 0) {
    return std::vector<ze_driver_handle_t>();
  }

  std::vector<ze_driver_handle_t> driver_list(driver_count);
  status = zeDriverGet(&driver_count, driver_list.data());
  ASSERT(status == ZE_RESULT_SUCCESS);

  return driver_list;
}

inline std::vector<ze_device_handle_t> GetDeviceList(ze_driver_handle_t driver) {
  ASSERT(driver != nullptr);
  ze_result_t status = ZE_RESULT_SUCCESS;

  uint32_t device_count = 0;
  status = zeDeviceGet(driver, &device_count, nullptr);
  ASSERT(status == ZE_RESULT_SUCCESS);

  if (device_count == 0) {
    return std::vector<ze_device_handle_t>();
  }

  std::vector<ze_device_handle_t> device_list(device_count);
  status = zeDeviceGet(driver, &device_count, device_list.data());
  ASSERT(status == ZE_RESULT_SUCCESS);

  return device_list;
}

inline std::vector<ze_device_handle_t> GetDeviceList() {
  std::vector<ze_device_handle_t> device_list;
  for (auto driver : utils::ze::GetDriverList()) {
    for (auto device : utils::ze::GetDeviceList(driver)) {
      device_list.push_back(device);
    }
  }
  return device_list;
}

inline std::vector<ze_device_handle_t> GetSubDeviceList(
    ze_device_handle_t device) {
  ASSERT(device != nullptr);
  ze_result_t status = ZE_RESULT_SUCCESS;

  uint32_t sub_device_count = 0;
  status = zeDeviceGetSubDevices(device, &sub_device_count, nullptr);
  ASSERT(status == ZE_RESULT_SUCCESS);

  if (sub_device_count == 0) {
    return std::vector<ze_device_handle_t>();
  }

  std::vector<ze_device_handle_t> sub_device_list(sub_device_count);
  status = zeDeviceGetSubDevices(
      device, &sub_device_count, sub_device_list.data());
  ASSERT(status == ZE_RESULT_SUCCESS);

  return sub_device_list;
}

inline ze_driver_handle_t GetGpuDriver() {
  std::vector<ze_driver_handle_t> driver_list;

  for (auto driver : GetDriverList()) {
    for (auto device : GetDeviceList(driver)) {
      ze_device_properties_t props{ZE_STRUCTURE_TYPE_DEVICE_PROPERTIES, };
      ze_result_t status = zeDeviceGetProperties(device, &props);
      ASSERT(status == ZE_RESULT_SUCCESS);
      if (props.type == ZE_DEVICE_TYPE_GPU) {
        driver_list.push_back(driver);
      }
    }
  }

  if (driver_list.empty()) {
    return nullptr;
  }

  uint32_t device_id = 0;
  ASSERT(device_id >= 0 && device_id < driver_list.size());
  return driver_list[device_id];
}

inline ze_device_handle_t GetGpuDevice() {
  std::vector<ze_device_handle_t> device_list;

  for (auto driver : GetDriverList()) {
    for (auto device : GetDeviceList(driver)) {
      ze_device_properties_t props{ZE_STRUCTURE_TYPE_DEVICE_PROPERTIES, };
      ze_result_t status = zeDeviceGetProperties(device, &props);
      ASSERT(status == ZE_RESULT_SUCCESS);
      if (props.type == ZE_DEVICE_TYPE_GPU) {
        device_list.push_back(device);
      }
    }
  }

  if (device_list.empty()) {
    return nullptr;
  }

  uint32_t device_id = 0;
  return device_list[device_id];
}

inline ze_context_handle_t GetContext(ze_driver_handle_t driver) {
  ASSERT(driver != nullptr);

  ze_result_t status = ZE_RESULT_SUCCESS;
  ze_context_handle_t context = nullptr;
  ze_context_desc_t context_desc = {
      ZE_STRUCTURE_TYPE_CONTEXT_DESC, nullptr, 0};

  status = zeContextCreate(driver, &context_desc, &context);
  ASSERT(status == ZE_RESULT_SUCCESS);
  return context;
}

inline std::string GetDeviceName(ze_device_handle_t device) {
  ASSERT(device != nullptr);
  ze_result_t status = ZE_RESULT_SUCCESS;
  ze_device_properties_t props{ZE_STRUCTURE_TYPE_DEVICE_PROPERTIES, };
  status = zeDeviceGetProperties(device, &props);
  ASSERT(status == ZE_RESULT_SUCCESS);
  return props.name;
}

inline std::string GetResultType(zet_value_type_t type) {
  switch (type) {
    case ZET_VALUE_TYPE_UINT32:
      return "UINT32";
    case ZET_VALUE_TYPE_UINT64:
      return "UINT64";
    case ZET_VALUE_TYPE_FLOAT32:
      return "FLOAT32";
    case ZET_VALUE_TYPE_FLOAT64:
      return "FLOAT64";
    case ZET_VALUE_TYPE_BOOL8:
      return "BOOL8";
    default:
      break;
  }
  return "UNKNOWN";
}

inline ze_api_version_t GetDriverVersion(ze_driver_handle_t driver) {
  ASSERT(driver != nullptr);

  ze_api_version_t version = ZE_API_VERSION_FORCE_UINT32;
  ze_result_t status = zeDriverGetApiVersion(driver, &version);
  ASSERT(status == ZE_RESULT_SUCCESS);

  return version;
}

inline ze_api_version_t GetVersion() {
  auto driver_list = GetDriverList();
  if (driver_list.empty()) {
    return ZE_API_VERSION_FORCE_UINT32;
  }
  return GetDriverVersion(driver_list.front());
}

} // namespace ze
} // namespace utils

#endif // UTILS_ZE_H_
