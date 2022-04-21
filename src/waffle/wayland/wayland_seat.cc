// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "waffle/wayland/wayland_seat.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <wayland/protocols/wayland-server-protocol.h>

#include <cassert>
#include <string>
#include <unordered_map>

#include "waffle/logger.h"
#include "waffle/wayland_server.h"

namespace waffle {

struct WlSeat::Impl : WaylandResource::Data {
  wl_resource* last_resource = nullptr;
  WaylandResource seat;
  WaylandResource pointer;
  WaylandResource keyboard;

  static std::unordered_map<wl_client*, std::weak_ptr<Impl>> umap;

  static const struct wl_pointer_interface kWlPointerInterface;
  static const struct wl_keyboard_interface kWlKeyboardInterface;
  static const struct wl_seat_interface kWlSeatInterface;
};

std::unordered_map<wl_client*, std::weak_ptr<WlSeat::Impl>> WlSeat::Impl::umap;

const struct wl_pointer_interface WlSeat::Impl::kWlPointerInterface {
  .set_cursor =
      +[](wl_client* client,
          wl_resource* resource,
          uint32_t serial,
          wl_resource* surface,
          int32_t hotspot_x,
          int32_t hotspot_y) {
        WAFFLE_LOG(WARNING) << "wl_pointer_interface.set_cursor is called (not "
                               "yet implemented)";
      },
  .release = +[](wl_client* client, wl_resource* resource) {
    WAFFLE_LOG(TRACE) << "wl_pointer_interface.release is called.";
    WaylandResource(resource).Destroy();
  }
};

const struct wl_keyboard_interface WlSeat::Impl::kWlKeyboardInterface {
  .release = +[](wl_client* client, wl_resource* resource) {
    WAFFLE_LOG(TRACE) << "wl_keyboard_interface.release is called.";
    WaylandResource(resource).Destroy();
  }
};

const struct wl_seat_interface WlSeat::Impl::kWlSeatInterface {
  .get_pointer =
      +[](wl_client* client, wl_resource* resource, uint32_t id) {
        WAFFLE_LOG(TRACE) << "wl_seat_interface.get_pointer is called.";
        auto impl = WaylandResource(resource).Get<Impl>();
        if (!impl) {
          WAFFLE_LOG(WARNING) << "Resouce is invalid.";
          return;
        }

        if (impl->pointer.IsValid()) {
          WAFFLE_LOG(WARNING) << "Resouce is already created.";
          return;
        }

        impl->pointer.Create(impl, client, id, &wl_pointer_interface,
                             wl_resource_get_version(resource),
                             &kWlPointerInterface);
      },
  .get_keyboard =
      +[](wl_client* client, wl_resource* resource, uint32_t id) {
        WAFFLE_LOG(TRACE) << "wl_seat_interface.get_keyboard is called.";
        auto impl = WaylandResource(resource).Get<Impl>();
        if (!impl) {
          WAFFLE_LOG(WARNING) << "Resouce is invalid.";
          return;
        }

        if (impl->pointer.IsValid()) {
          WAFFLE_LOG(WARNING) << "Resouce is already created.";
          return;
        }

        impl->keyboard.Create(impl, client, id, &wl_keyboard_interface,
                              wl_resource_get_version(resource),
                              &kWlKeyboardInterface);
        auto keyboard = impl->keyboard;
        // TODO: implement here.
      },
  .get_touch =
      +[](wl_client* client, wl_resource* resource, uint32_t id) {
        WAFFLE_LOG(WARNING)
            << "wl_seat_interface.get_touch is called (not yet implemented)";
      },
  .release = +[](wl_client* client, wl_resource* resource) {
    WAFFLE_LOG(TRACE) << "wl_seat_interface.release is called.";
    WaylandResource(resource).Destroy();
  }
};

WlSeat::WlSeat(wl_client* client, uint32_t id, uint version) {
  WAFFLE_LOG(TRACE) << "Creating WlSeat ...";

  assert(version <= kWlSeatMaxVersion);
  if (Impl::umap.find(client) != Impl::umap.end()) {
    WAFFLE_LOG(WARNING) << "Client made multiple seats.";
  }

  auto impl = std::make_shared<Impl>();
  Impl::umap[client] = impl;
  impl->seat.Create(impl, client, id, &wl_seat_interface, version,
                    &Impl::kWlSeatInterface);
  impl_ = impl;

  wl_seat_send_capabilities(
      impl->seat.Resource(),
      WL_SEAT_CAPABILITY_POINTER | WL_SEAT_CAPABILITY_KEYBOARD);
}

void WlSeat::OnPointerMove(Vec2<double> pos, WaylandResource surface) {
  auto impl = GetImplFromSurface(surface);
  if (!impl || impl->pointer.IsNull()) {
    WAFFLE_LOG(ERROR) << "Client has not created the needed objects";
    return;
  }

  auto pointer = impl->pointer;
  if (impl->last_resource != surface.Resource()) {
    if (pointer.Version() >= WL_POINTER_ENTER_SINCE_VERSION) {
      impl->last_resource = surface.Resource();
      wl_pointer_send_enter(pointer.Resource(), WaylandServer::SerialNumber(),
                            surface.Resource(), wl_fixed_from_double(pos.X()),
                            wl_fixed_from_double(pos.Y()));
    }
  } else {
    if (pointer.Version() >= WL_POINTER_MOTION_SINCE_VERSION) {
      wl_pointer_send_motion(pointer.Resource(), 100 /*TODO*/,
                             wl_fixed_from_double(pos.X()),
                             wl_fixed_from_double(pos.Y()));
    }
  }

  if (pointer.Version() >= WL_POINTER_FRAME_SINCE_VERSION) {
    wl_pointer_send_frame(pointer.Resource());
  }
}

void WlSeat::OnPointerLeave(WaylandResource surface) {
  auto impl = GetImplFromSurface(surface);
  if (!impl || impl->pointer.IsNull()) {
    WAFFLE_LOG(ERROR) << "Client has no target implementation.";
    return;
  }

  auto pointer = impl->pointer;
  impl->last_resource = nullptr;
  if (pointer.Version() >= WL_POINTER_LEAVE_SINCE_VERSION) {
    wl_pointer_send_leave(pointer.Resource(), WaylandServer::SerialNumber(),
                          surface.Resource());
  }

  if (pointer.Version() >= WL_POINTER_FRAME_SINCE_VERSION) {
    wl_pointer_send_frame(pointer.Resource());
  }
}

void WlSeat::OnPointerClick(uint button, bool down, WaylandResource surface) {
  auto impl = GetImplFromSurface(surface);
  if (!impl || impl->pointer.IsNull()) {
    WAFFLE_LOG(ERROR) << "Client has no target implementation.";
    return;
  }

  auto pointer = impl->pointer;
  if (pointer.Version() >= WL_POINTER_BUTTON_SINCE_VERSION) {
    wl_pointer_send_button(pointer.Resource(), WaylandServer::SerialNumber(),
                           100 /*TODO*/, button,
                           down ? WL_POINTER_BUTTON_STATE_PRESSED
                                : WL_POINTER_BUTTON_STATE_RELEASED);
  }

  if (pointer.Version() >= WL_POINTER_FRAME_SINCE_VERSION) {
    wl_pointer_send_frame(pointer.Resource());
  }
}

void WlSeat::OnKey(uint key, bool down, WaylandResource surface) {
  auto impl = GetImplFromSurface(surface);
  if (!impl || impl->keyboard.IsNull()) {
    WAFFLE_LOG(ERROR) << "Client has no target implementation.";
    return;
  }

  // TODO: implement here.
}

WlSeat WlSeat::GetFromClient(wl_client* client) {
  auto iter = Impl::umap.find(client);
  if (iter == Impl::umap.end()) {
    WAFFLE_LOG(TRACE) << "Client has no seats.";
    return WlSeat();
  }

  WlSeat seat;
  seat.impl_ = iter->second;
  return seat;
}

std::shared_ptr<WlSeat::Impl> WlSeat::GetImplFromSurface(
    WaylandResource surface) {
  if (!surface.IsValid() || !surface.Resource()) {
    return nullptr;
  }

  auto* client = surface.Resource()->client;
  return GetFromClient(client).impl_.lock();
}

}  // namespace waffle
