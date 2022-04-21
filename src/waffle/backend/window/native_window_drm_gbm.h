// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Copyright 2021 Sony Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WAFFLE_BACKEND_WINDOW_WAFFLE_WINDOW_NATIVE_WINDOW_DRM_GBM_H_
#define WAFFLE_BACKEND_WINDOW_WAFFLE_WINDOW_NATIVE_WINDOW_DRM_GBM_H_

#include <gbm.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

#include <string>

#include "waffle/backend/window/native_window_drm.h"

namespace waffle {

class NativeWindowDrmGbm : public NativeWindowDrm {
 public:
  NativeWindowDrmGbm(const char* device_filename, const uint16_t rotation);
  ~NativeWindowDrmGbm();

  // |NativeWindowDrm|
  bool ShowCursor(double x, double y) override;

  // |NativeWindowDrm|
  bool UpdateCursor(const std::string& cursor_name,
                    double x,
                    double y) override;

  // |NativeWindowDrm|
  bool DismissCursor() override;

  // |NativeWindowDrm|
  std::unique_ptr<SurfaceGl> CreateRenderSurface() override;

  // |NativeWindow|
  bool IsNeedRecreateSurfaceAfterResize() const override;

  // |NativeWindow|
  bool Resize(const size_t width, const size_t height) override;

  // |NativeWindow|
  void SwapBuffers() override;

 private:
  bool CreateGbmSurface();

  bool CreateCursorBuffer(const std::string& cursor_name);

  gbm_bo* gbm_previous_bo_ = nullptr;
  uint32_t gbm_previous_fb_;
  gbm_device* gbm_device_ = nullptr;
  gbm_bo* gbm_cursor_bo_ = nullptr;
};

}  // namespace waffle

#endif  // WAFFLE_BACKEND_WINDOW_WAFFLE_WINDOW_NATIVE_WINDOW_DRM_GBM_H_
