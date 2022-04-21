// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Copyright 2021 Sony Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WAFFLE_BACKEND_SURFACE_LINUX_EGL_SURFACE_H_
#define WAFFLE_BACKEND_SURFACE_LINUX_EGL_SURFACE_H_

#include <EGL/egl.h>

namespace waffle {

class LinuxEGLSurface {
 public:
  // Note that EGLSurface will be destroyed in this class's destructor.
  LinuxEGLSurface(EGLSurface surface, EGLDisplay display, EGLContext context);
  ~LinuxEGLSurface();

  bool IsValid() const;

  bool MakeCurrent() const;

  bool SwapBuffers() const;

 private:
  EGLDisplay display_;
  EGLSurface surface_;
  EGLContext context_;
};

}  // namespace waffle

#endif  // WAFFLE_BACKEND_SURFACE_LINUX_EGL_SURFACE_H_
