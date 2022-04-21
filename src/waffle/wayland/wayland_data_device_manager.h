// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WAFFLE_WAYLAND_WAYLAND_DATA_DEVICE_MANAGER_H_
#define WAFFLE_WAYLAND_WAYLAND_DATA_DEVICE_MANAGER_H_

#include "waffle/wayland/wayland_resource.h"

namespace waffle {

constexpr uint kWlDataDeviceManagerMaxVersion = 3;

class WaylandDataDeviceManager {
 public:
  WaylandDataDeviceManager(wl_client* client, uint32_t id, int32_t version);
  ~WaylandDataDeviceManager() = default;

 private:
  struct Impl;
  std::weak_ptr<Impl> impl_;
};

}  // namespace waffle

#endif  // WAFFLE_WAYLAND_WAYLAND_DATA_DEVICE_MANAGER_H_
