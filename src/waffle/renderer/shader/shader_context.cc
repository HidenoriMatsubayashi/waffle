// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Copyright 2021 Sony Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "waffle/renderer/shader/shader_context.h"

#include <EGL/egl.h>
#include <GLES3/gl32.h>

#include <string>

#include "waffle/logger.h"

namespace waffle {

namespace {

struct GlProcs {
  PFNGLCREATESHADERPROC glCreateShader;
  PFNGLSHADERSOURCEPROC glShaderSource;
  PFNGLCOMPILESHADERPROC glCompileShader;
  PFNGLGETSHADERIVPROC glGetShaderiv;
  PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
  PFNGLDELETESHADERPROC glDeleteShader;
  bool valid;
};

static const GlProcs& GlProcs() {
  static struct GlProcs procs = {};
  static bool initialized = false;
  if (!initialized) {
    procs.glCreateShader = reinterpret_cast<PFNGLCREATESHADERPROC>(
        eglGetProcAddress("glCreateShader"));
    procs.glShaderSource = reinterpret_cast<PFNGLSHADERSOURCEPROC>(
        eglGetProcAddress("glShaderSource"));
    procs.glCompileShader = reinterpret_cast<PFNGLCOMPILESHADERPROC>(
        eglGetProcAddress("glCompileShader"));
    procs.glGetShaderiv = reinterpret_cast<PFNGLGETSHADERIVPROC>(
        eglGetProcAddress("glGetShaderiv"));
    procs.glGetShaderInfoLog = reinterpret_cast<PFNGLGETSHADERINFOLOGPROC>(
        eglGetProcAddress("glGetShaderInfoLog"));
    procs.glDeleteShader = reinterpret_cast<PFNGLDELETESHADERPROC>(
        eglGetProcAddress("glDeleteShader"));
    procs.valid = procs.glCreateShader && procs.glShaderSource &&
                  procs.glCompileShader && procs.glGetShaderiv &&
                  procs.glGetShaderInfoLog && procs.glDeleteShader;
    if (!procs.valid) {
      WAFFLE_LOG(ERROR) << "Failed to load GlProcs";
    }
    initialized = true;
  }
  return procs;
}

}  // namespace

ShaderContext::ShaderContext(std::string code, GLenum type) : shader_(0) {
  const auto& gl = GlProcs();
  if (!gl.valid) {
    return;
  }

  shader_ = gl.glCreateShader(type);
  if (!shader_) {
    WAFFLE_LOG(ERROR) << "Failed to create the shader";
    return;
  }

  auto* c_code = code.c_str();
  gl.glShaderSource(shader_, 1, &c_code, nullptr);
  gl.glCompileShader(shader_);

  GLint success;
  gl.glGetShaderiv(shader_, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLchar buf[1024];

    gl.glGetShaderInfoLog(shader_, sizeof(buf), nullptr, buf);
    WAFFLE_LOG(ERROR) << "Couldn't compile the shader: " << buf;

    gl.glDeleteShader(shader_);
    shader_ = 0;
  }
}

ShaderContext::~ShaderContext() {
  const auto& gl = GlProcs();
  if (!gl.valid) {
    return;
  }
  gl.glDeleteShader(shader_);
}

};  // namespace waffle
