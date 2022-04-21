// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Copyright 2021 Sony Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WAFFLE_BACKEND_SURFACE_SURFACE_GL_H_
#define WAFFLE_BACKEND_SURFACE_SURFACE_GL_H_

#include <memory>

#include "waffle/backend/surface/context_egl.h"
#include "waffle/backend/surface/surface_base.h"
#include "waffle/backend/surface/surface_gl_delegate.h"

namespace waffle {

class SurfaceGl final : public SurfaceBase, public SurfaceGlDelegate {
 public:
  SurfaceGl(std::unique_ptr<ContextEgl> context);
  ~SurfaceGl() = default;

  // |SurfaceGlDelegate|
  bool GLContextMakeCurrent() const override;

  // |SurfaceGlDelegate|
  bool GLContextClearCurrent() const override;

  // |SurfaceGlDelegate|
  bool GLContextPresent(uint32_t fbo_id) const override;

  // |SurfaceGlDelegate|
  uint32_t GLContextFBO() const override;

  // |SurfaceGlDelegate|
  void* GlProcResolver(const char* name) const override;
};

}  // namespace waffle

#endif  // WAFFLE_BACKEND_SURFACE_SURFACE_GL_H_
