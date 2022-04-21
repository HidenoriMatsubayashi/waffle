// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WAFFLE_BACKEND_BACKEND_H_
#define WAFFLE_BACKEND_BACKEND_H_

#include <memory>

#include "waffle/backend/window/waffle_window.h"
#include "waffle/backend/window/window_binding_handler.h"
#include "waffle/waffle_property.h"

namespace waffle {

class Backend {
 public:
  Backend(wl_display* wl_display, WaffleWindowProperties view_properties);
  ~Backend();

  void LoadIntoTexture(wl_resource* buffer, Texture& texture) const {
    backend_window_->LoadIntoTexture(buffer, texture);
  }

  bool IsValid() const { return backend_window_->IsValid(); }

  bool DispatchEvent() const { return backend_window_->DispatchEvent(); }

  void SetWindowBindingHandler(WindowBindingHandlerDelegate* delegater);

  void SwapBuffer();

  int32_t GetFrameRate() const { return backend_window_->GetFrameRate(); }

 private:
  std::unique_ptr<WaffleWindow> backend_window_;
};

}  // namespace waffle

#endif  // WAFFLE_BACKEND_BACKEND_H_
