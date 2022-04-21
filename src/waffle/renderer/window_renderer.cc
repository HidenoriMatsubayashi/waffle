// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "waffle/renderer/window_renderer.h"

#include <EGL/egl.h>
#include <GLES3/gl32.h>

#include <memory>
#include <string>

#include "waffle/logger.h"

namespace waffle {

namespace {

constexpr char kVertexShaderCode[] =
    "#version 300 es                                         \n"
    "layout (location = 0) in vec2 position;                 \n"
    "layout (location = 1) in vec2 texturePositionIn;        \n"
    "out vec2 texturePosition;                               \n"
    "uniform mat4 transform;                                 \n"
    "void main() {                                           \n"
    "  gl_Position = transform * vec4(position, 0.0f, 1.0f); \n"
    "  texturePosition = texturePositionIn;                  \n"
    "}                                                       \n";

constexpr char kFragmentShaderCode[] =
    "#version 300 es                                         \n"
    "precision mediump float;                                \n"
    "in vec2 texturePosition;                                \n"
    "out vec4 color;                                         \n"
    "uniform sampler2D textureData;                          \n"
    "void main() {                                           \n"
    "  color = texture(textureData, texturePosition);        \n"
    "}                                                       \n";

struct GlProcs {
  PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
  PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
  PFNGLGENBUFFERSPROC glGenBuffers;
  PFNGLDELETEBUFFERSPROC glDeleteBuffers;
  PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
  PFNGLDRAWELEMENTSPROC glDrawElements;
  PFNGLBINDBUFFERPROC glBindBuffer;
  PFNGLBUFFERDATAPROC glBufferData;
  PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
  PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
  bool valid;
};

static const GlProcs& GlProcs() {
  static struct GlProcs procs = {};
  static bool initialized = false;
  if (!initialized) {
    procs.glGenVertexArrays = reinterpret_cast<PFNGLGENVERTEXARRAYSPROC>(
        eglGetProcAddress("glGenVertexArrays"));
    procs.glDeleteVertexArrays = reinterpret_cast<PFNGLDELETEVERTEXARRAYSPROC>(
        eglGetProcAddress("glDeleteVertexArrays"));
    procs.glGenBuffers = reinterpret_cast<PFNGLGENBUFFERSPROC>(
        eglGetProcAddress("glGenBuffers"));
    procs.glDeleteBuffers = reinterpret_cast<PFNGLDELETEBUFFERSPROC>(
        eglGetProcAddress("glDeleteBuffers"));
    procs.glBindVertexArray = reinterpret_cast<PFNGLBINDVERTEXARRAYPROC>(
        eglGetProcAddress("glBindVertexArray"));
    procs.glDrawElements = reinterpret_cast<PFNGLDRAWELEMENTSPROC>(
        eglGetProcAddress("glDrawElements"));
    procs.glBindBuffer = reinterpret_cast<PFNGLBINDBUFFERPROC>(
        eglGetProcAddress("glBindBuffer"));
    procs.glBufferData = reinterpret_cast<PFNGLBUFFERDATAPROC>(
        eglGetProcAddress("glBufferData"));
    procs.glVertexAttribPointer =
        reinterpret_cast<PFNGLVERTEXATTRIBPOINTERPROC>(
            eglGetProcAddress("glVertexAttribPointer"));
    procs.glEnableVertexAttribArray =
        reinterpret_cast<PFNGLENABLEVERTEXATTRIBARRAYPROC>(
            eglGetProcAddress("glEnableVertexAttribArray"));
    procs.valid = procs.glGenVertexArrays && procs.glDeleteVertexArrays &&
                  procs.glGenBuffers && procs.glDeleteBuffers &&
                  procs.glBindVertexArray && procs.glDrawElements &&
                  procs.glBindBuffer && procs.glBufferData &&
                  procs.glVertexAttribPointer &&
                  procs.glEnableVertexAttribArray;
    if (!procs.valid) {
      WAFFLE_LOG(ERROR) << "Failed to load GlProcs";
    }
    initialized = true;
  }
  return procs;
}

};  // namespace

WindowRenderer::WindowRenderer() {}

WindowRenderer::~WindowRenderer() {
  const auto& gl = GlProcs();
  if (!gl.valid) {
    return;
  }
  gl.glDeleteVertexArrays(1, &vertex_array_);
}

bool WindowRenderer::Init() {
  shader_ = std::make_unique<Shader>();
  shader_->LoadProgram(kVertexShaderCode, kFragmentShaderCode);

  constexpr GLfloat vertices[] = {
      // clang-format off
      // position		 // texture position
       1.0f,  1.0f,  1.0f, 0.0f, // top-right
       1.0f, -1.0f,  1.0f, 1.0f, // bottom-right
      -1.0f, -1.0f,  0.0f, 1.0f, // bottom-left
      -1.0f,  1.0f,  0.0f, 0.0f, // top-left
      // clang-format on
  };
  constexpr GLuint indices[] = {
      0, 1, 3,  // First Triangle
      1, 2, 3   // Second Triangle
  };
  GLuint vbo, ebo;

  const auto& gl = GlProcs();
  if (!gl.valid) {
    return false;
  }
  gl.glGenVertexArrays(1, &vertex_array_);
  gl.glGenBuffers(1, &vbo);
  gl.glGenBuffers(1, &ebo);
  {
    gl.glBindVertexArray(vertex_array_);
    {
      gl.glBindBuffer(GL_ARRAY_BUFFER, vbo);
      {
        gl.glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
                        GL_STATIC_DRAW);
        gl.glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat),
                                 0);
        gl.glEnableVertexAttribArray(0);
        gl.glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat),
                                 (GLvoid*)(2 * sizeof(GLfloat)));
        gl.glEnableVertexAttribArray(1);
      }
      gl.glBindBuffer(GL_ARRAY_BUFFER, false);

      gl.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
      {
        gl.glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                        GL_STATIC_DRAW);
      }
      // Don't unbind the EBO to keep it bound to this VAO.
    }
    gl.glBindVertexArray(false);
  }
  gl.glDeleteBuffers(1, &vbo);
  gl.glDeleteBuffers(1, &ebo);

  return true;
}

void WindowRenderer::Draw(Texture& texture, Vec2<int> pos, Vec2<double> size) {
  shader_->Bind();
  {
    GLfloat transform[] = {
        // clang-format off
        static_cast<GLfloat>(size.X()),      0.0,                                 0.0, 0.0,
        0.0,                                 static_cast<GLfloat>(size.Y()),      0.0, 0.0,
        0.0,                                 0.0,                                 1.0, 0.0,
        GLfloat(pos.X() * 2 + size.X() - 1), GLfloat(pos.Y() * 2 + size.Y() - 1), 0.0, 1.0,
        // clang-format on
    };
    shader_->UniformMatrix("transform", transform);

    texture.Bind();
    {
      const auto& gl = GlProcs();
      if (gl.valid) {
        gl.glBindVertexArray(vertex_array_);
        gl.glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        gl.glBindVertexArray(false);
      }
    }
    texture.Unbind();
  }
  shader_->Unbind();
}

}  // namespace waffle
