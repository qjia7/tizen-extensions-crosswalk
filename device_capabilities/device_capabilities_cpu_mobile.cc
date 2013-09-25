#include "device_capabilities/device_capabilities_cpu.h"

void DeviceCapabilitiesCpu::Get(picojson::value& obj){
  if (QueryNumOfProcessors() ||
      QueryArchName()) {
    SetJsonValue(obj);
  }
}

void DeviceCapabilitiesCpu::SetJsonValue(picojson::value& obj) {
  picojson::object& o = obj.get<picojson::object>();
  o["numOfProcessors"] = picojson::value(static_cast<double>(numOfProcessors_));
  o["archName"] = picojson::value(archName_);
//  o["load"] = picojson::value(load_);
}                    

bool DeviceCapabilitiesCpu::QueryNumOfProcessors() {
  // FIXME (qjia7): use the actual value to set numOfProcessors
  numOfProcessors_ = 2;
  return true;
}

bool DeviceCapabilitiesCpu::QueryArchName() {
  // FIXME (qjia7): use the actual value to set archName_
  archName_ = "IA";
  return true;
}

