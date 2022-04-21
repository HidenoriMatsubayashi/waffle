// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <chrono>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include "waffle/compositor/compositor.h"
#include "waffle/wayland_server.h"

int main(int argc, char** argv) {
  waffle::WaffleWindowProperties properties = {
      .width = 1920,
      .height = 1080,
      .view_mode = waffle::WaffleViewMode::kFullscreen,
      .view_rotation = waffle::WaffleViewRotation::kRotation_0,
      .use_mouse_cursor = true,
      .background_image_filepath = "../assets/system-bg.png",
  };
  auto server = std::make_unique<waffle::WaylandServer>();
  waffle::Compositor::Create(server->Display(), properties);
  auto* compositor = waffle::Compositor::Instance();

  // Main loop.
  auto next_waffle_event_time =
      std::chrono::steady_clock::time_point::clock::now();
  auto running = true;
  while (running) {
    // Wait until the next event.
    {
      auto wait_duration =
          std::max(std::chrono::nanoseconds(0),
                   next_waffle_event_time -
                       std::chrono::steady_clock::time_point::clock::now());
      std::this_thread::sleep_for(
          std::chrono::duration_cast<std::chrono::milliseconds>(wait_duration));
    }

    server->HandleEvent();
    compositor->Draw();
    running = compositor->HandleEvent();

    {
      auto next_event_time = std::chrono::steady_clock::time_point::max();
      auto frame_rate = compositor->GetFrameRate();
      next_event_time = std::min(
          next_event_time, std::chrono::steady_clock::time_point::clock::now() +
                               std::chrono::milliseconds(static_cast<int>(
                                   std::trunc(1000000.0 / frame_rate))));
      next_waffle_event_time =
          std::max(next_waffle_event_time, next_event_time);
    }
  }
  compositor->Destroy();

  return 0;
}
