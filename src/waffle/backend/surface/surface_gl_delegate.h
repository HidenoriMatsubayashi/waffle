// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Copyright 2021 Sony Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WAFFLE_BACKEND_SURFACE_SURFACE_GL_DELEGATE_H_
#define WAFFLE_BACKEND_SURFACE_SURFACE_GL_DELEGATE_H_

#include <cstdint>

namespace waffle {

class SurfaceGlDelegate {
 public:
  virtual bool GLContextMakeCurrent() const = 0;

  virtual bool GLContextClearCurrent() const = 0;

  virtual bool GLContextPresent(uint32_t fbo_id) const = 0;

  virtual uint32_t GLContextFBO() const = 0;

  virtual void* GlProcResolver(const char* name) const = 0;
};

}  // namespace waffle

#endif  // WAFFLE_BACKEND_SURFACE_SURFACE_GL_DELEGATE_H_
