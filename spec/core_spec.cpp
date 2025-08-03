#include "catch.hpp"
#include "bdd.hpp"
#include "core.hpp"

class HaltEngine : public Steppable<int>, public Renderable<int> {
public:
  const int cycles;
  std::vector<int> updates;
  std::vector<int> renders;

  explicit HaltEngine(int cycles) : cycles(cycles) { }

  State<int> next_state(const int& state) override {
    updates.push_back(state);
    return state + 1;
  }

  void render(const int& state) override {
    renders.push_back(state);
  }
};

describe("Cask Core") {
  context("game loop") {
    it("huh?")  {
      auto e = HaltEngine(1);
      e.next_state(0);
      should_eq(0, e.updates.at(0));
    }
  }
}