// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Copyright 2021 Sony Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "waffle/renderer/shader/shader.h"

#include <EGL/egl.h>
#include <GLES3/gl32.h>

#include <string>

#include "waffle/logger.h"

namespace waffle {

namespace {

struct GlProcs {
  PFNGLUSEPROGRAMPROC glUseProgram;
  bool valid;
};

static const GlProcs& GlProcs() {
  static struct GlProcs procs = {};
  static bool initialized = false;
  if (!initialized) {
    procs.glUseProgram = reinterpret_cast<PFNGLUSEPROGRAMPROC>(
        eglGetProcAddress("glUseProgram"));
    procs.valid = procs.glUseProgram;
    if (!procs.valid) {
      WAFFLE_LOG(ERROR) << "Failed to load GlProcs";
    }
    initialized = true;
  }
  return procs;
}

}  // namespace

void Shader::LoadProgram(std::string vertex_code, std::string fragment_code) {
  auto vertex = std::make_unique<ShaderContext>(vertex_code, GL_VERTEX_SHADER);
  auto fragment =
      std::make_unique<ShaderContext>(fragment_code, GL_FRAGMENT_SHADER);
  program_ =
      std::make_unique<ShaderProgram>(std::move(vertex), std::move(fragment));
}

void Shader::Bind() {
  const auto& gl = GlProcs();
  if (!gl.valid) {
    return;
  }
  gl.glUseProgram(program_->Program());
}

void Shader::Unbind() {
  const auto& gl = GlProcs();
  if (!gl.valid) {
    return;
  }
  gl.glUseProgram(false);
}

void Shader::UniformMatrix(std::string name, GLfloat* data) {
  auto location = glGetUniformLocation(program_->Program(), name.c_str());
  if (location == -1) {
    WAFFLE_LOG(ERROR) << "Failed to get uniform location (" << name << ")";
    return;
  }
  glUniformMatrix4fv(location, 1, GL_FALSE, data);
}

}  // namespace waffle
