// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "waffle/wayland/wayland_data_device_manager.h"

#include <wayland/protocols/wayland-server-protocol.h>

#include <cassert>

#include "waffle/logger.h"

namespace waffle {

struct WaylandDataDeviceManager::Impl : WaylandResource::Data {
  WaylandResource data_device_manager;
  WaylandResource data_device;

  static const struct wl_data_device_manager_interface
      data_device_manager_interface;
  static const struct wl_data_device_interface data_device_interface;
};

const struct wl_data_device_manager_interface
    WaylandDataDeviceManager::Impl::data_device_manager_interface {
  .create_data_source =
      +[](wl_client* client, wl_resource* resource, uint32_t id) {
        WAFFLE_LOG(WARNING) << "wl_data_device_manager_interface.create_data_"
                               "source is called (not yet implemented).";
      },
  .get_data_device = +[](wl_client* client,
                         wl_resource* resource,
                         uint32_t id,
                         wl_resource* seat) {
    WAFFLE_LOG(TRACE)
        << "wl_data_device_manager_interface.get_data_device is called.";

    auto impl = WaylandResource(resource).Get<Impl>();
    if (!impl || impl->data_device.IsNull()) {
      WAFFLE_LOG(WARNING) << "Wayland resource is invalid.";
      return;
    }
    impl->data_device.Create(
        impl, client, id, &wl_data_device_interface,
        WL_DATA_DEVICE_MANAGER_GET_DATA_DEVICE_SINCE_VERSION,
        &Impl::data_device_interface);
  }
};

const struct wl_data_device_interface
    WaylandDataDeviceManager::Impl::data_device_interface {
  .start_drag =
      +[](wl_client* client,
          wl_resource* resource,
          wl_resource* source,
          wl_resource* origin,
          wl_resource* icon,
          uint32_t serial) {
        WAFFLE_LOG(WARNING) << "wl_data_device_interface.start_drag is called"
                               "(not yet implemented).";
      },
  .set_selection =
      +[](wl_client* client,
          wl_resource* resource,
          wl_resource* source,
          uint32_t serial) {
        WAFFLE_LOG(WARNING) << "wl_data_device_interface.set_selection is "
                               "called(not yet implemented).";
      },
  .release = +[](wl_client* client, wl_resource* resource) {
    WAFFLE_LOG(TRACE) << "wl_data_device_interface.release called.";

    WaylandResource(resource).Destroy();
  },
};

WaylandDataDeviceManager::WaylandDataDeviceManager(wl_client* client,
                                                   uint32_t id,
                                                   int32_t version) {
  WAFFLE_LOG(TRACE) << "Creating WaylandDataDeviceManager...";
  assert(version <= kWlDataDeviceManagerMaxVersion);

  auto impl = std::make_shared<Impl>();
  impl->data_device_manager.Create(impl, client, id,
                                   &wl_data_device_manager_interface, version,
                                   &Impl::data_device_manager_interface);
  impl_ = impl;
}

}  // namespace waffle
