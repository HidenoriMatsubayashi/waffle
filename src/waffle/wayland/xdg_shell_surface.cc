// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "waffle/wayland/xdg_shell_surface.h"

#include <wayland/protocols/xdg-shell-server-protocol.h>

#include <cassert>
#include <cstdio>

#include "waffle/compositor/compositor.h"
#include "waffle/logger.h"
#include "waffle/wayland/wayland_array.h"
#include "waffle/wayland/wayland_binding_handler_delegate.h"
#include "waffle/wayland/wayland_resource.h"
#include "waffle/wayland_server.h"

namespace waffle {

struct XdgShellSurface::Impl : WaylandResource::Data, WaylandBindingHandler {
  WaylandSurface wayland_surface;
  WaylandResource xdg_surface_resource;
  WaylandResource xdg_top_level_resource;

  static const struct zxdg_surface_v6_interface xdg_surface_v6_interface;
  static const struct zxdg_toplevel_v6_interface xdg_top_level_v6_interface;

  // |WaylandBindingHandler|
  void SetSize(Vec2<int> size) {
    WAFFLE_LOG(ERROR) << "SetSize is called. (not yet implemented)";
  }

  // |WaylandBindingHandler|
  std::weak_ptr<WaylandBindingHandlerDelegate> InputInterface() {
    return wayland_surface.InputInterface();
  }
};

const struct zxdg_surface_v6_interface
    XdgShellSurface::Impl::xdg_surface_v6_interface {
  .destroy =
      +[](wl_client* client, wl_resource* resource) {
        WAFFLE_LOG(TRACE) << "zxdg_surface_v6_interface.destroy is called.";
        WaylandResource(resource).Destroy();
      },
  .get_toplevel =
      +[](wl_client* client, wl_resource* resource, uint32_t id) {
        WAFFLE_LOG(TRACE)
            << "zxdg_surface_v6_interface.get_toplevel is called.";

        auto impl = WaylandResource(resource).Get<Impl>();
        if (!impl) {
          WAFFLE_LOG(ERROR) << "Client has no target implementation.";
          return;
        }

        if (impl->xdg_top_level_resource.IsValid()) {
          WAFFLE_LOG(WARNING) << "Resouce is already created.";
        } else {
          impl->xdg_top_level_resource.Create(impl, client, id,
                                              &zxdg_toplevel_v6_interface, 1,
                                              &xdg_top_level_v6_interface);
        }

        WaylandArray<zxdg_toplevel_v6_state> states;
        states.Append(ZXDG_TOPLEVEL_V6_STATE_ACTIVATED);

        zxdg_toplevel_v6_send_configure(impl->xdg_top_level_resource.Resource(),
                                        0, 0, states.Array());
        if (!impl->xdg_surface_resource.IsValid()) {
          WAFFLE_LOG(ERROR) << "Resource is invalid.";
          return;
        }
        zxdg_surface_v6_send_configure(impl->xdg_surface_resource.Resource(),
                                       WaylandServer::SerialNumber());
      },
  .get_popup =
      +[](wl_client* client,
          wl_resource* resource,
          uint32_t id,
          wl_resource* parent,
          wl_resource* positioner) {
        WAFFLE_LOG(TRACE) << "zxdg_surface_v6_interface.get_popup is called "
                             "(not yet implemented)";
      },
  .set_window_geometry =
      +[](wl_client* client,
          wl_resource* resource,
          int32_t x,
          int32_t y,
          int32_t width,
          int32_t height) {
        WAFFLE_LOG(TRACE) << "zxdg_surface_v6_interface.set_window_geometry "
                             "is called (not yet implemented)";
      },
  .ack_configure =
      +[](wl_client* client, wl_resource* resource, uint32_t serial) {
        WAFFLE_LOG(TRACE) << "zxdg_surface_v6_interface.ack_configure is "
                             "called (not yet implemented)";
      }
};

const struct zxdg_toplevel_v6_interface
    XdgShellSurface::Impl::xdg_top_level_v6_interface {
  .destroy =
      +[](wl_client* client, wl_resource* resource) {
        WAFFLE_LOG(TRACE) << "zxdg_toplevel_v6_interface.destroy is called";
        WaylandResource(resource).Destroy();
      },
  .set_parent =
      +[](wl_client* client, wl_resource* resource, wl_resource* parent) {
        WAFFLE_LOG(TRACE) << "zxdg_toplevel_v6_interface.set_parent is called "
                             "(not yet implemented)";
      },
  .set_title =
      +[](wl_client* client, wl_resource* resource, const char* title) {
        WAFFLE_LOG(TRACE) << "zxdg_toplevel_v6_interface.set_title is called "
                             "(not yet implemented)";
      },
  .set_app_id =
      +[](wl_client* client, wl_resource* resource, const char* app_id) {
        WAFFLE_LOG(TRACE) << "zxdg_toplevel_v6_interface.set_app_id is called "
                             "(not yet implemented)";
      },
  .show_window_menu =
      +[](wl_client* client,
          wl_resource* resource,
          wl_resource* seat,
          uint32_t serial,
          int32_t x,
          int32_t y) {
        WAFFLE_LOG(TRACE) << "zxdg_toplevel_v6_interface.show_window_menu "
                             "is called (not yet implemented)";
      },
  .move =
      +[](wl_client* client,
          wl_resource* resource,
          wl_resource* seat,
          uint32_t serial) {
        WAFFLE_LOG(TRACE) << "zxdg_toplevel_v6_interface.move is called (not "
                             "yet implemented)";
      },
  .resize =
      +[](wl_client* client,
          wl_resource* resource,
          wl_resource* seat,
          uint32_t serial,
          uint32_t edges) {
        WAFFLE_LOG(TRACE) << "zxdg_toplevel_v6_interface.resize is called (not "
                             "yet implemented)";
      },
  .set_max_size =
      +[](wl_client* client,
          wl_resource* resource,
          int32_t width,
          int32_t height) {
        WAFFLE_LOG(TRACE)
            << "zxdg_toplevel_v6_interface.set_max_size is called "
               "(not yet implemented)";
      },
  .set_min_size =
      +[](wl_client* client,
          wl_resource* resource,
          int32_t width,
          int32_t height) {
        WAFFLE_LOG(TRACE)
            << "zxdg_toplevel_v6_interface.set_min_size is called "
               "(not yet implemented)";
      },
  .set_maximized =
      +[](wl_client* client, wl_resource* resource) {
        WAFFLE_LOG(TRACE) << "zxdg_toplevel_v6_interface.set_maximized "
                             "is called (not yet implemented)";
      },
  .unset_maximized =
      +[](wl_client* client, wl_resource* resource) {
        WAFFLE_LOG(TRACE) << "zxdg_toplevel_v6_interface.unset_maximized "
                             "is called (not yet implemented)";
      },
  .set_fullscreen =
      +[](wl_client* client, wl_resource* resource, wl_resource* output) {
        WAFFLE_LOG(TRACE) << "zxdg_toplevel_v6_interface.set_fullscreen "
                             "is called (not yet implemented)";
      },
  .unset_fullscreen =
      +[](wl_client* client, wl_resource* resource) {
        WAFFLE_LOG(TRACE) << "zxdg_toplevel_v6_interface.unset_fullscreen "
                             "is called (not yet implemented)";
      },
  .set_minimized = +[](wl_client* client, wl_resource* resource) {
    WAFFLE_LOG(TRACE) << "zxdg_toplevel_v6_interface.set_minimized is called "
                         "(not yet implemented)";
  },
};

XdgShellSurface::XdgShellSurface(wl_client* client,
                                 uint32_t id,
                                 uint version,
                                 WaylandSurface surface) {
  WAFFLE_LOG(TRACE) << "Creating XdgShellSurface...";

  auto impl = std::make_shared<Impl>();
  waffle::Compositor::Instance()->AddWindow(impl);

  impl->wayland_surface = surface;
  impl->SetTexture(surface.GetTexture());
  impl->xdg_surface_resource.Create(impl, client, id,
                                    &zxdg_surface_v6_interface, version,
                                    &Impl::xdg_surface_v6_interface);
  impl_ = impl;
}

}  // namespace waffle
