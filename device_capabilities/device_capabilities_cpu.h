// Copyright (c) 2013 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DEVICE_CAPABILITIES_DEVICE_CAPABILITIES_CPU_H_
#define DEVICE_CAPABILITIES_DEVICE_CAPABILITIES_CPU_H_

#include <string>

#include "common/extension_adapter.h"
#include "common/picojson.h"
#include "common/utils.h"

class DeviceCapabilitiesCpu {
 public:
  static DeviceCapabilitiesCpu& GetDeviceCapabilitiesCpu() {
    static DeviceCapabilitiesCpu instance;
    return instance;
  }
  ~DeviceCapabilitiesCpu() {}
  void Get(picojson::value& obj);

 private:
  explicit DeviceCapabilitiesCpu()
      : numOfProcessors_(0),
        archName_(""),
        load_(0.0),
        old_total_(0),
        old_used_(0) { }

  bool QueryNumOfProcessors();
  bool QueryArchName();
  bool QueryLoad();
  void SetJsonValue(picojson::value& obj);

  unsigned int numOfProcessors_;
  std::string archName_;
  double load_;
  unsigned long long old_total_; //NOLINT
  unsigned long long old_used_; //NOLINT
};

#endif  // DEVICE_CAPABILITIES_DEVICE_CAPABILITIES_CPU_H_

