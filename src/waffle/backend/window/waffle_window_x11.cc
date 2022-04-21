// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Copyright 2021 Sony Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "waffle/backend/window/waffle_window_x11.h"

#include <fcntl.h>
#include <linux/input-event-codes.h>
#include <unistd.h>

#include "waffle/backend/surface/context_egl.h"
#include "waffle/logger.h"

namespace waffle {

namespace {
// Only Button1 - Button5 are defined in X11/X.h
constexpr int kButton6 = 6;
constexpr int kButton7 = 7;
constexpr int kButton8 = 8;
constexpr int kButton9 = 9;

uint32_t GetLinuxButtonValue(uint32_t x11_button) {
  switch (x11_button) {
    case Button1:
      return BTN_LEFT;
    case Button2:
      return BTN_MIDDLE;
    case Button3:
      return BTN_RIGHT;
    default:
      WAFFLE_LOG(ERROR) << "Unkown button: " << x11_button;
      return BTN_EXTRA;
  }
}
}  // namespace

WaffleWindowX11::WaffleWindowX11(wl_display* wl_display,
                                 WaffleWindowProperties view_properties) {
  wl_display_ = wl_display;
  window_properties_ = view_properties;
  SetRotation(window_properties_.view_rotation);

  display_ = XOpenDisplay(NULL);
  if (!display_) {
    WAFFLE_LOG(ERROR) << "Failed to open display.";
    return;
  }

  display_valid_ = true;
}

WaffleWindowX11::~WaffleWindowX11() {
  display_valid_ = false;
  if (display_) {
    XSetCloseDownMode(display_, DestroyAll);
    XCloseDisplay(display_);
  }
}

bool WaffleWindowX11::IsValid() const {
  if (!display_valid_ || !native_window_ || !render_surface_ ||
      !native_window_->IsValid() || !render_surface_->IsValid()) {
    return false;
  }
  return true;
}

bool WaffleWindowX11::DispatchEvent() {
  while (XPending(display_)) {
    XEvent event;
    XNextEvent(display_, &event);
    switch (event.type) {
      case EnterNotify:
      case MotionNotify:
        if (binding_handler_delegate_) {
          binding_handler_delegate_->OnPointerMove(event.xbutton.x,
                                                   event.xbutton.y);
        }
        break;
      case LeaveNotify:
        if (binding_handler_delegate_) {
          binding_handler_delegate_->OnPointerLeave();
        }
        break;
      case ButtonPress: {
        constexpr bool button_pressed = true;
        HandlePointerButtonEvent(event.xbutton.button, button_pressed,
                                 event.xbutton.x, event.xbutton.y);
      } break;
      case ButtonRelease: {
        constexpr bool button_pressed = false;
        HandlePointerButtonEvent(event.xbutton.button, button_pressed,
                                 event.xbutton.x, event.xbutton.y);
      } break;
      case KeyPress:
        if (binding_handler_delegate_) {
          constexpr bool pressed = true;
          binding_handler_delegate_->OnKey(event.xkey.keycode - 8, pressed);
        }
        break;
      case KeyRelease:
        if (binding_handler_delegate_) {
          constexpr bool pressed = false;
          binding_handler_delegate_->OnKey(event.xkey.keycode - 8, pressed);
        }
        break;
      case ConfigureNotify: {
        auto width = event.xconfigure.width;
        auto height = event.xconfigure.height;
        if (current_rotation_ == 90 || current_rotation_ == 270) {
          std::swap(width, height);
        }

        if (((width != window_properties_.width) ||
             (height != window_properties_.height))) {
          window_properties_.width = width;
          window_properties_.height = height;
          if (binding_handler_delegate_) {
            binding_handler_delegate_->OnWindowSizeChanged(
                window_properties_.width, window_properties_.height);
          }
        }
      } break;
      case ClientMessage:
        native_window_->Destroy(display_);
        // Quit the main loop.
        return false;
      case DestroyNotify:
        // Quit the main loop.
        return false;
      default:
        break;
    }
  }
  return true;
}

bool WaffleWindowX11::CreateRenderSurface(int32_t width, int32_t height) {
  auto context_egl =
      std::make_unique<ContextEgl>(std::make_unique<EnvironmentEgl>(display_));

  if (current_rotation_ == 90 || current_rotation_ == 270) {
    std::swap(width, height);
  }
  native_window_ = std::make_unique<NativeWindowX11>(
      display_, context_egl->GetAttrib(EGL_NATIVE_VISUAL_ID), width, height);
  if (!native_window_->IsValid()) {
    WAFFLE_LOG(ERROR) << "Failed to create the native window";
    return false;
  }

  render_surface_ = std::make_unique<SurfaceGl>(std::move(context_egl));
  render_surface_->SetNativeWindow(native_window_.get());
  render_surface_->GLContextMakeCurrent();

  render_surface_->BindWlDisplay(wl_display_);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  return true;
}

void WaffleWindowX11::DestroyRenderSurface() {
  render_surface_->UnbindWlDisplay(wl_display_);

  // destroy the main surface before destroying the client window on X11.
  render_surface_ = nullptr;
  native_window_ = nullptr;
}

std::string WaffleWindowX11::GetClipboardData() {
  return clipboard_data_;
}

void WaffleWindowX11::SetClipboardData(const std::string& data) {
  clipboard_data_ = data;
}

void WaffleWindowX11::HandlePointerButtonEvent(uint32_t button,
                                               bool button_pressed,
                                               int16_t x,
                                               int16_t y) {
  if (binding_handler_delegate_) {
    uint32_t waffle_button;
    switch (button) {
      case Button1:
      case Button2:
      case Button3:
        waffle_button = GetLinuxButtonValue(button);
        break;
      case Button4:
      case Button5:
      case kButton6:
      case kButton7: {
        const bool vertical_scroll = (button == Button4 || button == Button5);
        const double delta = button == Button5 ? 1 : -1;
        constexpr int32_t kScrollOffsetMultiplier = 20;
        binding_handler_delegate_->OnScroll(x, y, vertical_scroll ? 0 : delta,
                                            vertical_scroll ? delta : 0,
                                            kScrollOffsetMultiplier);
        return;
      }
        /*
              case kButton8:
                waffle_button = kFlutterPointerButtonMouseBack;
                break;
              case kButton9:
                waffle_button = kFlutterPointerButtonMouseForward;
                break;
        */
      default:
        WAFFLE_LOG(ERROR) << "Not expected button input: " << button;
        return;
    }

    if (button_pressed) {
      binding_handler_delegate_->OnPointerButton(x, y, waffle_button,
                                                 button_pressed);
    } else {
      binding_handler_delegate_->OnPointerButton(x, y, waffle_button,
                                                 button_pressed);
    }
  }
}

}  // namespace waffle
