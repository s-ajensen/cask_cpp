#pragma once

#include "time.hpp"
#include <variant>
#include <iostream>

// TODO (AJ) - implement CRTP
template<typename State>
class Steppable {
public:
  virtual ~Steppable() = default;
  virtual State next_state(const State& state) const = 0;
};

template<typename T>
class Renderable {
public:
  virtual ~Renderable() = default;
  virtual void render(const T& state) const = 0;
};

template<typename T>
class Haltable {
public:
  virtual ~Haltable() = default;
  virtual bool is_halted(const T& state) const = 0;
};

template<typename E, typename T>
concept Engine =
  std::is_base_of_v<Steppable<T>, E> &&
  std::is_base_of_v<Renderable<T>, E> &&
  std::is_base_of_v<Haltable<T>, E>;

template<typename T, Clock ClockType, Engine<T> E>
void game_loop(const ClockType& clock, const E& engine, uint64_t ms_per_update, const T& initial_state) {
  
  T current_state = initial_state;

  uint64_t prev_ms = millis_since_epoch(clock);
  uint64_t lag_ms = 0;

  while(!engine.is_halted(current_state)) {
    engine.render(current_state);
    uint64_t current_ms = millis_since_epoch(clock);
    uint64_t elapsed = current_ms - prev_ms;
    prev_ms = current_ms;
    lag_ms += elapsed;

    while(!engine.is_halted(current_state) && (lag_ms >= ms_per_update)) {
      current_state = engine.next_state(current_state);
      lag_ms -= ms_per_update;
    }
  }
}

template<typename T, Engine<T> E>
void game_loop(const E& engine, uint64_t ms_per_update, const T& initial_state) {
  SteadyClock clock;
  game_loop(clock, engine, ms_per_update, initial_state);
}