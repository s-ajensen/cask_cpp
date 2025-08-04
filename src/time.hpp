#pragma once

#include <concepts>
#include <functional>
#include <chrono>

template<typename T>
concept Clock = requires(T t) {
  { t.now() } -> std::convertible_to<typename T::time_point>;
};

struct SteadyClock {
  using time_point = std::chrono::steady_clock::time_point;
  
  time_point now() {
    return std::chrono::steady_clock::now();
  }
};

template<Clock ClockType>
uint64_t millis_since_epoch(const ClockType& clock) {
  auto now = clock.now().time_since_epoch();
  auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(now);
  return millis.count();
}