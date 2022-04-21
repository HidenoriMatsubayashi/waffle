// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WAFFLE_WAYLAND_SERVER_H_
#define WAFFLE_WAYLAND_SERVER_H_

#include <wayland-server-protocol.h>
#include <wayland-server.h>
#include <wayland/protocols/xdg-shell-server-protocol.h>

namespace waffle {

class WaylandServer {
 public:
  WaylandServer();
  ~WaylandServer();

  static void Global(wl_client* client,
                     void* data,
                     uint32_t version,
                     uint32_t id);
  static void Shell(wl_client* client,
                    void* data,
                    uint32_t version,
                    uint32_t id);
  static void XdgShellV6(wl_client* client,
                         void* data,
                         uint32_t version,
                         uint32_t id);
  static void Seat(wl_client* client,
                   void* data,
                   uint32_t version,
                   uint32_t id);
  static void DataDeviceManager(wl_client* client,
                                void* data,
                                uint32_t version,
                                uint32_t id);
  static void Output(wl_client* client,
                     void* data,
                     uint32_t version,
                     uint32_t id);
  static uint32_t SerialNumber() { return ++serial_num_; }
  wl_display* Display() { return display_; }
  void HandleEvent();

 private:
  static const struct wl_compositor_interface kWlCompositorInterface;
  static const struct zxdg_shell_v6_interface kZxdgShellV6Interface;
  static const struct wl_shell_interface kWlShellInterface;
  static const struct wl_output_interface kWlOutputInterface;

  static uint32_t serial_num_;

  wl_display* display_ = nullptr;
  wl_event_loop* event_loop_ = nullptr;
};

}  // namespace waffle

#endif  // WAFFLE_WAYLAND_SERVER_H_
