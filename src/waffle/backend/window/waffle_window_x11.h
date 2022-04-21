// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Copyright 2021 Sony Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WAFFLE_BACKEND_WINDOW_WAFFLE_WINDOW_X11_H_
#define WAFFLE_BACKEND_WINDOW_WAFFLE_WINDOW_X11_H_

#include <memory>

#include "waffle/backend/surface/surface_gl.h"
#include "waffle/backend/window/native_window_x11.h"
#include "waffle/backend/window/waffle_window.h"
#include "waffle/backend/window/window_binding_handler.h"

namespace waffle {

class WaffleWindowX11 : public WaffleWindow {
 public:
  WaffleWindowX11(wl_display* wl_display,
                  WaffleWindowProperties view_properties);
  ~WaffleWindowX11();

  // |WaffleWindow|
  bool IsValid() const override;

  // |WindowBindingHandler|
  bool DispatchEvent() override;

  // |WindowBindingHandler|
  bool CreateRenderSurface(int32_t width, int32_t height) override;

  // |WindowBindingHandler|
  void DestroyRenderSurface() override;

  // |WindowBindingHandler|
  std::string GetClipboardData() override;

  // |WindowBindingHandler|
  void SetClipboardData(const std::string& data) override;

 private:
  // Handles the events of the mouse button.
  void HandlePointerButtonEvent(uint32_t button,
                                bool button_pressed,
                                int16_t x,
                                int16_t y);

  Display* display_ = nullptr;
  std::unique_ptr<NativeWindowX11> native_window_;
};

}  // namespace waffle

#endif  // WAFFLE_BACKEND_WINDOW_WAFFLE_WINDOW_X11_H_
