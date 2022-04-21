// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Copyright 2021 Sony Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WAFFLE_BACKEND_SURFACE_SURFACE_BASE_H_
#define WAFFLE_BACKEND_SURFACE_SURFACE_BASE_H_

#include <wayland-server.h>

#include <memory>

#include "waffle/backend/surface/context_egl.h"
#include "waffle/backend/surface/linux_egl_surface.h"
#include "waffle/backend/window/native_window.h"
#include "waffle/renderer/texture.h"

namespace waffle {

class SurfaceBase {
 public:
  SurfaceBase() = default;
  virtual ~SurfaceBase() = default;

  // Shows a surface is valid or not.
  bool IsValid() const;

  // Sets a netive platform's window.
  bool SetNativeWindow(NativeWindow* window);

  // Changes an on-screen surface size.
  // On-screen surface needs to be recreated after window size changed only when
  // using DRM-GBM backend. Because gbm-surface is recreated when the window
  // size changed.
  bool OnScreenSurfaceResize(const size_t width, const size_t height);

  // Clears current on-screen context.
  bool ClearCurrentContext() const;

  // Clears and destroys current ons-screen context.
  void DestroyOnScreenContext();

  // Makes an off-screen resource context.
  bool ResourceContextMakeCurrent() const;

  //
  void LoadIntoTexture(wl_resource* buffer, Texture& texture) const;

  //
  void BindWlDisplay(wl_display* display) const;

  //
  void UnbindWlDisplay(wl_display* display) const;

 protected:
  std::unique_ptr<ContextEgl> context_;
  NativeWindow* native_window_ = nullptr;
  std::unique_ptr<LinuxEGLSurface> onscreen_surface_ = nullptr;
  std::unique_ptr<LinuxEGLSurface> offscreen_surface_ = nullptr;
};

}  // namespace waffle

#endif  // WAFFLE_BACKEND_SURFACE_SURFACE_BASE_H_
