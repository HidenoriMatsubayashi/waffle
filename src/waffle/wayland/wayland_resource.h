// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WAFFLE_WAYLAND_WAYLAND_RESOURCE_H_
#define WAFFLE_WAYLAND_WAYLAND_RESOURCE_H_

#include <wayland-server-core.h>

#include <memory>

namespace waffle {

class WaylandResource {
 public:
  class Data {};

  WaylandResource() = default;
  explicit WaylandResource(wl_resource* resource);

  void Create(std::shared_ptr<Data> data,
              wl_client* client,
              uint32_t id,
              const wl_interface* interface,
              int32_t version,
              const void* implementation);

  void Destroy();

  bool IsNull() { return impl_.expired(); };

  bool IsValid() { return !IsNull(); };

  int32_t Version();

  wl_resource* Resource();

  template <typename T>
  inline std::shared_ptr<T> Get() {
    return std::static_pointer_cast<T>(GetData());
  }

 private:
  std::shared_ptr<Data> GetData();

  struct Impl;
  std::weak_ptr<Impl> impl_;
};

}  // namespace waffle

#endif  // WAFFLE_WAYLAND_WAYLAND_RESOURCE_H_
