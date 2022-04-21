// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WAFFLE_WAYLAND_WAYLAND_ARRAY_H_
#define WAFFLE_WAYLAND_WAYLAND_ARRAY_H_

#include <wayland-util.h>

#include <memory>

namespace waffle {

template <typename T>
class WaylandArray {
 public:
  WaylandArray() {
    array_ = std::make_unique<wl_array>();
    wl_array_init(array_.get());
  }

  ~WaylandArray() {
    if (array_) {
      wl_array_release(array_.get());
    }
  }

  wl_array* Array() { return &*array_; }

  void Append(T item) {
    auto itr = static_cast<T*>(wl_array_add(array_.get(), sizeof(T)));
    *itr = item;
  }

 private:
  std::unique_ptr<wl_array> array_ = nullptr;
};

}  // namespace waffle

#endif  // WAFFLE_WAYLAND_WAYLAND_ARRAY_H_
