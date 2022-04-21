// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WAFFLE_WAYLAND_WAYLAND_BINDING_HANDLER_DELEGATE_H_
#define WAFFLE_WAYLAND_WAYLAND_BINDING_HANDLER_DELEGATE_H_

#include <iostream>

#include "waffle/utils/vec2.h"

namespace waffle {

class WaylandBindingHandlerDelegate {
 public:
  virtual void OnPointerMove(Vec2<double> pos) = 0;
  virtual void OnPointerLeave() = 0;
  virtual void OnPointerClick(uint32_t button, bool down) = 0;
  virtual void OnKey(uint32_t key, bool down) = 0;
};

};  // namespace waffle

#endif  // WAFFLE_WAYLAND_WAYLAND_BINDING_HANDLER_DELEGATE_H_
