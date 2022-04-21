// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WAFFLE_COMPOSITOR_RENDERER_TEXTURE_H_
#define WAFFLE_COMPOSITOR_RENDERER_TEXTURE_H_

#include <memory>
#include <string>

#include "waffle/renderer/texture_context.h"
#include "waffle/utils/vec2.h"

namespace waffle {

class Texture {
 public:
  Texture();
  ~Texture() = default;

  void Init();
  bool Valid() const { return context_ != nullptr; };
  Vec2<int> Size();
  void LoadEGLImage(void* image, int x, int y);
  void LoadBufferImage(void* image, int x, int y);
  void LoadFileImage(std::string filename);
  void Bind();
  void Unbind();

 private:
  std::shared_ptr<TextureContext> context_ = nullptr;
};

}  // namespace waffle

#endif  // WAFFLE_COMPOSITOR_RENDERER_TEXTURE_H_
