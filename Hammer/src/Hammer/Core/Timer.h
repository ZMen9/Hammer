#pragma once
#include <chrono>

namespace hammer {
class Timer {
 public:
  Timer() { Reset(); }
  void Reset() { start_ = std::chrono::steady_clock::now();
  }
  float Elapsed() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
               std::chrono::steady_clock::now() - start_)
               .count() *
           0.001f * 0.001f * 0.001f;
  }
  float ElapsedMillis() { return Elapsed() * 1000.0f;
  }
 private:
  std::chrono::time_point<std::chrono::steady_clock> start_;
};
}