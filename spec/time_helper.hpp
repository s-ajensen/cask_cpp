#pragma once

#include "time.hpp"

template<size_t N>
class FakeClock {
public:
    using time_point = std::chrono::steady_clock::time_point;
private:
    mutable std::array<time_point, N> _times;
    mutable size_t index = 0;
public:
    FakeClock(const std::array<int64_t, N>& times) {
        for (size_t i = 0; i < N; ++i) {
            _times[i] = time_point{std::chrono::milliseconds{times[i]}};
        }
    }

    time_point now() const {
        if (index >= _times.size()) {
            index = 0;
        }
        return _times[index++];
    }
};