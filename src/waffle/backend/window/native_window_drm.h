// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Copyright 2021 Sony Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WAFFLE_BACKEND_WINDOW_NATIVE_WINDOW_DRM_H_
#define WAFFLE_BACKEND_WINDOW_NATIVE_WINDOW_DRM_H_

#include <xf86drmMode.h>

#include <string>

#include "waffle/backend/surface/surface_gl.h"
#include "waffle/backend/window/native_window.h"

namespace waffle {

class NativeWindowDrm : public NativeWindow {
 public:
  NativeWindowDrm(const char* device_filename, const uint16_t rotation);
  virtual ~NativeWindowDrm();

  bool ConfigureDisplay(const uint16_t rotation);

  bool MoveCursor(double x, double y);

  virtual bool ShowCursor(double x, double y) = 0;

  virtual bool UpdateCursor(const std::string& cursor_name,
                            double x,
                            double y) = 0;

  virtual bool DismissCursor() = 0;

  virtual std::unique_ptr<SurfaceGl> CreateRenderSurface() = 0;

 protected:
  drmModeConnectorPtr FindConnector(drmModeResPtr resources);

  drmModeEncoder* FindEncoder(drmModeRes* resources,
                              drmModeConnector* connector);

  // Convert Flutter's cursor value to cursor data.
  const uint32_t* GetCursorData(const std::string& cursor_name);

  int drm_device_;
  uint32_t drm_connector_id_;
  drmModeCrtc* drm_crtc_ = nullptr;
  drmModeModeInfo drm_mode_info_;

  std::string cursor_name_ = "";
  std::pair<int32_t, int32_t> cursor_hotspot_ = {0, 0};
};

}  // namespace waffle

#endif  // WAFFLE_BACKEND_WINDOW_NATIVE_WINDOW_DRM_H_
