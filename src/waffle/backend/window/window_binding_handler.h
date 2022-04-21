// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WAFFLE_BACKEND_WINDOW_WINDOW_BINDING_HANDLER_H_
#define WAFFLE_BACKEND_WINDOW_WINDOW_BINDING_HANDLER_H_

#include <string>
#include <variant>

#include "waffle/backend/surface/surface_gl.h"
#include "waffle/backend/window/window_binding_handler_delegate.h"
#include "waffle/waffle_property.h"

namespace waffle {

class WindowBindingHandler {
 public:
  virtual ~WindowBindingHandler() = default;

  // Dispatches window events such as mouse and keyboard inputs. For Wayland,
  // you have to call this every time in the main loop.
  virtual bool DispatchEvent() = 0;

  // Create a surface.
  virtual bool CreateRenderSurface(int32_t width, int32_t height) = 0;

  // Destroy a surface which is currently used.
  virtual void DestroyRenderSurface() = 0;

  // Returns a valid SurfaceGl representing the backing
  // window.
  virtual SurfaceGl* GetRenderSurfaceTarget() const = 0;

  // Sets the delegate used to communicate state changes from window to view
  // such as key presses, mouse position updates etc.
  virtual void SetWindowBindingHandler(WindowBindingHandlerDelegate* view) = 0;

  // Returns the rotation(degree) for the backing window.
  virtual uint16_t GetRotationDegree() const = 0;

  // Returns the scale factor for the backing window.
  virtual double GetDpiScale() = 0;

  // Returns the bounds of the backing window in physical pixels.
  virtual WafflePhysicalWindowBounds GetPhysicalWindowBounds() const = 0;

  // Returns the frame rate of the display.
  virtual int32_t GetFrameRate() const = 0;

  // Returns the clipboard data.
  virtual std::string GetClipboardData() = 0;

  // Sets the clipboard data.
  virtual void SetClipboardData(const std::string& data) = 0;
};

}  // namespace waffle

#endif  // WAFFLE_BACKEND_WINDOW_WINDOW_BINDING_HANDLER_H_
