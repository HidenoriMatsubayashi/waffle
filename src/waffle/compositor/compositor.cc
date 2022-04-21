// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "waffle/compositor/compositor.h"

#include <cassert>
#include <memory>
#include <vector>

#include <EGL/egl.h>
#include <GLES3/gl32.h>

namespace waffle {

namespace {

struct GlProcs {
  PFNGLVIEWPORTPROC glViewport;
  bool valid;
};

static const GlProcs& GlProcs() {
  static struct GlProcs procs = {};
  static bool initialized = false;
  if (!initialized) {
    procs.glViewport =
        reinterpret_cast<PFNGLVIEWPORTPROC>(eglGetProcAddress("glViewport"));
    procs.valid = procs.glViewport;
    if (!procs.valid) {
      WAFFLE_LOG(ERROR) << "Failed to load GlProcs";
    }
    initialized = true;
  }
  return procs;
}

}  // namespace

Compositor* Compositor::instance_ = nullptr;

Compositor::Compositor(wl_display* wl_display,
                       WaffleWindowProperties view_properties) {
  backend_ = std::make_unique<Backend>(wl_display, view_properties);
  backend_->SetWindowBindingHandler(this);

  renderer_.Init();
  bg_renderer_.Init();

  bg_texture_ = Texture();
  bg_texture_.LoadFileImage(view_properties.background_image_filepath);
}

bool Compositor::HandleEvent() {
  if (!backend_ || !backend_->IsValid()) {
    return false;
  }
  return backend_->DispatchEvent();
}

Compositor::Window Compositor::ActiveWindow() {
  for (auto w : windows_) {
    if (!w.interface.expired()) {
      return w;
    }
  }
  return Window();
}

void Compositor::AddWindow(std::weak_ptr<WaylandBindingHandler> window) {
  Window data;
  data.interface = window;
  data.pos = Vec2<int>(0, 0);
  windows_.push_back(data);
}

void Compositor::SetCursor(Texture texture) {
  cursor_texture_ = texture;
}

void Compositor::ClearCursor() {
  cursor_texture_ = Texture();
  cursor_pos_ = Vec2<double>();
}

void Compositor::Draw() {
  // todo: support different window size.
  constexpr double kWidth = 1920;
  constexpr double kHeight = 1024;

  bg_renderer_.Draw(bg_texture_, Vec2<int>(0, 0), Vec2<double>(1, 1));

  for (auto window : windows_) {
    auto interface = window.interface.lock();
    if (interface && interface->GetTexture().Valid()) {
      auto texture = interface->GetTexture();
      auto texture_size = texture.Size();
      auto size =
          Vec2<double>(texture_size.X() / kWidth, texture_size.Y() / kHeight);
      renderer_.Draw(texture, window.pos, size);
    }
  }

  // todo: support cursor.
#if 0
  if (cursor_texture_.Valid()) {
    renderer_.Draw(cursor_texture_, cursor_pos_, Vec2<double>(0.03, 0.03));
  }
#endif

  backend_->SwapBuffer();
}

void Compositor::OnWindowSizeChanged(size_t width, size_t height) const {
  const auto& gl = GlProcs();
  if (!gl.valid) {
    return;
  }
  gl.glViewport(0, 0, width, height);
}

void Compositor::OnPointerMove(double x, double y) {
  auto window = ActiveWindow();
  if (auto interface = window.interface.lock()) {
    auto input = interface->InputInterface().lock();
    if (!input) {
      return;
    }

    Vec2<double> pos(x, y);
    cursor_pos_ = pos;
    auto transformed =
        Vec2<double>((pos.X() - window.pos.X()),   /// window.size.X(),
                     (pos.Y() - window.pos.Y()));  // / window.size.Y());
    input->OnPointerMove(transformed);
  }
}

void Compositor::OnPointerButton(double x,
                                 double y,
                                 uint32_t button,
                                 bool pressed) {
  auto window = ActiveWindow();
  if (auto interface = window.interface.lock()) {
    auto input = interface->InputInterface().lock();
    if (!input) {
      return;
    }

    input->OnPointerClick(button, pressed);
  }
}

void Compositor::OnPointerLeave() {
  auto window = ActiveWindow();
  if (auto interface = window.interface.lock()) {
    auto input = interface->InputInterface().lock();
    if (!input) {
      return;
    }

    input->OnPointerLeave();
    ClearCursor();
  }
}

void Compositor::OnTouchDown(uint32_t time, int32_t id, double x, double y) {}

void Compositor::OnTouchUp(uint32_t time, int32_t id) {}

void Compositor::OnTouchMotion(uint32_t time, int32_t id, double x, double y) {}

void Compositor::OnTouchCancel() {}

void Compositor::OnKeyMap(uint32_t format, int fd, uint32_t size) {}

void Compositor::OnKeyModifiers(uint32_t mods_depressed,
                                uint32_t mods_latched,
                                uint32_t mods_locked,
                                uint32_t group) {}

void Compositor::OnKey(uint32_t key, uint32_t state) {
  auto window = ActiveWindow();
  if (auto interface = window.interface.lock()) {
    auto input = interface->InputInterface().lock();
    if (!input) {
      return;
    }
    input->OnKey(key, state);
  }
}

void Compositor::OnScroll(double x,
                          double y,
                          double delta_x,
                          double delta_y,
                          int scroll_offset_multiplier) {}

};  // namespace waffle
