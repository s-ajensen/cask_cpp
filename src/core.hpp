#pragma once
#include <variant>

struct Halt {};

template<typename T>
using State = std::variant<T, Halt>;

template<typename T>
class Steppable {
public:
  virtual ~Steppable() = default;
  virtual State<T> next_state(const T& state) = 0;
};

template<typename T>
class Renderable {
public:
  virtual ~Renderable() = default;
  virtual void render(const T& state) = 0;
};