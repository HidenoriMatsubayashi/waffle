// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "waffle/wayland/wayland_region.h"

#include <wayland/protocols/wayland-server-protocol.h>

#include "waffle/logger.h"
#include "waffle/wayland/wayland_resource.h"

namespace waffle {

struct Area {
  virtual bool Contains(double x, double y) = 0;
};

struct RectArea : Area {
  double x_ = 0;
  double y_ = 0;
  double width_ = 0;
  double height_ = 0;

  RectArea(double x, double y, double width, double height)
      : x_(x), y_(y), width_(width), height_(height) {}

  bool Contains(double x, double y) {
    return (x >= x_ && y >= y_) && (x <= (x_ + width_) && y <= (y_ + height_));
  }
};

struct UnionArea : Area {
  std::unique_ptr<Area> a_;
  std::unique_ptr<Area> b_;

  UnionArea(std::unique_ptr<Area> a, std::unique_ptr<Area> b)
      : a_(std::move(a)), b_(std::move(b)) {}

  bool Contains(double x, double y) {
    return a_->Contains(x, y) || b_->Contains(x, y);
  }
};

struct IntersectionArea : Area {
  std::unique_ptr<Area> a_;
  std::unique_ptr<Area> b_;

  IntersectionArea(std::unique_ptr<Area> a, std::unique_ptr<Area> b)
      : a_(std::move(a)), b_(std::move(b)) {}

  bool Contains(double x, double y) {
    return a_->Contains(x, y) && b_->Contains(x, y);
  }
};

struct InverseArea : Area {
  std::unique_ptr<Area> a_;

  InverseArea(std::unique_ptr<Area> a) : a_(std::move(a)) {}

  bool Contains(double x, double y) { return !a_->Contains(x, y); }
};

struct WaylandRegion::Impl : WaylandResource::Data {
  std::unique_ptr<Area> data;
  WaylandResource resource;

  static const struct wl_region_interface region_interface;
};

const struct wl_region_interface WaylandRegion::Impl::region_interface {
  .destroy =
      +[](wl_client* client, wl_resource* resource) {
        WAFFLE_LOG(TRACE) << "wl_region_interface::destroy is called.";
        WaylandResource(resource).Destroy();
      },
  .add =
      +[](wl_client* client,
          wl_resource* resource,
          int32_t x,
          int32_t y,
          int32_t width,
          int32_t height) {
        WAFFLE_LOG(TRACE) << "wl_region_interface::add is called: "
                             "x = " +
                                 std::to_string(x) +
                                 ", y = " + std::to_string(y) +
                                 ", width = " + std::to_string(width) +
                                 ", height = " + std::to_string(height);

        auto impl = WaylandResource(resource).Get<Impl>();
        if (!impl) {
          WAFFLE_LOG(ERROR) << "Resouce is invalid.";
          return;
        }

        auto rect = std::make_unique<RectArea>(x, y, width, height);
        impl->data =
            std::make_unique<UnionArea>(std::move(impl->data), std::move(rect));
      },
  .subtract = +[](wl_client* client,
                  wl_resource* resource,
                  int32_t x,
                  int32_t y,
                  int32_t width,
                  int32_t height) {
    WAFFLE_LOG(TRACE) << "wl_region_interface::subtract is called: "
                         "x = " +
                             std::to_string(x) + ", y = " + std::to_string(y) +
                             ", width = " + std::to_string(width) +
                             ", height = " + std::to_string(height);

    auto impl = WaylandResource(resource).Get<Impl>();
    if (!impl) {
      WAFFLE_LOG(ERROR) << "Resouce is invalid.";
      return;
    }

    auto rect = std::make_unique<RectArea>(x, y, width, height);
    impl->data = std::make_unique<IntersectionArea>(
        std::move(impl->data), std::make_unique<InverseArea>(std::move(rect)));
  }
};

WaylandRegion::WaylandRegion(wl_client* client, uint32_t id, uint version) {
  WAFFLE_LOG(TRACE) << "Creating WaylandRegion ...";

  auto impl = std::make_shared<Impl>();
  impl->resource.Create(impl, client, id, &wl_region_interface, version,
                        &Impl::region_interface);
  impl_ = impl;
}

}  // namespace waffle
