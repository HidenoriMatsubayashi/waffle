// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WAFFLE_WAYLAND_XDG_SHELL_SURFACE_H_
#define WAFFLE_WAYLAND_XDG_SHELL_SURFACE_H_

#include <wayland-server.h>

#include "waffle/wayland/wayland_surface.h"

namespace waffle {

class XdgShellSurface {
 public:
  XdgShellSurface(wl_client* client,
                  uint32_t id,
                  uint version,
                  WaylandSurface surface);
  ~XdgShellSurface() = default;

 private:
  struct Impl;
  std::weak_ptr<Impl> impl_;
};

}  // namespace waffle

#endif  // WAFFLE_WAYLAND_XDG_SHELL_SURFACE_H_
