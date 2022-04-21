// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Copyright 2021 Sony Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WAFFLE_BACKEND_SURFACE_ENVIRONMENT_EGL_H_
#define WAFFLE_BACKEND_SURFACE_ENVIRONMENT_EGL_H_

#include <EGL/egl.h>
#include <EGL/eglext.h>

#include "waffle/backend/surface/egl_utils.h"
#include "waffle/logger.h"

namespace waffle {

class EnvironmentEgl {
 public:
  EnvironmentEgl(EGLNativeDisplayType platform_display,
                 bool sub_environment = false)
      : display_(EGL_NO_DISPLAY), sub_environment_(sub_environment) {
#if defined(DISPLAY_BACKEND_IS_X11)
    auto get_platform_display =
        reinterpret_cast<PFNEGLGETPLATFORMDISPLAYEXTPROC>(
            eglGetProcAddress("eglGetPlatformDisplayEXT"));
    if (!get_platform_display) {
      WAFFLE_LOG(ERROR) << "Failed to get eglGetProcAddress: "
                        << get_egl_error_cause();
      return;
    }
    display_ =
        get_platform_display(EGL_PLATFORM_X11_KHR, platform_display, NULL);
#else
    display_ = eglGetDisplay(platform_display);
#endif

    if (display_ == EGL_NO_DISPLAY) {
      WAFFLE_LOG(ERROR) << "Failed to get the EGL display: "
                        << get_egl_error_cause();
      return;
    }

    // sub_environment flag is used for window decorations such as toolbar and
    // buttons. When this flag is active, EGLDisplay doesn't be initialized and
    // finalized.
    if (!sub_environment_) {
      valid_ = InitializeEgl();
    } else {
      valid_ = true;
    }
  }

  EnvironmentEgl(bool sub_environment = false)
      : display_(EGL_NO_DISPLAY), sub_environment_(sub_environment) {}

  ~EnvironmentEgl() {
    if (display_ != EGL_NO_DISPLAY) {
      if (eglTerminate(display_) != EGL_TRUE) {
        WAFFLE_LOG(ERROR) << "Failed to terminate the EGL display: "
                          << get_egl_error_cause();
      }
      display_ = EGL_NO_DISPLAY;
    }
  }

  bool InitializeEgl() const {
    if (eglInitialize(display_, nullptr, nullptr) != EGL_TRUE) {
      WAFFLE_LOG(ERROR) << "Failed to initialize the EGL display: "
                        << get_egl_error_cause();
      return false;
    }

    if (eglBindAPI(EGL_OPENGL_ES_API) != EGL_TRUE) {
      WAFFLE_LOG(ERROR) << "Failed to bind EGL API: " << get_egl_error_cause();
      return false;
    }

    return true;
  }

  bool IsValid() const { return valid_; }

  EGLDisplay Display() const { return display_; };

 private:
  EGLDisplay display_;
  bool valid_ = false;
  bool sub_environment_;
};

}  // namespace waffle

#endif  // WAFFLE_BACKEND_SURFACE_ENVIRONMENT_EGL_H_
