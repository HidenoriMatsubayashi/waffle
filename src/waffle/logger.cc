// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Copyright 2021 Sony Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "waffle/logger.h"

#include <cstring>
#include <unordered_map>

namespace waffle {

namespace {

constexpr char kWaffleLogLevelsEnvironmentKey[] = "WAFFLE_LOG_LEVELS";
constexpr char kWaffleLogLevelTrace[] = "TRACE";
constexpr char kWaffleLogLevelDebug[] = "DEBUG";
constexpr char kWaffleLogLevelInfo[] = "INFO";
constexpr char kWaffleLogLevelWarning[] = "WARNING";
constexpr char kWaffleLogLevelError[] = "ERROR";
constexpr char kWaffleLogLevelFatal[] = "FATAL";
constexpr char kWaffleLogLevelUnknown[] = "UNKNOWN";

const char* const kLogLevelNames[WAFFLE_LOG_NUM] = {
    kWaffleLogLevelTrace,   kWaffleLogLevelDebug, kWaffleLogLevelInfo,
    kWaffleLogLevelWarning, kWaffleLogLevelError, kWaffleLogLevelFatal};

const std::unordered_map<std::string, int> gLogLevelsMap{
    {kWaffleLogLevelTrace, WAFFLE_LOG_TRACE},
    {kWaffleLogLevelDebug, WAFFLE_LOG_DEBUG},
    {kWaffleLogLevelInfo, WAFFLE_LOG_INFO},
    {kWaffleLogLevelWarning, WAFFLE_LOG_WARNING},
    {kWaffleLogLevelError, WAFFLE_LOG_ERROR},
    {kWaffleLogLevelFatal, WAFFLE_LOG_FATAL},
};

int gFilterLogLevel = -1;

int GetCurrentLogLevel() {
  if (gFilterLogLevel == -1) {
    auto env_log_level = std::getenv(kWaffleLogLevelsEnvironmentKey);
    if (!env_log_level || (env_log_level[0] == '\0')) {
      gFilterLogLevel = WAFFLE_LOG_WARNING;
    } else {
      if (gLogLevelsMap.find(env_log_level) != gLogLevelsMap.end()) {
        gFilterLogLevel = gLogLevelsMap.at(env_log_level);
      } else {
        gFilterLogLevel = WAFFLE_LOG_WARNING;
      }
    }
  }
  return gFilterLogLevel;
}

const char* GetLogLevelName(int level) {
  if (WAFFLE_LOG_TRACE <= level && level < WAFFLE_LOG_NUM)
    return kLogLevelNames[level];
  return kWaffleLogLevelUnknown;
}

}  // namespace

Logger::Logger(int level, const char* file, int line) : level_(level) {
  if (level_ < GetCurrentLogLevel()) {
    return;
  }

  stream_ << "[" << GetLogLevelName(level_) << "]";
  stream_ << "[" << file << "(" << line << ")] ";
}

Logger::~Logger() {
  if (level_ < GetCurrentLogLevel()) {
    return;
  }

  stream_ << std::endl;
  std::cerr << stream_.str();
  std::cerr.flush();
  if (level_ >= WAFFLE_LOG_FATAL) {
    abort();
  }
}

}  // namespace waffle
