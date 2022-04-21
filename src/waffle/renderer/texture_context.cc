// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "waffle/renderer/texture_context.h"

#include <EGL/egl.h>

#include "waffle/logger.h"

namespace waffle {

namespace {

struct GlProcs {
  PFNGLGENTEXTURESPROC glGenTextures;
  PFNGLDELETETEXTURESPROC glDeleteTextures;
  PFNGLBINDTEXTUREPROC glBindTexture;
  PFNGLTEXPARAMETERIPROC glTexParameteri;
  bool valid;
};

static const GlProcs& GlProcs() {
  static struct GlProcs procs = {};
  static bool initialized = false;
  if (!initialized) {
    procs.glGenTextures = reinterpret_cast<PFNGLGENTEXTURESPROC>(
        eglGetProcAddress("glGenTextures"));
    procs.glDeleteTextures = reinterpret_cast<PFNGLDELETETEXTURESPROC>(
        eglGetProcAddress("glDeleteTextures"));
    procs.glBindTexture = reinterpret_cast<PFNGLBINDTEXTUREPROC>(
        eglGetProcAddress("glBindTexture"));
    procs.glTexParameteri = reinterpret_cast<PFNGLTEXPARAMETERIPROC>(
        eglGetProcAddress("glTexParameteri"));
    procs.valid = procs.glGenTextures && procs.glDeleteTextures &&
                  procs.glBindTexture && procs.glTexParameteri;
    if (!procs.valid) {
      WAFFLE_LOG(ERROR) << "Failed to load GlProcs";
    }
    initialized = true;
  }
  return procs;
}

}  // namespace

TextureContext::TextureContext() {
  const auto& gl = GlProcs();
  if (!gl.valid) {
    return;
  }

  gl.glGenTextures(1, &texture_id_);
  gl.glBindTexture(GL_TEXTURE_2D, texture_id_);
  {
    gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  }
  gl.glBindTexture(GL_TEXTURE_2D, 0);
}

TextureContext::~TextureContext() {
  const auto& gl = GlProcs();
  if (!gl.valid) {
    return;
  }

  gl.glDeleteTextures(1, &texture_id_);
}

}  // namespace waffle
