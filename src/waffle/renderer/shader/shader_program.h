// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Copyright 2021 Sony Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WAFFLE_RENDERER_SHADER_SHADER_PROGRAM_H_
#define WAFFLE_RENDERER_SHADER_SHADER_PROGRAM_H_

#include <GLES3/gl32.h>

#include <memory>

#include "waffle/renderer/shader/shader_context.h"

namespace waffle {

class ShaderProgram {
 public:
  ShaderProgram(std::unique_ptr<ShaderContext> vertex_shader,
                std::unique_ptr<ShaderContext> fragment_shader);
  ~ShaderProgram();

  GLuint Program() const { return program_; }

 private:
  GLuint program_;
  std::unique_ptr<ShaderContext> vertex_shader_;
  std::unique_ptr<ShaderContext> fragment_shader_;
};

}  // namespace waffle

#endif  // WAFFLE_RENDERER_SHADER_SHADER_PROGRAM_H_
