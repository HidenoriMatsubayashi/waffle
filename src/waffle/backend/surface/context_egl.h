// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Copyright 2021 Sony Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WAFFLE_BACKEND_SURFACE_CONTEXT_EGL_H_
#define WAFFLE_BACKEND_SURFACE_CONTEXT_EGL_H_

#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <memory>

#include "waffle/backend/surface/environment_egl.h"
#include "waffle/backend/surface/linux_egl_surface.h"
#include "waffle/backend/window/native_window.h"
#include "waffle/renderer/texture.h"

namespace waffle {

class ContextEgl {
 public:
  ContextEgl(std::unique_ptr<EnvironmentEgl> environment,
             EGLint egl_surface_type = EGL_WINDOW_BIT);
  ~ContextEgl() = default;

  virtual std::unique_ptr<LinuxEGLSurface> CreateOnscreenSurface(
      NativeWindow* window) const;

  std::unique_ptr<LinuxEGLSurface> CreateOffscreenSurface(
      NativeWindow* window_resource) const;

  bool IsValid() const;

  bool ClearCurrent() const;

  void* GlProcResolver(const char* name) const;

  EGLint GetAttrib(EGLint attribute);

  bool BindWlDisplay(wl_display* display);

  bool UnbindWlDisplay(wl_display* display);

  void LoadIntoTexture(wl_resource* buffer, Texture& texture);

 protected:
  std::unique_ptr<EnvironmentEgl> environment_;
  EGLConfig config_;
  EGLContext context_;
  EGLContext resource_context_;
  bool valid_;

  PFNEGLBINDWAYLANDDISPLAYWL eglBindWaylandDisplayWL_ = nullptr;
  PFNEGLUNBINDWAYLANDDISPLAYWL eglUnbindWaylandDisplayWL_ = nullptr;
  PFNEGLQUERYWAYLANDBUFFERWL eglQueryWaylandBufferWL_ = nullptr;
  PFNEGLCREATEIMAGEKHRPROC eglCreateImageKHR_ = nullptr;
};

}  // namespace waffle

#endif  // WAFFLE_BACKEND_SURFACE_CONTEXT_EGL_H_
