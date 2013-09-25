// Copyright (c) 2013 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "device_capabilities/device_capabilities_instance.h"
#include "device_capabilities/device_capabilities_utils.h"

void DeviceCapabilitiesInstance::HandleSyncMessage(const char* message) {
  picojson::value v;

  std::string err;
  picojson::parse(v, message, message + strlen(message), &err);
  if (!err.empty()) {
    std::cout << "Ignoring message.\n";
    return;
  }

  std::string cmd = v.get("cmd").to_str();
  if (cmd == "getSyncCPUInfo") {
    HandleSyncGetCPUInfo();
  } else {
    std::cout << "Not supported sync api " << cmd << "().\n";
  }
}

void DeviceCapabilitiesInstance::HandleMessage(const char* message) {
  picojson::value v = ParseJSONMessage(message);
  std::string cmd = v.get("cmd").to_str();

  if (cmd == "getCPUInfo") {
    HandleGetDeviceInfo("CPU", v);
  } else if (cmd == "getMemoryInfo") {
    HandleGetDeviceInfo("Memory", v);
  } else if (cmd == "getStorageInfo") {
    HandleGetDeviceInfo("Storage", v);
  } else if (cmd == "getDisplayInfo") {
    HandleGetDeviceInfo("Display", v);
  } else if (cmd == "addEventListener") {
    HandleAddEventListener(v);
  }
}

void DeviceCapabilitiesInstance::HandleGetDeviceInfo(std::string deviceName, const picojson::value& msg) {
  std::string reply_id = msg.get("_reply_id").to_str();
  picojson::value output = picojson::value(picojson::object());
  picojson::object& o = output.get<picojson::object>();
  o["_reply_id"] = picojson::value(reply_id);

  if (deviceName == "CPU") {
    device_cpu_.Get(output);
  } else if (deviceName == "Memory") {
    // device_memory_.Get(output);
  } else if (deviceName == "Storage") {
    device_storage_.Get(output);
  } else if (deviceName == "Display") {
    // device_display_.Get(output);
  }

  PostMessage(output.serialize().c_str());  
}

void DeviceCapabilitiesInstance::HandleAddEventListener(const picojson::value& msg) {  
  std::string event_name = msg.get("eventName").to_str();
  if (event_name == "onattach" || event_name == "ondetach") {
    device_storage_.AddEventListener();
  } else if(event_name == "onconnect" || event_name == "ondisconnect") {
    //device_display_.AddEventListener();
  }
}

void DeviceCapabilitiesInstance::HandleSyncGetCPUInfo() {
  picojson::value output = picojson::value(picojson::object());
  device_cpu_.Get(output);
  SendSyncReply(output.serialize().c_str());  
}
