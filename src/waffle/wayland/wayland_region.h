// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WAFFLE_WAYLAND_WAYLAND_REGION_H_
#define WAFFLE_WAYLAND_WAYLAND_REGION_H_

#include <wayland-server-core.h>

#include <memory>

namespace waffle {

class WaylandRegion {
 public:
  WaylandRegion(wl_client* client, uint32_t id, uint version);

 private:
  struct Impl;
  std::weak_ptr<Impl> impl_;
};

}  // namespace waffle

#endif  // WAFFLE_WAYLAND_WAYLAND_REGION_H_
