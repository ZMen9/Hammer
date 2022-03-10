#pragma once

namespace hammer {
class Timestep {
 public:
  Timestep(float time = 0.0f) : time_(time) {}

  operator float() const { return time_; }

  inline float GetSeconds() const { return time_; }
  inline float GetMilliseconds() const { return time_ * 1000.0f; }
 private:
  float time_;
};
} // namespace hammer