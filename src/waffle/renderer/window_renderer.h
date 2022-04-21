// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WAFFLE_RENDERER_WINDOW_RENDERER_H_
#define WAFFLE_RENDERER_WINDOW_RENDERER_H_

#include <memory>

#include "waffle/renderer/shader/shader.h"
#include "waffle/renderer/texture.h"
#include "waffle/utils/vec2.h"

namespace waffle {

class WindowRenderer {
 public:
  WindowRenderer();
  ~WindowRenderer();

  // Prevent copying.
  WindowRenderer(WindowRenderer const&) = delete;
  WindowRenderer& operator=(WindowRenderer const&) = delete;

  bool Init();
  void Draw(Texture& texture, Vec2<int> pos, Vec2<double> size);

 private:
  std::unique_ptr<Shader> shader_ = nullptr;
  GLuint vertex_array_ = 0;
};

}  // namespace waffle

#endif  // WAFFLE_RENDERER_WINDOW_RENDERER_H_
