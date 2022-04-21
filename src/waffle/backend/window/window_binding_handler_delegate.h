// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WAFFLE_BACKEND_WINDOW_BINDING_HANDLER_DELEGATE_H_
#define WAFFLE_BACKEND_WINDOW_BINDING_HANDLER_DELEGATE_H_

#include <iostream>

namespace waffle {

class WindowBindingHandlerDelegate {
 public:
  virtual void OnWindowSizeChanged(size_t width, size_t height) const = 0;
  virtual void OnPointerMove(double x, double y) = 0;
  virtual void OnPointerLeave() = 0;
  virtual void OnPointerButton(double x,
                               double y,
                               uint32_t button,
                               bool pressed) = 0;
  virtual void OnTouchDown(uint32_t time, int32_t id, double x, double y) = 0;
  virtual void OnTouchUp(uint32_t time, int32_t id) = 0;
  virtual void OnTouchMotion(uint32_t time, int32_t id, double x, double y) = 0;
  virtual void OnTouchCancel() = 0;
  virtual void OnKeyMap(uint32_t format, int fd, uint32_t size) = 0;
  virtual void OnKeyModifiers(uint32_t mods_depressed,
                              uint32_t mods_latched,
                              uint32_t mods_locked,
                              uint32_t group) = 0;
  virtual void OnKey(uint32_t key, uint32_t state) = 0;
  virtual void OnScroll(double x,
                        double y,
                        double delta_x,
                        double delta_y,
                        int scroll_offset_multiplier) = 0;
};

}  // namespace waffle

#endif  // WAFFLE_BACKEND_WINDOW_BINDING_HANDLER_DELEGATE_H_
