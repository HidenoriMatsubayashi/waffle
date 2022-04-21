// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WAFFLE_WAFFLE_PROPERTY_H_
#define WAFFLE_WAFFLE_PROPERTY_H_

#include <cstddef>
#include <cstdint>
#include <string>

namespace waffle {

// Structure containing physical bounds of a Window
struct WafflePhysicalWindowBounds {
  size_t width;
  size_t height;
};

// The View display mode.
enum WaffleViewMode {
  kNormalscreen = 0,
  kFullscreen = 1,
};

// The View rotation setting.
enum WaffleViewRotation {
  kRotation_0 = 0,
  kRotation_90 = 1,
  kRotation_180 = 2,
  kRotation_270 = 3,
};

// Properties for configuring a Waffle view instance.
typedef struct {
  size_t width;
  size_t height;
  WaffleViewMode view_mode;
  WaffleViewRotation view_rotation;
  bool use_mouse_cursor;
  std::string background_image_filepath;
} WaffleWindowProperties;

}  // namespace waffle

#endif  // WAFFLE_WAFFLE_PROPERTY_H_
