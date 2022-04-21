// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Copyright 2021 Sony Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "waffle/backend/surface/surface_gl.h"

namespace waffle {

SurfaceGl::SurfaceGl(std::unique_ptr<ContextEgl> context) {
  context_ = std::move(context);
}

bool SurfaceGl::GLContextMakeCurrent() const {
  return onscreen_surface_->MakeCurrent();
}

bool SurfaceGl::GLContextClearCurrent() const {
  return context_->ClearCurrent();
}

bool SurfaceGl::GLContextPresent(uint32_t fbo_id) const {
  if (!onscreen_surface_->SwapBuffers()) {
    return false;
  }
  native_window_->SwapBuffers();
  return true;
}

uint32_t SurfaceGl::GLContextFBO() const {
  return 0;
}

void* SurfaceGl::GlProcResolver(const char* name) const {
  return context_->GlProcResolver(name);
}

}  // namespace waffle
