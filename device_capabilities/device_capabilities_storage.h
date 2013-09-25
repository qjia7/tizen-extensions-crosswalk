#ifndef DEVICE_CAPABILITIES_DEVICE_CAPABILITIES_STORAGE_H_
#define DEVICE_CAPABILITIES_DEVICE_CAPABILITIES_STORAGE_H_

#include <vconf.h>

#include <map>

#include "common/extension_adapter.h"
#include "common/picojson.h"
#include "common/utils.h"

class DeviceCapabilitiesStorage {
public:
  static DeviceCapabilitiesStorage& GetDeviceCapabilitiesStorage() {
    static DeviceCapabilitiesStorage instance;
    return instance;
  }
  ~DeviceCapabilitiesStorage () {}
  void Get(picojson::value& obj);
  void AddEventListener();
  struct DeviceStorageUnit {
    std::string id;
    std::string name;
    std::string type;
    double capability;
  };

private:
  explicit DeviceCapabilitiesStorage() { }

  void SetJsonValue(picojson::value& obj, const DeviceStorageUnit& unit);
  void QueryInternal();
  void QueryMMC();
  void UpdateStorageUnits(std::string command);
  void static OnStorageUnitAttach(keynode_t* node, void* user_data);
  void static OnStorageUnitDetach(keynode_t* node, void* user_data);

  typedef std::map<std::string, DeviceStorageUnit> StoragesMap;
  StoragesMap storages_;
};

#endif  // DEVICE_CAPABILITIES_DEVICE_CAPABILITIES_STORAGE_H_

