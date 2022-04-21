// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WAFFLE_WAYLAND_WAYLAND_BINDING_HANDLER_H_
#define WAFFLE_WAYLAND_WAYLAND_BINDING_HANDLER_H_

#include "waffle/renderer/texture.h"
#include "waffle/utils/vec2.h"
#include "waffle/wayland/wayland_binding_handler_delegate.h"

namespace waffle {

class WaylandBindingHandler {
 public:
  virtual void SetSize(Vec2<int> size) = 0;
  virtual std::weak_ptr<WaylandBindingHandlerDelegate> InputInterface() = 0;
  void SetTexture(Texture texture) { texture_ = texture; }
  Texture GetTexture() { return texture_; }

 private:
  Texture texture_;
};

};  // namespace waffle

#endif  // WAFFLE_WAYLAND_WAYLAND_BINDING_HANDLER_H_
