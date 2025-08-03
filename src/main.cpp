#include <chrono>
#include <functional>
#include <iostream>
#include <cmath>

template<typename T>
concept State = requires(T t)
{
  std::copyable<T>;
};

template<typename T, typename S>
concept Steppable = State<S> && requires(T t, S s)
{
  { t.setup(s) } -> std::same_as<S>;
  { t.next_state(s) } -> std::same_as<S>;
};

template<typename Engine>
  requires Steppable<Engine, typename Engine::State>
void game_loop(Engine &engine, typename Engine::State initial = {}) {
  auto state = engine.setup(initial);
  for (int i = 0; i < 10000; ++i) {
    state = engine.next_state(state);
  }
}

struct CaskEngine {
  using State = double;

  static State setup(State s) {
    return 0;
  }

  static State next_state(State s) {
    for (int i = 0; i < 50 + (50 * std::sin(s)); ++i) {
      std::cout << '*';
    }
    std::cout << std::endl;
    return s + 0.1;
  }
};

// Event Handler Sandbox

template<typename T>
class EventChannel {
public:
  using Handler = std::function<void(const T&)>;
  std::vector<Handler> handlers;

  void subscribe(Handler handler) {
    handlers.push_back(std::move(handler));
  }

  void dispatch(const T& event) const {
    for (const auto& handler: handlers) {
      handler(event);
    }
  }
};

class EventDispatcher {
public:
  template<typename T>
  static EventChannel<T>& getChannel() {
    static EventChannel<T> channel;
    return channel;
  }

  template<typename T, typename Handler>
  void subscribe(Handler handler) {
    getChannel<T>().subscribe(handler);
  }

  template<typename T>
  void dispatch(const T& event) {
    getChannel<T>().dispatch(event);
  }
};

int main() {
  EventDispatcher dispatcher;

  dispatcher.subscribe<int>([](const int& i) {
    std::cout << "blah" << i << std::endl;
  });

  dispatcher.dispatch(123);

  return 0;
}