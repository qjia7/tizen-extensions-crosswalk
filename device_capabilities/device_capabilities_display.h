// Copyright (c) 2013 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DEVICE_CAPABILITIES_DEVICE_CAPABILITIES_DISPLAY_H_
#define DEVICE_CAPABILITIES_DEVICE_CAPABILITIES_DISPLAY_H_

#include <map>
#include <string>

#include "common/extension_adapter.h"
#include "common/picojson.h"
#include "common/utils.h"
#include "device_capabilities/device_capabilities_utils.h"

struct DeviceDisplayUnit {
  std::string id;
  std::string name;
  bool isPrimary;
  bool isInternal;
  unsigned int dpiX;
  unsigned int dpiY;
  double width;
  double height;
  double availWidth;
  double availHeight;
};

class DeviceCapabilitiesDisplay : public DeviceCapabilitiesObject {
 public:
  static DeviceCapabilitiesDisplay& GetDeviceCapabilitiesDisplay() {
    static DeviceCapabilitiesDisplay instance;
    return instance;
  }
  ~DeviceCapabilitiesDisplay() {}
  void Get(picojson::value& obj);
  void AddEventListener(const std::string& event_name,
                        DeviceCapabilitiesInstance* instance);
  void RemoveEventListener(DeviceCapabilitiesInstance* instance);

 private:
  explicit DeviceCapabilitiesDisplay();

  void SetJsonValue(picojson::value& obj, const DeviceDisplayUnit& unit);
  void QueryDisplayUnits();

  typedef std::map<std::string, DeviceDisplayUnit> DisplaysMap;
  DisplaysMap displays_;
};

#endif  // DEVICE_CAPABILITIES_DEVICE_CAPABILITIES_DISPLAY_H_
