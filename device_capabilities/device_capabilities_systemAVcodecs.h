// Copyright (c) 2013 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DEVICE_CAPABILITIES_DEVICE_CAPABILITIES_SYSTEMAVCODECS_H_
#define DEVICE_CAPABILITIES_DEVICE_CAPABILITIES_SYSTEMAVCODECS_H_

#include <string>

#include "common/extension_adapter.h"
#include "common/picojson.h"
#include "common/utils.h"
#include "device_capabilities/device_capabilities_utils.h"

struct AudioCodec {
  std::string format;
}

struct VideoCodec {
  std::string format;
  bool hwAccel;
  bool encode;
}

class DeviceCapabilitiesSystemAVCodecs : public DeviceCapabilitiesObject {
 public:
  static DeviceCapabilitiesSystemAVCodecs& GetDeviceCapabilitiesSystemAVCodecs() {
    static DeviceCapabilitiesSystemAVCodecs instance;
    return instance;
  }
  ~DeviceCapabilitiesSystemAVCodecs() {}
  void Get(picojson::value& obj);
  void AddEventListener(std::string event_name,
                        DeviceCapabilitiesInstance* instance) { }
  void RemoveEventListener(DeviceCapabilitiesInstance* instance) { }

 private:
  explicit DeviceCapabilitiesSystemAVCodecs() { }

#if defined(TIZEN_MOBILE)
  bool QueryAudioCodec();
  bool QueryVideoCodec();
  void SetJsonValue(picojson::value& obj);
#endif

  std::list<AudioCodec> audioCodecs;
  std::list<VideoCodec> videoCodecs;
};

#endif  // DEVICE_CAPABILITIES_DEVICE_CAPABILITIES_SYSTEMAVCODECS_H_
