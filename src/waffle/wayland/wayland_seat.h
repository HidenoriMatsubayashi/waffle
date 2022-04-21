// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WAFFLE_WAYLAND_WAYLAND_SEAT_H_
#define WAFFLE_WAYLAND_WAYLAND_SEAT_H_

#include <wayland-server-core.h>

#include <memory>

#include "waffle/utils/vec2.h"
#include "waffle/wayland/wayland_resource.h"

namespace waffle {

constexpr uint kWlSeatMaxVersion = 6;

class WlSeat {
 public:
  WlSeat() = default;
  WlSeat(wl_client* client, uint32_t id, uint version);
  ~WlSeat() = default;

  static void OnPointerMove(Vec2<double> pos, WaylandResource surface);

  static void OnPointerLeave(WaylandResource surface);

  static void OnPointerClick(uint32_t button,
                             bool down,
                             WaylandResource surface);

  static void OnKey(uint32_t key, bool down, WaylandResource surface);

 private:
  struct Impl;
  std::weak_ptr<Impl> impl_;

  static WlSeat GetFromClient(wl_client* client);
  static std::shared_ptr<Impl> GetImplFromSurface(WaylandResource surface);
};

}  // namespace waffle

#endif  // WAYLAND_COMPOSITOR_WAYLAND_SEAT_H_