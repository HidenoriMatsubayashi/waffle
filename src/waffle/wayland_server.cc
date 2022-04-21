// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "waffle/wayland_server.h"

#include <cassert>

#include "waffle/logger.h"
#include "waffle/wayland/wayland_data_device_manager.h"
#include "waffle/wayland/wayland_region.h"
#include "waffle/wayland/wayland_resource.h"
#include "waffle/wayland/wayland_seat.h"
#include "waffle/wayland/wayland_shell_surface.h"
#include "waffle/wayland/wayland_surface.h"
#include "waffle/wayland/xdg_shell_surface.h"

namespace waffle {

namespace {

constexpr uint32_t kWlCompositorMaxVersion = 4;
constexpr uint32_t kWlShellMaxVersion = 1;
constexpr uint32_t kZxdgShellV6MaxVersion = 1;
constexpr uint32_t kWlOutputMaxVersion = 3;

};  // namespace

const struct wl_compositor_interface WaylandServer::kWlCompositorInterface {
  .create_surface =
      +[](wl_client* client, wl_resource* resource, uint32_t id) {
        WAFFLE_LOG(TRACE)
            << "wl_compositor_interface.create_surface is called.";

        auto version = wl_resource_get_version(resource);
        WaylandSurface(client, id, version);
      },
  .create_region = +[](wl_client* client, wl_resource* resource, uint32_t id) {
    WAFFLE_LOG(TRACE) << "wl_compositor_interface.create_region is called.";

    auto version = wl_resource_get_version(resource);
    WaylandRegion(client, id, version);
  }
};

const struct zxdg_shell_v6_interface WaylandServer::kZxdgShellV6Interface {
  .destroy =
      +[](wl_client* client, wl_resource* resource) {
        WAFFLE_LOG(TRACE) << "zxdg_shell_v6_interface.destroy is called.";

        WaylandResource(resource).Destroy();
      },
  .create_positioner =
      +[](wl_client* client, wl_resource* resource, uint32_t id) {
        WAFFLE_LOG(TRACE)
            << "zxdg_shell_v6_interface.create_positioner is called.";
        WAFFLE_LOG(ERROR) << "Not implemented yet";
      },
  .get_xdg_surface =
      +[](wl_client* client,
          wl_resource* resource,
          uint32_t id,
          wl_resource* surface) {
        WAFFLE_LOG(TRACE)
            << "zxdg_shell_v6_interface.get_xdg_surface is called.";

        XdgShellSurface(
            client, id, wl_resource_get_version(resource),
            WaylandSurface::GetSurfaceFrom(WaylandResource(surface)));
      },
  .pong = +[](wl_client* client, wl_resource* resource, uint32_t serial) {
    WAFFLE_LOG(TRACE)
        << "zxdg_shell_v6_interface.zxdg_shell_v6.pong is called.";
    WAFFLE_LOG(ERROR) << "Not implemented yet";
  }
};

const struct wl_shell_interface WaylandServer::kWlShellInterface {
  .get_shell_surface = +[](wl_client* client,
                           wl_resource* resource,
                           uint32_t id,
                           wl_resource* surface) {
    WAFFLE_LOG(TRACE) << "wl_shell_interface.get_shell_surface is called.";

    WaylandShellSurface(
        client, id, wl_resource_get_version(resource),
        WaylandSurface::GetSurfaceFrom(WaylandResource(surface)));
  }
};

const struct wl_output_interface WaylandServer::kWlOutputInterface {
  .release = +[](wl_client* client, wl_resource* resource) {
    WAFFLE_LOG(TRACE) << "wl_output_interface.release is called.";

    WaylandResource(resource).Destroy();
  }
};

uint32_t WaylandServer::serial_num_ = 0;

WaylandServer::WaylandServer() {
  serial_num_ = 0;
  display_ = wl_display_create();
  auto* socket_name = wl_display_add_socket_auto(display_);
  if (!socket_name) {
    WAFFLE_LOG(ERROR) << "Faild to add socket.";
    return;
  }

  wl_global_create(display_, &wl_compositor_interface, kWlCompositorMaxVersion,
                   this, &WaylandServer::Global);
  wl_global_create(display_, &wl_shell_interface, kWlShellMaxVersion, nullptr,
                   &WaylandServer::Shell);
  wl_global_create(display_, &zxdg_shell_v6_interface, kZxdgShellV6MaxVersion,
                   this, &WaylandServer::XdgShellV6);
  wl_global_create(display_, &wl_seat_interface, kWlSeatMaxVersion, nullptr,
                   &WaylandServer::Seat);
  wl_global_create(display_, &wl_data_device_manager_interface,
                   kWlDataDeviceManagerMaxVersion, nullptr,
                   &WaylandServer::DataDeviceManager);
  wl_global_create(display_, &wl_output_interface, kWlOutputMaxVersion, nullptr,
                   &WaylandServer::Output);

  wl_display_init_shm(display_);
  event_loop_ = wl_display_get_event_loop(display_);
}

WaylandServer::~WaylandServer() {
  if (display_) {
    wl_display_destroy(display_);
    display_ = nullptr;
  }
}

void WaylandServer::Global(wl_client* client,
                           void* data,
                           uint32_t version,
                           uint32_t id) {
  WAFFLE_LOG(TRACE) << "Server::Global is called.";
  assert(version <= kWlCompositorMaxVersion);

  auto* resource =
      wl_resource_create(client, &wl_compositor_interface, version, id);
  assert(resource);

  wl_resource_set_implementation(resource, &kWlCompositorInterface, data,
                                 nullptr);
}

void WaylandServer::Shell(wl_client* client,
                          void* data,
                          uint32_t version,
                          uint32_t id) {
  WAFFLE_LOG(TRACE) << "Server::Shell is called.";
  assert(version <= kWlShellMaxVersion);

  WaylandResource shell;
  shell.Create(nullptr, client, id, &wl_shell_interface, version,
               &kWlShellInterface);
};

void WaylandServer::XdgShellV6(wl_client* client,
                               void* data,
                               uint32_t version,
                               uint32_t id) {
  WAFFLE_LOG(TRACE) << "Server::XdgShellV6 is called.";
  assert(version <= kZxdgShellV6MaxVersion);

  WaylandResource shell;
  shell.Create(nullptr, client, id, &zxdg_shell_v6_interface, version,
               &kZxdgShellV6Interface);
};

void WaylandServer::Seat(wl_client* client,
                         void* data,
                         uint32_t version,
                         uint32_t id) {
  WAFFLE_LOG(TRACE) << "Server::Seat is called.";

  WlSeat(client, id, version);
};

void WaylandServer::DataDeviceManager(wl_client* client,
                                      void* data,
                                      uint32_t version,
                                      uint32_t id) {
  WAFFLE_LOG(TRACE) << "Server::DataDeviceManager is called.";

  WaylandDataDeviceManager(client, id, version);
}

void WaylandServer::Output(wl_client* client,
                           void* data,
                           uint32_t version,
                           uint32_t id) {
  WAFFLE_LOG(TRACE) << "Server::Output is called.";
  assert(version <= kWlOutputMaxVersion);

  WaylandResource output;
  output.Create(nullptr, client, id, &wl_output_interface, version,
                &kWlOutputInterface);

  // TODO: support different window size.
  constexpr int kWidth = 1920;
  constexpr int kHeight = 1024;
  if (version >= WL_OUTPUT_GEOMETRY_SINCE_VERSION) {
    wl_output_send_geometry(output.Resource(), 0, 0, kWidth, kHeight,
                            WL_OUTPUT_SUBPIXEL_NONE, "", "",
                            WL_OUTPUT_TRANSFORM_NORMAL);
  }

  if (version >= WL_OUTPUT_SCALE_SINCE_VERSION) {
    wl_output_send_scale(output.Resource(), 1);
  }

  if (version >= WL_OUTPUT_MODE_SINCE_VERSION) {
    wl_output_send_mode(output.Resource(),
                        WL_OUTPUT_MODE_CURRENT | WL_OUTPUT_MODE_PREFERRED,
                        kWidth, kHeight, 60000);
  }

  if (version >= WL_OUTPUT_DONE_SINCE_VERSION) {
    wl_output_send_done(output.Resource());
  }
}

void WaylandServer::HandleEvent() {
  wl_event_loop_dispatch(event_loop_, 0);
  wl_display_flush_clients(display_);
  WaylandSurface::HandleFrameCallbacks();
}

}  // namespace waffle
