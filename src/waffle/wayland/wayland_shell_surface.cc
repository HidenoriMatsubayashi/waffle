// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "waffle/wayland/wayland_shell_surface.h"

#include <wayland/protocols/wayland-server-protocol.h>

#include "waffle/compositor/compositor.h"
#include "waffle/logger.h"
#include "waffle/utils/vec2.h"
#include "waffle/wayland/wayland_binding_handler_delegate.h"
#include "waffle/wayland/wayland_resource.h"
#include "waffle/wayland/wayland_seat.h"

namespace waffle {

struct WaylandShellSurface::Impl : WaylandResource::Data,
                                   WaylandBindingHandler {
  wl_client* client = nullptr;
  WaylandSurface wayland_surface;
  WaylandResource resource;

  static const struct wl_shell_surface_interface wl_shell_surface_interface;

  // |WaylandBindingHandler|
  void SetSize(Vec2<int> size) {
    WAFFLE_LOG(ERROR) << "SetSize is called. (not yet implemented)";
  }

  // |WaylandBindingHandler|
  std::weak_ptr<WaylandBindingHandlerDelegate> InputInterface() {
    return wayland_surface.InputInterface();
  }
};

const struct wl_shell_surface_interface
    WaylandShellSurface::Impl::wl_shell_surface_interface {
  .pong =
      +[](wl_client* client, wl_resource* resource, uint32_t serial) {
        WAFFLE_LOG(ERROR) << "wl_shell_surface_interface.pong is called (not "
                             "yet implemented)";
      },
  .move =
      +[](wl_client* client,
          wl_resource* resource,
          wl_resource* seat,
          uint32_t serial) {
        WAFFLE_LOG(ERROR) << "wl_shell_surface_interface.move is called (not "
                             "yet implemented)";
      },
  .resize =
      +[](wl_client* client,
          wl_resource* resource,
          wl_resource* seat,
          uint32_t serial,
          uint32_t edges) {
        WAFFLE_LOG(ERROR) << "wl_shell_surface_interface.resize is called (not "
                             "yet implemented)";
      },
  .set_toplevel =
      +[](wl_client* client, wl_resource* resource) {
        WAFFLE_LOG(ERROR) << "wl_shell_surface_interface.set_toplevel is "
                             "called (not yet implemented)";
      },
  .set_transient =
      +[](wl_client* client,
          wl_resource* resource,
          wl_resource* parent,
          int32_t x,
          int32_t y,
          uint32_t flags) {
        WAFFLE_LOG(ERROR) << "wl_shell_surface_interface.set_transient is "
                             "called (not yet implemented)";
      },
  .set_fullscreen =
      +[](wl_client* client,
          wl_resource* resource,
          uint32_t method,
          uint32_t framerate,
          wl_resource* output) {
        WAFFLE_LOG(ERROR) << "wl_shell_surface_interface.set_fullscreen is "
                             "called (not yet implemented)";
      },
  .set_popup =
      +[](wl_client* client,
          wl_resource* resource,
          wl_resource* seat,
          uint32_t serial,
          wl_resource* parent,
          int32_t x,
          int32_t y,
          uint32_t flags) {
        WAFFLE_LOG(ERROR) << "wl_shell_surface_interface.set_popup called is "
                             "(not yet implemented)";
      },
  .set_maximized =
      +[](wl_client* client, wl_resource* resource, wl_resource* output) {
        WAFFLE_LOG(ERROR) << "wl_shell_surface_interface.set_maximized is "
                             "called (not yet implemented)";
      },
  .set_title =
      +[](wl_client* client, wl_resource* resource, const char* title) {
        WAFFLE_LOG(ERROR) << "wl_shell_surface_interface.set_title called is "
                             "(not yet implemented)";
      },
  .set_class =
      +[](wl_client* client, wl_resource* resource, const char* class_) {
        WAFFLE_LOG(ERROR) << "wl_shell_surface_interface.set_class is called "
                             "(not yet implemented)";
      },
};

WaylandShellSurface::WaylandShellSurface(wl_client* client,
                                         uint32_t id,
                                         uint version,
                                         WaylandSurface surface) {
  WAFFLE_LOG(TRACE) << "Creating WaylandShellSurface ...";

  auto impl = std::make_shared<Impl>();
  waffle::Compositor::Instance()->AddWindow(impl);

  impl->wayland_surface = surface;
  impl->client = client;
  impl->SetTexture(surface.GetTexture());
  impl->resource.Create(impl, client, id, &wl_shell_surface_interface, version,
                        &Impl::wl_shell_surface_interface);
  impl_ = impl;
}

}  // namespace waffle
