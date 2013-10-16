// Copyright (c) 2013 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "device_capabilities/device_capabilities_display.h"

#include <dirent.h>
#include <sys/statfs.h>
#include <X11/Xlib.h>

#include <sstream>

DeviceCapabilitiesDisplay::DeviceCapabilitiesDisplay() {
  QueryDisplayUnits();
}

void DeviceCapabilitiesDisplay::Get(picojson::value& obj) {
  picojson::value displays = picojson::value(picojson::array(0));
  picojson::array& units_arr = displays.get<picojson::array>();
  units_arr.clear();

  for (DisplaysMap::iterator it = displays_.begin();
       it != displays_.end(); it++) {
    picojson::value unit = picojson::value(picojson::object());
    SetJsonValue(unit, it->second);
    units_arr.push_back(unit);
  }

  picojson::object& o = obj.get<picojson::object>();
  o["displays"] = displays;
}

// won't support
void DeviceCapabilitiesDisplay::AddEventListener(const std::string& event_name,
    DeviceCapabilitiesInstance* instance) {
}

// won't support
void DeviceCapabilitiesDisplay::RemoveEventListener(
    DeviceCapabilitiesInstance* instance) {
}

void DeviceCapabilitiesDisplay::QueryDisplayUnits() {
  DIR* dir = opendir("/tmp/.X11-unix");
  if (NULL == dir) {
    return;
  }

  struct dirent entry;
  struct dirent* dr;
  for (int return_code = readdir_r(dir, &entry, &dr);
        return_code == 0 && dr != NULL;
        return_code = readdir_r(dir, &entry, &dr)) {
    if (entry.d_name[0] != 'X') {
      continue;
    }
    char display_name[64] = ":";
    strncat(display_name, entry.d_name + 1, 1);
    Display* dpy = XOpenDisplay(display_name);
    if (NULL == dpy) {
      continue;
    }

    DeviceDisplayUnit unit;
    unit.id = entry.d_name + 1;
    // FIXME (YuZhiqiangX) find which field reflects 'name'
    unit.name = "";
    if (unit.id == "0") {
      unit.isPrimary = true;
    } else {
      unit.isPrimary = false;
    }
    // FIXME (YuZhiqiangX) find which field reflects 'isInternal'
    unit.isInternal = unit.isPrimary;
    unit.width = DisplayWidth(dpy, DefaultScreen(dpy));;
    unit.height = DisplayHeight(dpy, DefaultScreen(dpy));;
    int physicalwidth = DisplayWidthMM(dpy, DefaultScreen(dpy));;
    int physicalheight = DisplayHeightMM(dpy, DefaultScreen(dpy));
    if (physicalwidth * physicalheight != 0) {
      unit.dpiX = static_cast<int>((unit.width * 25.4) / physicalwidth);
      unit.dpiY = static_cast<int>((unit.height * 25.4) / physicalheight);
    }
    // FIXME (YuZhiqiangX) find which field reflects 'availWidth'
    unit.availWidth = unit.width;
    // FIXME (YuZhiqiangX) find which field reflects 'availHeight'
    unit.availHeight = unit.height;

    displays_[unit.id] = unit;
    XCloseDisplay(dpy);
  }

  closedir(dir);
  return;
}

void DeviceCapabilitiesDisplay::SetJsonValue(picojson::value& obj,
                                             const DeviceDisplayUnit& unit) {
  picojson::object& o = obj.get<picojson::object>();
  o["id"] = picojson::value(unit.id);
  o["name"] = picojson::value(unit.name);
  o["isPrimary"] = picojson::value(unit.isPrimary);
  o["isInternal"] = picojson::value(unit.isInternal);
  o["dpiX"] = picojson::value(static_cast<double>(unit.dpiX));
  o["dpiY"] = picojson::value(static_cast<double>(unit.dpiY));
  o["width"] = picojson::value(static_cast<double>(unit.width));
  o["height"] = picojson::value(static_cast<double>(unit.height));
  o["availWidth"] = picojson::value(static_cast<double>(unit.availWidth));
  o["availHeight"] = picojson::value(static_cast<double>(unit.availHeight));
}
