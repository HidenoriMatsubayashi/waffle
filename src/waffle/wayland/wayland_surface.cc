// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "waffle/wayland/wayland_surface.h"

#include <wayland/protocols/wayland-server-protocol.h>

#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

#include "waffle/compositor/compositor.h"
#include "waffle/logger.h"
#include "waffle/renderer/window_renderer.h"
#include "waffle/utils/vec2.h"
#include "waffle/wayland/wayland_binding_handler_delegate.h"
#include "waffle/wayland/wayland_resource.h"
#include "waffle/wayland/wayland_seat.h"

namespace waffle {

struct WaylandSurface::Impl : WaylandResource::Data,
                              WaylandBindingHandlerDelegate {
  wl_resource* wl_resource_buffer = nullptr;
  Texture texture;
  WaylandResource resource_surface;
  WindowRenderer renderer;
  Vec2<int> size;
  bool is_damaged = false;

  static const struct wl_surface_interface kWlSurfaceInterface;
  static std::vector<WaylandResource> callbacks;

  void OnPointerMove(Vec2<double> pos) {
    if (!resource_surface.IsValid()) {
      return;
    }

    // todo: compositor window size.
    constexpr double kHeight = 1024;
    auto surface_pos = Vec2<double>(pos.X(), pos.Y() - (kHeight - size.Y()));
    if (surface_pos.Y() < 0) {
      return;
    }
    WlSeat::OnPointerMove(surface_pos, resource_surface);
  }

  void OnPointerLeave() {
    if (!resource_surface.IsValid()) {
      return;
    }
    WlSeat::OnPointerLeave(resource_surface);
  }

  void OnPointerClick(uint button, bool down) {
    if (!resource_surface.IsValid()) {
      return;
    }
    WlSeat::OnPointerClick(button, down, resource_surface);
  }

  void OnKey(uint key, bool down) {
    if (!resource_surface.IsValid()) {
      return;
    }
    WlSeat::OnKey(key, down, resource_surface);
  }
};

std::vector<WaylandResource> WaylandSurface::Impl::callbacks;

const struct wl_surface_interface WaylandSurface::Impl::kWlSurfaceInterface {
  .destroy =
      +[](wl_client* client, wl_resource* resource) {
        WAFFLE_LOG(TRACE) << "wl_surface_interface.destroy is called.";
        WaylandResource(resource).Destroy();
      },
  .attach =
      +[](wl_client* client,
          wl_resource* resource,
          wl_resource* buffer,
          int32_t x,
          int32_t y) {
        WAFFLE_LOG(TRACE) << "wl_surface_interface.attach is called.";

        auto impl = WaylandResource(resource).Get<Impl>();
        if (!impl) {
          WAFFLE_LOG(INFO) << "Resource is invalid.";
          return;
        }
        impl->wl_resource_buffer = buffer;
      },
  .damage =
      +[](wl_client* client,
          wl_resource* resource,
          int32_t x,
          int32_t y,
          int32_t width,
          int32_t height) {
        WAFFLE_LOG(TRACE) << "wl_surface_interface.damage called.";

        auto impl = WaylandResource(resource).Get<Impl>();
        if (!impl) {
          WAFFLE_LOG(INFO) << "Resource is invalid.";
          return;
        }
        impl->is_damaged = true;
      },
  .frame =
      +[](wl_client* client, wl_resource* resource, uint32_t callback) {
        WAFFLE_LOG(TRACE) << "wl_surface_interface.frame called.";

        WaylandResource resource_callback;
        resource_callback.Create(nullptr, client, callback,
                                 &wl_callback_interface, 1, nullptr);
        callbacks.push_back(resource_callback);
      },
  .set_opaque_region =
      +[](wl_client* client, wl_resource* resource, wl_resource* region) {
        WAFFLE_LOG(TRACE) << "wl_surface_interface.set_opaque_region is "
                             "called (not yet implemented)";
      },
  .set_input_region =
      +[](wl_client* client, wl_resource* resource, wl_resource* region) {
        WAFFLE_LOG(WARNING) << "wl_surface_interface.set_input_region is "
                               "called (not yet implemented)";
      },
  .commit =
      +[](wl_client* client, wl_resource* resource) {
        WAFFLE_LOG(TRACE) << "wl_surface_interface.commit is called.";

        auto impl = WaylandResource(resource).Get<Impl>();
        if (!impl) {
          WAFFLE_LOG(INFO) << "Resource is invalid.";
          return;
        }

        auto* buffer = impl->wl_resource_buffer;
        if (buffer != nullptr && impl->is_damaged) {
          // TODO: Repaint damaged region only.
          uint32_t width = 0;
          uint32_t height = 0;
          auto* shm_buffer = wl_shm_buffer_get(buffer);

          if (shm_buffer) {
            width = wl_shm_buffer_get_width(shm_buffer);
            height = wl_shm_buffer_get_height(shm_buffer);
            auto* data = wl_shm_buffer_get_data(shm_buffer);
            auto format = wl_shm_buffer_get_format(shm_buffer);

            // TODO: Support target shm buffer format.
            switch (format) {
              case WL_SHM_FORMAT_ARGB8888:
                WAFFLE_LOG(TRACE) << "shm buffer format: ARGB8888";
                break;
              case WL_SHM_FORMAT_XRGB8888:
                WAFFLE_LOG(TRACE) << "shm buffer format: XRGB8888";
                break;
              default:
                WAFFLE_LOG(TRACE) << "shm buffer format: " << format;
                break;
            }
            impl->texture.LoadBufferImage(data, width, height);
          } else {
            auto* compositor = waffle::Compositor::Instance();
            compositor->LoadIntoTexture(buffer, impl->texture);
          }

          wl_buffer_send_release(buffer);
          impl->wl_resource_buffer = nullptr;
          impl->size = Vec2<int>(width, height);
        }
      },
  .set_buffer_transform =
      +[](wl_client* client, wl_resource* resource, int32_t transform) {
        WAFFLE_LOG(WARNING) << "wl_surface_interface::set_buffer_transform "
                               "(not yet implemented)";
      },
  .set_buffer_scale =
      +[](wl_client* client, wl_resource* resource, int32_t scale) {
        WAFFLE_LOG(TRACE) << "wl_surface_interface::set_buffer_scale called "
                             "(not yet implemented)";
        if (scale != 1) {
          WAFFLE_LOG(ERROR) << "scale is " << std::to_string(scale);
        }
      },
  .damage_buffer = +[](wl_client* client,
                       wl_resource* resource,
                       int32_t x,
                       int32_t y,
                       int32_t width,
                       int32_t height) {
    WAFFLE_LOG(ERROR) << "wl_surface_interface::damage_buffer called "
                         "(not yet implemented)";
  },
};

WaylandSurface::WaylandSurface(wl_client* client,
                               uint32_t id,
                               int32_t version) {
  WAFFLE_LOG(TRACE) << "Creating WaylandSurface ...";

  auto impl = std::make_shared<Impl>();
  impl->resource_surface.Create(impl, client, id, &wl_surface_interface,
                                version, &Impl::kWlSurfaceInterface);
  impl_ = impl;
}

WaylandSurface WaylandSurface::GetSurfaceFrom(WaylandResource resource) {
  assert(resource.IsValid());

  WaylandSurface result;
  result.impl_ = resource.Get<Impl>();
  return result;
}

std::chrono::high_resolution_clock::time_point
    WaylandSurface::program_start_time_ =
        std::chrono::high_resolution_clock::now();

int WaylandSurface::TimeSinceProgramStartMillisecond() {
  auto usec =
      (std::chrono::duration_cast<std::chrono::duration<double>>(
           std::chrono::high_resolution_clock::now() - program_start_time_))
          .count();
  return usec * 1000;
}

void WaylandSurface::HandleFrameCallbacks() {
  // TODO: Don't call the callbacks if their windows are hidden.
  for (auto resource : Impl::callbacks) {
    if (resource.IsValid()) {
      if (resource.Version() >= WL_CALLBACK_DONE_SINCE_VERSION) {
        wl_callback_send_done(resource.Resource(),
                              TimeSinceProgramStartMillisecond());
      }
      resource.Destroy();
    }
  }

  Impl::callbacks.clear();
}

Texture WaylandSurface::GetTexture() {
  std::shared_ptr<Impl> impl = impl_.lock();
  if (!impl) {
    return Texture();
  }

  if (!impl->texture.Valid()) {
    impl->texture.Init();
  }

  return impl->texture;
}

std::weak_ptr<WaylandBindingHandlerDelegate> WaylandSurface::InputInterface() {
  return impl_;
}

}  // namespace waffle
