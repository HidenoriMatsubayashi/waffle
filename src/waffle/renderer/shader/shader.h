// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Copyright 2021 Sony Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WAFFLE_RENDERER_SHADER_SHADER_H_
#define WAFFLE_RENDERER_SHADER_SHADER_H_

#include <GLES3/gl32.h>

#include <memory>
#include <string>

#include "waffle/renderer/shader/shader_context.h"
#include "waffle/renderer/shader/shader_program.h"

namespace waffle {

class Shader {
 public:
  Shader() = default;
  ~Shader() = default;

  void LoadProgram(std::string vertex_code, std::string fragment_code);
  void Bind();
  void Unbind();
  GLuint Program() const { return program_->Program(); }
  void UniformMatrix(std::string name, GLfloat* data);

 private:
  std::unique_ptr<ShaderProgram> program_;
  std::unique_ptr<ShaderContext> vertex_;
  std::unique_ptr<ShaderContext> fragment_;
};

}  // namespace waffle

#endif  // WAFFLE_RENDERER_SHADER_SHADER_H_
