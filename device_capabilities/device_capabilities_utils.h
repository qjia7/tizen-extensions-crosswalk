// Copyright (c) 2013 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DEVICE_CAPABILITIES_DEVICE_CAPABILITIES_UTILS_H_
#define DEVICE_CAPABILITIES_DEVICE_CAPABILITIES_UTILS_H_

#include "device_capabilities/device_capabilities_cpu.h"
#include "device_capabilities/device_capabilities_storage.h"
#include "common/picojson.h"

DeviceCapabilitiesCpu device_cpu_(DeviceCapabilitiesCpu::GetDeviceCapabilitiesCpu());
DeviceCapabilitiesStorage device_storage_(DeviceCapabilitiesStorage::GetDeviceCapabilitiesStorage());

picojson::value ParseJSONMessage(const char* message) {
  picojson::value result;
  std::string err;
  picojson::parse(result, message, message + strlen(message), &err);
  if (!err.empty()) {
    std::cerr << "Notification: error parsing JSON message '"
              << err << "'\n";
  }
  return result;
}

#endif  // DEVICE_CAPABILITIES_DEVICE_CAPABILITIES_UTILS_H_
