// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "waffle/wayland/wayland_resource.h"

#include <cassert>
#include <unordered_map>

#include "waffle/logger.h"

namespace waffle {

struct WaylandResource::Impl {
  wl_resource* resource = nullptr;
  std::shared_ptr<Data> data = nullptr;
  int32_t version = 0;

  static std::unordered_map<wl_resource*, std::shared_ptr<Impl>> umap;

  static void Destroy(wl_resource* resource) {
    auto wayland_resource = WaylandResource(resource);
    auto impl = wayland_resource.impl_.lock();
    if (!impl) {
      WAFFLE_LOG(ERROR) << "Unexpected error happened. Resource is invalid: "
                        << resource;
      return;
    }

    auto itr = umap.find(impl->resource);
    if (itr != Impl::umap.end()) {
      Impl::umap.erase(itr);
      impl->resource = nullptr;
      impl->data = nullptr;
    } else {
      WAFFLE_LOG(ERROR) << "Resouce is not found: " << resource;
    }
  }
};

std::unordered_map<wl_resource*, std::shared_ptr<WaylandResource::Impl>>
    WaylandResource::Impl::umap;

WaylandResource::WaylandResource(wl_resource* resource) {
  auto itr = Impl::umap.find(resource);
  if (itr == Impl::umap.end()) {
    WAFFLE_LOG(ERROR) << "Resouce is not found: " << resource;
    return;
  }
  impl_ = itr->second;
}

void WaylandResource::Create(std::shared_ptr<Data> data,
                             wl_client* client,
                             uint32_t id,
                             const wl_interface* interface,
                             int32_t version,
                             const void* implementation) {
  auto* resource = wl_resource_create(client, interface, version, id);
  if (Impl::umap.find(resource) != Impl::umap.end()) {
    WAFFLE_LOG(WARNING) << "Client made same resource that has already made.";
  }

  auto impl = std::make_shared<Impl>();
  Impl::umap[resource] = impl;
  wl_resource_set_implementation(resource, implementation, nullptr,
                                 Impl::Destroy);
  impl->resource = resource;
  impl->version = version;
  impl->data = data;
  impl_ = impl;
}

void WaylandResource::Destroy() {
  wl_resource_destroy(impl_.lock()->resource);
}

wl_resource* WaylandResource::Resource() {
  return impl_.lock()->resource;
}

int32_t WaylandResource::Version() {
  return impl_.lock()->version;
}

std::shared_ptr<WaylandResource::Data> WaylandResource::GetData() {
  return impl_.lock()->data;
}

}  // namespace waffle
