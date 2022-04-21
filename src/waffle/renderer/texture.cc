// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "waffle/renderer/texture.h"

#include <EGL/egl.h>
#include <GLES3/gl32.h>
#include <SOIL/SOIL.h>

#include "waffle/logger.h"

namespace waffle {

namespace {

typedef void (*glEGLImageTargetTexture2DOESProc)(GLenum target, EGLImage image);

struct GlProcs {
  glEGLImageTargetTexture2DOESProc glEGLImageTargetTexture2DOES;
  PFNGLBINDTEXTUREPROC glBindTexture;
  PFNGLTEXPARAMETERIPROC glTexParameteri;
  PFNGLTEXIMAGE2DPROC glTexImage2D;
  bool valid;
};

static const GlProcs& GlProcs() {
  static struct GlProcs procs = {};
  static bool initialized = false;
  if (!initialized) {
    procs.glEGLImageTargetTexture2DOES =
        reinterpret_cast<glEGLImageTargetTexture2DOESProc>(
            eglGetProcAddress("glEGLImageTargetTexture2DOES"));
    procs.glBindTexture = reinterpret_cast<PFNGLBINDTEXTUREPROC>(
        eglGetProcAddress("glBindTexture"));
    procs.glTexParameteri = reinterpret_cast<PFNGLTEXPARAMETERIPROC>(
        eglGetProcAddress("glTexParameteri"));
    procs.glTexImage2D = reinterpret_cast<PFNGLTEXIMAGE2DPROC>(
        eglGetProcAddress("glTexImage2D"));
    procs.valid = procs.glEGLImageTargetTexture2DOES && procs.glBindTexture &&
                  procs.glTexParameteri && procs.glTexImage2D;
    if (!procs.valid) {
      WAFFLE_LOG(ERROR) << "Failed to load GlProcs";
    }
    initialized = true;
  }
  return procs;
}

}  // namespace

Texture::Texture() {
  Init();
}

void Texture::Init() {
  if (!context_) {
    context_ = std::make_shared<TextureContext>();
  }
}

void Texture::LoadFileImage(std::string filename) {
  if (!context_) {
    context_ = std::make_shared<TextureContext>();
  }

  int width, height;
  auto* image =
      SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
  if (!image) {
    WAFFLE_LOG(WARNING) << "Failed to load the image: " << SOIL_last_result();
    return;
  }

  const auto& gl = GlProcs();
  if (!gl.valid) {
    return;
  }
  gl.glBindTexture(GL_TEXTURE_2D, context_->Texture());
  {
    gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    gl.glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                    GL_UNSIGNED_BYTE, image);
  }
  gl.glBindTexture(GL_TEXTURE_2D, 0);

  context_->Size(width, height);
  SOIL_free_image_data(image);
}

void Texture::LoadEGLImage(void* image, int x, int y) {
  if (!context_) {
    context_ = std::make_shared<TextureContext>();
  }

  const auto& gl = GlProcs();
  if (!gl.valid) {
    return;
  }
  gl.glBindTexture(GL_TEXTURE_2D, context_->Texture());
  gl.glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, image);
  gl.glBindTexture(GL_TEXTURE_2D, 0);

  context_->Size(x, y);
}

void Texture::LoadBufferImage(void* data, int x, int y) {
  if (!context_) {
    context_ = std::make_shared<TextureContext>();
  }

  const auto& gl = GlProcs();
  if (!gl.valid) {
    return;
  }
  gl.glBindTexture(GL_TEXTURE_2D, context_->Texture());
  {
    gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // todo:
    // SHM color format(ARGB8888) to RGBA format.
    // SHM's ARGB8888 means BGRA.
    gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_BLUE);
    gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_GREEN);
    gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
    gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ALPHA);
    gl.glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA,
                    GL_UNSIGNED_BYTE, data);
  }
  gl.glBindTexture(GL_TEXTURE_2D, 0);

  context_->Size(x, y);
}

void Texture::Bind() {
  const auto& gl = GlProcs();
  if (!gl.valid) {
    return;
  }
  gl.glBindTexture(GL_TEXTURE_2D, context_->Texture());
}

void Texture::Unbind() {
  const auto& gl = GlProcs();
  if (!gl.valid) {
    return;
  }
  gl.glBindTexture(GL_TEXTURE_2D, 0);
}

Vec2<int> Texture::Size() {
  return context_->Size();
}

}  // namespace waffle
