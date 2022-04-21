// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WAFFLE_RENDERER_TEXTURE_CONTEXT_H_
#define WAFFLE_RENDERER_TEXTURE_CONTEXT_H_

#include <GLES3/gl32.h>

#include "waffle/utils/vec2.h"

namespace waffle {

class TextureContext {
 public:
  TextureContext();
  ~TextureContext();

  void Size(int x, int y) { texture_size_ = Vec2<int>(x, y); }
  Vec2<int> Size() { return texture_size_; }
  GLuint Texture() { return texture_id_; }

 private:
  GLuint texture_id_ = 0;
  Vec2<int> texture_size_;
};

}  // namespace waffle

#endif  // WAFFLE_RENDERER_TEXTURE_CONTEXT_H_
