// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Copyright 2021 Sony Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WAFFLE_LOGGER_H_
#define WAFFLE_LOGGER_H_

#include <string.h>

#include <iostream>
#include <sstream>

namespace waffle {

constexpr int WAFFLE_LOG_TRACE = 0;
constexpr int WAFFLE_LOG_DEBUG = 1;
constexpr int WAFFLE_LOG_INFO = 2;
constexpr int WAFFLE_LOG_WARNING = 3;
constexpr int WAFFLE_LOG_ERROR = 4;
constexpr int WAFFLE_LOG_FATAL = 5;
constexpr int WAFFLE_LOG_NUM = 6;

#if defined(ENABLE_WAFFLE_LOG)
#if defined(NDEBUG)
// We don't use __FILE__ macro with release build.
#define WAFFLE_LOG(level) \
  Logger(WAFFLE_LOG_##level, __FUNCTION__, __LINE__).stream()
#else
#define __LOG_FILE_NAME__ \
  (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define WAFFLE_LOG(level) \
  Logger(WAFFLE_LOG_##level, __LOG_FILE_NAME__, __LINE__).stream()
#endif
#else
#define WAFFLE_LOG(level) Logger(-1, "", 0).stream()
#endif

class Logger {
 public:
  Logger(int level, const char* file, int line);
  ~Logger();

  std::ostream& stream() { return stream_; }

 private:
  const int level_;
  std::ostringstream stream_;
};

}  // namespace waffle

#endif  // WAFFLE_LOGGER_H_
