// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Copyright 2021 Sony Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WAFFLE_RENDERER_SHADER_SHADER_CONTEXT_H_
#define WAFFLE_RENDERER_SHADER_SHADER_CONTEXT_H_

#include <GLES3/gl32.h>

#include <string>

namespace waffle {

class ShaderContext {
 public:
  ShaderContext(std::string code, GLenum type);
  ~ShaderContext();

  GLuint Shader() const { return shader_; }

 private:
  GLuint shader_;
};

}  // namespace waffle

#endif  // WAFFLE_RENDERER_SHADER_SHADER_CONTEXT_H_
