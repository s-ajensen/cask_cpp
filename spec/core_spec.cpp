#include "bdd.hpp"
#include "catch.hpp"
#include "core.hpp"
#include "time_helper.hpp"

class HaltEngine : public Steppable<int>, public Renderable<int> {
public:
  const int cycles;
  mutable std::vector<int> updates;
  mutable std::vector<int> renders;

  explicit HaltEngine(int cycles) : cycles(cycles) { }

  State<int> next_state(const int& state) const override {
    if (state >= this->cycles) {
      return Halt();
    }

    updates.push_back(state);
    return state + 1;
  }

  void render(const int& state) const override {
    renders.push_back(state);
  }
};

describe("Cask Core") {

  context("game loop") {

    it("steps once")  {
      auto clock = FakeClock<5>({1, 2, 3, 4, 5});
      auto engine = HaltEngine(1);
      game_loop(clock, engine, 1, 0);
      should_eq(0, engine.updates.at(0));
      should_eq(1, engine.updates.size());
    }

    it("steps twice") {
      auto clock = FakeClock<5>({1, 2, 3, 4, 5});
      auto engine = HaltEngine(2);
      game_loop(clock, engine, 1, 0);
      should_eq(1, engine.updates.at(1));
      should_eq(2, engine.updates.size());
    }

    it("renders once") {
      auto clock = FakeClock<2>({1, 2});
      auto engine = HaltEngine(2);
      game_loop(clock, engine, 1, 0);
      should_eq(1, engine.renders.at(0));
      should_eq(1, engine.renders.size());
    }

    it("logic and rendering perfectly in sync") {
      auto clock = FakeClock<7>({1, 2, 3, 4, 5, 6, 7});
      auto engine = HaltEngine(5);
      game_loop(clock, engine, 1, 0);
      should_eq(5, engine.updates.size());
      should_eq(5, engine.renders.size());
    }

    it("logic twice as fast as rendering") {
      auto clock = FakeClock<7>({2, 4, 6, 8, 10, 12, 14});
      auto engine = HaltEngine(6);
      game_loop(clock, engine, 1, 0);
      should_eq(6, engine.updates.size());
      should_eq(3, engine.renders.size());
    }
  }
}