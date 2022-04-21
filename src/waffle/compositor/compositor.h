// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WAFFLE_COMPOSITOR_COMPOSITOR_COMPOSITOR_H_
#define WAFFLE_COMPOSITOR_COMPOSITOR_COMPOSITOR_H_

#include <cassert>
#include <vector>

#include "waffle/backend/backend.h"
#include "waffle/renderer/window_renderer.h"
#include "waffle/utils/vec2.h"
#include "waffle/wayland/wayland_binding_handler.h"

namespace waffle {

class Compositor : public WindowBindingHandlerDelegate {
 public:
  struct Window {
    std::weak_ptr<WaylandBindingHandler> interface;
    Vec2<int> pos = Vec2<int>();
  };

  Compositor(wl_display* wl_display, WaffleWindowProperties view_properties);

  static void Create(wl_display* wl_display,
                     WaffleWindowProperties view_properties) {
    instance_ = new Compositor(wl_display, view_properties);
  }

  static Compositor* Instance() {
    assert(instance_ != nullptr);
    return instance_;
  }

  static void Destroy() {
    if (instance_) {
      delete instance_;
      instance_ = nullptr;
    }
  }

  void LoadIntoTexture(wl_resource* buffer, Texture& texture) const {
    backend_->LoadIntoTexture(buffer, texture);
  }

  bool HandleEvent();

  void AddWindow(std::weak_ptr<WaylandBindingHandler> window);

  void SetCursor(Texture texture);

  void ClearCursor();

  void Draw();

  int32_t GetFrameRate() const { return backend_->GetFrameRate(); }

  // |WindowBindingHandlerDelegate|
  void OnWindowSizeChanged(size_t width, size_t height) const override;

  // |WindowBindingHandlerDelegate|
  void OnPointerMove(double x, double y) override;

  // |WindowBindingHandlerDelegate|
  void OnPointerButton(double x,
                       double y,
                       uint32_t button,
                       bool pressed) override;

  // |WindowBindingHandlerDelegate|
  void OnPointerLeave() override;

  // |WindowBindingHandlerDelegate|
  void OnTouchDown(uint32_t time, int32_t id, double x, double y) override;

  // |WindowBindingHandlerDelegate|
  void OnTouchUp(uint32_t time, int32_t id) override;

  // |WindowBindingHandlerDelegate|
  void OnTouchMotion(uint32_t time, int32_t id, double x, double y) override;

  // |WindowBindingHandlerDelegate|
  void OnTouchCancel() override;

  // |WindowBindingHandlerDelegate|
  void OnKeyMap(uint32_t format, int fd, uint32_t size) override;

  // |WindowBindingHandlerDelegate|
  void OnKeyModifiers(uint32_t mods_depressed,
                      uint32_t mods_latched,
                      uint32_t mods_locked,
                      uint32_t group) override;

  // |WindowBindingHandlerDelegate|
  void OnKey(uint32_t key, uint32_t state) override;

  // |WindowBindingHandlerDelegate|
  void OnScroll(double x,
                double y,
                double delta_x,
                double delta_y,
                int scroll_offset_multiplier) override;

 protected:
  Compositor();
  ~Compositor() = default;

  static Compositor* instance_;

 private:
  Compositor::Window ActiveWindow();

  std::unique_ptr<Backend> backend_;
  std::vector<Compositor::Window> windows_;
  WindowRenderer renderer_;
  WindowRenderer bg_renderer_;
  Texture bg_texture_;
  Texture cursor_texture_;
  Vec2<double> cursor_pos_;
};

};  // namespace waffle

#endif  // WAFFLE_COMPOSITOR_COMPOSITOR_COMPOSITOR_H_
