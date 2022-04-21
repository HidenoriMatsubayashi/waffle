// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Copyright 2021 Sony Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "waffle/backend/surface/linux_egl_surface.h"

#include "waffle/backend/surface/egl_utils.h"
#include "waffle/logger.h"

namespace waffle {

LinuxEGLSurface::LinuxEGLSurface(EGLSurface surface,
                                 EGLDisplay display,
                                 EGLContext context)
    : surface_(surface), display_(display), context_(context){};

LinuxEGLSurface::~LinuxEGLSurface() {
  if (surface_ != EGL_NO_SURFACE) {
    if (eglDestroySurface(display_, surface_) != EGL_TRUE) {
      WAFFLE_LOG(ERROR) << "Failed to destory surface: "
                        << get_egl_error_cause();
    }
    surface_ = EGL_NO_SURFACE;
  }
}

bool LinuxEGLSurface::IsValid() const {
  return surface_ != EGL_NO_SURFACE;
}

bool LinuxEGLSurface::MakeCurrent() const {
  if (eglMakeCurrent(display_, surface_, surface_, context_) != EGL_TRUE) {
    WAFFLE_LOG(ERROR) << "Failed to make the EGL context current: "
                      << get_egl_error_cause();
    return false;
  }

#if defined(ENABLE_EGL_ASYNC_BUFFER_SWAPPING)
  // Non-blocking when swappipping buffers on Wayland.
  // However, we might encounter rendering problems on some Wayland compositors
  // (e.g. weston 9.0) when we use them.
  if (eglSwapInterval(display_, 0) != EGL_TRUE) {
    WAFFLE_LOG(ERROR) << "Failed to eglSwapInterval(Free): "
                      << get_egl_error_cause();
  }
#endif

  return true;
}

bool LinuxEGLSurface::SwapBuffers() const {
  if (eglSwapBuffers(display_, surface_) != EGL_TRUE) {
    WAFFLE_LOG(ERROR) << "Failed to swap the EGL buffer: "
                      << get_egl_error_cause();
    return false;
  }
  return true;
}

}  // namespace waffle
