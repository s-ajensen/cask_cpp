#pragma once

#include "time.hpp"
#include <variant>
#include <iostream>

struct Halt {};

template<typename T>
using State = std::variant<T, Halt>;

template<typename T>
class Steppable {
public:
  virtual ~Steppable() = default;
  virtual State<T> next_state(const T& state) const = 0;
};

template<typename T>
class Renderable {
public:
  virtual ~Renderable() = default;
  virtual void render(const T& state) const = 0;
};

template<typename Engine, typename T>
concept SteppableAndRenderable =
  std::is_base_of_v<Steppable<T>, Engine> &&
  std::is_base_of_v<Renderable<T>, Engine>;

template<typename T, SteppableAndRenderable<T> Engine>
std::tuple<State<T>, uint64_t> fast_forward(const Engine& engine, const T& state, uint64_t lag_ms, uint64_t ms_per_update) {
  State<T> next_state = state;
  while(std::holds_alternative<T>(next_state) && (lag_ms >= ms_per_update)) {
    next_state = engine.next_state(std::get<T>(next_state));
    lag_ms -= ms_per_update;
  }
  return {next_state, lag_ms};
}

template<typename T, Clock ClockType, SteppableAndRenderable<T> Engine>
void game_loop(const ClockType& clock, const Engine& engine, uint64_t ms_per_update, T initial_state) {
  
  State<T> current_state = std::move(initial_state);

  uint64_t prev_ms = millis_since_epoch(clock);
  uint64_t lag_ms = 0;

  while(std::holds_alternative<T>(current_state)) {
    const T& state = std::get<T>(current_state);

    uint64_t current_ms = millis_since_epoch(clock);
    uint64_t elapsed = current_ms - prev_ms;
    prev_ms = current_ms;
    lag_ms += elapsed;

    auto [new_state, new_lag] = fast_forward(engine, state, lag_ms, ms_per_update);
    current_state = std::move(new_state);
    lag_ms = new_lag;

    if(std::holds_alternative<T>(current_state)) {
      engine.render(std::get<T>(current_state));
    }
  }
}

template<typename T, SteppableAndRenderable<T> Engine>
void game_loop(const Engine& engine, const T& initial_state) {
  SteadyClock clock;
  game_loop(clock, engine, initial_state);
}