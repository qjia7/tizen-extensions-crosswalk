#include "device_capabilities/device_capabilities_storage.h"

#include <sys/statfs.h>

void DeviceCapabilitiesStorage::Get(picojson::value& obj) {
  QueryInternal();
  QueryMMC();
  picojson::value storages = picojson::value(picojson::array(0));
  picojson::array& units_arr = storages.get<picojson::array>();
  units_arr.clear();
  for (StoragesMap::iterator it = storages_.begin(); it != storages_.end(); it++) {
    picojson::value unit = picojson::value(picojson::object());
    SetJsonValue(unit, it->second);    
    units_arr.push_back(unit);
  }
  picojson::object& o = obj.get<picojson::object>();
  o["storages"] = storages;
}

void DeviceCapabilitiesStorage::AddEventListener() {
  vconf_notify_key_changed(VCONFKEY_SYSMAN_MMC_MOUNT,
      (vconf_callback_fn)OnStorageUnitAttach, this);
  vconf_notify_key_changed(VCONFKEY_SYSMAN_MMC_UNMOUNT,
      (vconf_callback_fn)OnStorageUnitDetach, this);
}

void DeviceCapabilitiesStorage::QueryInternal() {
  struct statfs fs;
  if (statfs("/opt/usr/media", &fs) < 0) {
    std::cout << "Internal Storage path Error \n";
    return;
  }

  DeviceStorageUnit unit;
//  unit.id = fs.f_fsid;
  unit.id = "storage1";
  unit.name = "test1";
  unit.type = "fixed";
  unit.capability = static_cast<double>(fs.f_bsize) *
                    static_cast<double>(fs.f_blocks);
  storages_[unit.id] = unit;
}

void DeviceCapabilitiesStorage::QueryMMC() {
  int sdcard_state;
  if ((vconf_get_int(VCONFKEY_SYSMAN_MMC_STATUS, &sdcard_state) != 0) ||
      (sdcard_state != VCONFKEY_SYSMAN_MMC_MOUNTED)) {
    return;
  }

  struct statfs fs;
  if (statfs("/opt/storage/sdcard", &fs) < 0) {
    std::cout << "MMC mount path error \n";
    return;
  }

  DeviceStorageUnit unit;
//  unit.id = fs.f_fsid;
  unit.id = "storage2";
  unit.name = "test2";
  unit.type = "removable";
  unit.capability = static_cast<double>(fs.f_bsize) *
                    static_cast<double>(fs.f_blocks);
  storages_[unit.id] = unit;
}

void DeviceCapabilitiesStorage::SetJsonValue(picojson::value& obj,
                                             const DeviceStorageUnit& unit) {
  picojson::object& o = obj.get<picojson::object>();
  o["id"] = picojson::value(unit.id);
  o["name"] = picojson::value(unit.name);
  o["type"] = picojson::value(unit.type);
  o["capability"] = picojson::value(unit.capability); 
}

void DeviceCapabilitiesStorage::UpdateStorageUnits(std::string command) {
  picojson::value output = picojson::value(picojson::object());
  
  picojson::value storages = picojson::value(picojson::array(0));
  picojson::array& units_arr = storages.get<picojson::array>();
  for (StoragesMap::iterator it = storages_.begin(); it != storages_.end(); it++) {
    picojson::value unit = picojson::value(picojson::object());
    SetJsonValue(unit, it->second);
    units_arr.push_back(unit);
  }

  picojson::object& o = output.get<picojson::object>();
  o["cmd"] = picojson::value(command);
  o["storages"] = storages;

  std::string result = output.serialize();
//  const char* result_as_cstr = result.c_str();
//  AutoLock lock(&(instance->events_list_mutex_));
//  for (SystemInfoEventsList::iterator it = storage_events_.begin();
//       it != storage_events_.end(); it++) {
//    (*it)->PostMessage(result_as_cstr);
//  }
}

void DeviceCapabilitiesStorage::OnStorageUnitAttach(keynode_t* node, void* user_data) {
//  int wfd = vconf_keynode_get_int(node);
  DeviceCapabilitiesStorage* instance =
      static_cast<DeviceCapabilitiesStorage*>(user_data);

  instance->UpdateStorageUnits("attachStorage");
}

void DeviceCapabilitiesStorage::OnStorageUnitDetach(keynode_t* node, void* user_data) {
//  int wfd = vconf_keynode_get_int(node);
  DeviceCapabilitiesStorage* instance =
      static_cast<DeviceCapabilitiesStorage*>(user_data);

  instance->UpdateStorageUnits("detachStorage");
}
