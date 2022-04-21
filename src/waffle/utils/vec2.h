// Copyright 2022 Hidenori Matsubayashi All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WAFFLE_UTILS_VEC2_H_
#define WAFFLE_UTILS_VEC2_H_

namespace waffle {

template <typename T>
class Vec2 {
 public:
  Vec2() : x_(0), y_(0) {}
  Vec2(T x, T y) : x_(x), y_(y) {}
  ~Vec2() = default;

  T X() const { return x_; }
  T Y() const { return y_; }

  inline bool operator==(Vec2 v);

  inline Vec2& operator+=(Vec2& v);
  inline Vec2& operator-=(Vec2& v);
  inline Vec2& operator*=(Vec2& v);
  inline Vec2& operator/=(Vec2& v);

  inline Vec2 operator+(Vec2 v);
  inline Vec2 operator-(Vec2 v);
  inline Vec2 operator*(Vec2 v);
  inline Vec2 operator/(Vec2 v);

 private:
  T x_, y_;
};

}  // namespace waffle

#endif  // WAFFLE_UTILS_VEC2_H_
