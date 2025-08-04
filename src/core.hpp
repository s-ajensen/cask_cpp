#pragma once

#include "time.hpp"
#include <variant>

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

template <typename T, Clock ClockType, SteppableAndRenderable<T> Engine>
void game_loop(const ClockType& clock, const Engine& engine, uint64_t ms_per_update, const T& initial_state) {
  
  State<T> current_state = engine.next_state(initial_state);

  uint64_t prev_ms = millis_since_epoch(clock);
  uint64_t current_ms = prev_ms;
  uint64_t lag_ms = 0;

  while(std::holds_alternative<T>(current_state)) {
    const T& state = std::get<T>(current_state);
    engine.render(state);
    current_state = engine.next_state(state);
  }
}

template<typename T, SteppableAndRenderable<T> Engine>
void game_loop(const Engine& engine, const T& initial_state) {
  SteadyClock clock;
  game_loop(clock, engine, initial_state);
}