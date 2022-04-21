// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "waffle/backend/backend.h"

#include "waffle/logger.h"

#if defined(DISPLAY_BACKEND_TYPE_DRM_GBM)
#include "waffle/backend/window/waffle_window_drm.h"
#elif defined(DISPLAY_BACKEND_TYPE_DRM_EGLSTREAM)
#error "DRM-EGLStream backend is not supported yet."
#elif defined(DISPLAY_BACKEND_TYPE_X11)
#include "waffle/backend/window/waffle_window_x11.h"
#else
#error "Wayland backend is not supported yet."
#endif

namespace waffle {

Backend::Backend(wl_display* wl_display,
                 WaffleWindowProperties view_properties) {
  backend_window_ =
#if defined(DISPLAY_BACKEND_TYPE_DRM_GBM)
      std::make_unique<WaffleWindowDrm<NativeWindowDrmGbm>>(wl_display,
                                                            view_properties);
#elif defined(DISPLAY_BACKEND_TYPE_DRM_EGLSTREAM)
#error "DRM-EGLStream backend is not supported yet."
#elif defined(DISPLAY_BACKEND_TYPE_X11)
      std::make_unique<WaffleWindowX11>(wl_display, view_properties);
#else
#error "Wayland backend is not supported yet."
#endif
  backend_window_->CreateRenderSurface(view_properties.width,
                                       view_properties.height);
}

Backend::~Backend() {
  backend_window_ = nullptr;
}

void Backend::SwapBuffer() {
  auto render_surface = backend_window_->GetRenderSurfaceTarget();
  render_surface->GLContextPresent(0);
}

void Backend::SetWindowBindingHandler(WindowBindingHandlerDelegate* delegater) {
  backend_window_->SetWindowBindingHandler(delegater);
}

}  // namespace waffle
