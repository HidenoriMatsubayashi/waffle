// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WAFFLE_BACKEND_WINDOW_WAFFLE_WINDOW_H_
#define WAFFLE_BACKEND_WINDOW_WAFFLE_WINDOW_H_

#include <cstdint>
#include <string>

#include <EGL/egl.h>
#include <EGL/eglext.h>

#include "waffle/backend/surface/surface_gl.h"
#include "waffle/backend/window/window_binding_handler.h"

namespace waffle {

class WaffleWindow : public WindowBindingHandler {
 public:
  WaffleWindow() = default;
  virtual ~WaffleWindow() = default;

  virtual bool IsValid() const = 0;

  void SetRotation(WaffleViewRotation rotation) {
    if (rotation == WaffleViewRotation::kRotation_90) {
      current_rotation_ = 90;
    } else if (rotation == WaffleViewRotation::kRotation_180) {
      current_rotation_ = 180;
    } else if (rotation == WaffleViewRotation::kRotation_270) {
      current_rotation_ = 270;
    } else {
      current_rotation_ = 0;
    }
  }

  void LoadIntoTexture(wl_resource* buffer, Texture& texture) const {
    render_surface_->LoadIntoTexture(buffer, texture);
  }

  // |WindowBindingHandler|
  void SetWindowBindingHandler(WindowBindingHandlerDelegate* window) override {
    binding_handler_delegate_ = window;
  }

  // |WindowBindingHandler|
  SurfaceGl* GetRenderSurfaceTarget() const override {
    return render_surface_.get();
  }

  // |WindowBindingHandler|
  uint16_t GetRotationDegree() const override { return current_rotation_; }

  // |WindowBindingHandler|
  double GetDpiScale() override { return current_scale_; }

  // |WindowBindingHandler|
  WafflePhysicalWindowBounds GetPhysicalWindowBounds() const override {
    return {GetCurrentWidth(), GetCurrentHeight()};
  }

  // |WindowBindingHandler|
  int32_t GetFrameRate() const override { return current_fps_; }

 protected:
  uint32_t GetCurrentWidth() const { return window_properties_.width; }
  uint32_t GetCurrentHeight() const { return window_properties_.height; }

  WindowBindingHandlerDelegate* binding_handler_delegate_ = nullptr;
  WaffleWindowProperties window_properties_;
  uint16_t current_rotation_ = 0;
  double pointer_x_ = 0;
  double pointer_y_ = 0;
  int32_t current_fps_ = 60000;
  double current_scale_ = 1.0;
  std::string clipboard_data_ = "";
  std::unique_ptr<SurfaceGl> render_surface_;
  wl_display* wl_display_;
  bool display_valid_;
};

}  // namespace waffle

#endif  // WAFFLE_BACKEND_WINDOW_WAFFLE_WINDOW_H_
