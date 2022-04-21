// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WAFFLE_WAYLAND_WAYLAND_SURFACE_H_
#define WAFFLE_WAYLAND_WAYLAND_SURFACE_H_

#include <wayland-server.h>

#include <chrono>

#include "waffle/renderer/texture.h"
#include "waffle/wayland/wayland_binding_handler_delegate.h"
#include "waffle/wayland/wayland_resource.h"

namespace waffle {

class WaylandSurface {
 public:
  WaylandSurface() = default;
  WaylandSurface(wl_client* client, uint32_t id, int32_t version);
  ~WaylandSurface() = default;

  std::weak_ptr<WaylandBindingHandlerDelegate> InputInterface();

  Texture GetTexture();

  static WaylandSurface GetSurfaceFrom(WaylandResource resource);

  static void HandleFrameCallbacks();

 private:
  static int TimeSinceProgramStartMillisecond();

  struct Impl;
  std::weak_ptr<Impl> impl_;

  static std::chrono::high_resolution_clock::time_point program_start_time_;
};

}  // namespace waffle

#endif  // WAFFLE_WAYLAND_WAYLAND_SURFACE_H_
