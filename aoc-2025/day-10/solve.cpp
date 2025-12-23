#include "machine.h"

#include <algorithm>
#include <bit>
#include <cstdio>
#include <queue>

struct press_t {
  btn_state_t state;       // state of the switches after presses
  btn_state_t button_mask; // track which button has been pressed.

  // get number of presses
  size_t get_presses() const { return std::popcount(button_mask); }
};

struct joltage_press_t {
  size_t total_presses;       // total number of presses of buttons
  uint32_t next_press_shifts; // number of shifts for next press
  joltages_t joltages;        // remaining joltage data
};

template <typename T>
bool iter_solutions(const Machine &machine, btn_state_t target_state,
                    T callback)
  requires std::invocable<T, const press_t &> &&
           std::convertible_to<std::invoke_result_t<T, const press_t &>, bool>
{
  // Breadth-first search, so we can find minimal presses.
  auto button_size = machine.button_size();
  std::queue<press_t> explore{};
  explore.push({0, 0});

  while (!explore.empty()) {
    auto current = explore.front();
    explore.pop();

    for (size_t btn_idx = std::bit_width(current.button_mask);
         btn_idx < button_size; btn_idx++) {
      if ((current.button_mask & (1 << btn_idx)) == 0) {
        auto state = machine.press(current.state, btn_idx);
        auto button_mask = current.button_mask | (1 << btn_idx);
        press_t updated_press_state = {.state = state,
                                       .button_mask = button_mask};
        if (state == target_state && callback(updated_press_state)) {
          return true;
        }
        explore.push(updated_press_state);
      }
    }
  }

  return false;
}

size_t solve_p1(const Machine &machine) {
  int presses = 0;
  iter_solutions(machine, machine.target_state(),
                 [&](const press_t &press) -> bool {
                   if (press.state == machine.target_state()) {
                     presses = press.get_presses();
                     return true;
                   }
                   return false;
                 });

  return presses;
}

size_t solve_p2(const Machine &machine) {
  // implementing the algorithm described in:
  //   https://old.reddit.com/r/adventofcode/comments/1pk87hl/

  size_t result = SIZE_MAX;
  const auto &joltages = machine.joltages();

  std::queue<joltage_press_t> explore{};
  explore.push(
      {.total_presses = 0, .next_press_shifts = 0, .joltages = joltages});

  while (!explore.empty()) {
    auto current = explore.front();
    explore.pop();

    auto target_state = joltages_to_button_state(current.joltages);
    if (target_state == 0) {
      // when the joltages are all zero (even),
      // we also expore the solution when this has been halved.
      auto next =
          joltage_press_t{.total_presses = current.total_presses,
                          .next_press_shifts = current.next_press_shifts + 1,
                          .joltages = joltages_half(current.joltages)};
      explore.push(next);
    }
    iter_solutions(machine, target_state, [&](const press_t &press) -> bool {
      auto joltage_to_subtract = machine.joltages_by_buttons(press.button_mask);
      auto rem = joltage_sub(current.joltages, joltage_to_subtract);
      if (rem) {
        auto total_presses = current.total_presses +
                             (press.get_presses() << current.next_press_shifts);

#ifndef NDEBUG
        std::stringstream rem_ss;
        rem_ss << "(";
        for (auto j : *rem) {
          rem_ss << j << ",";
        }
        rem_ss.seekp(-1, std::ios_base::cur);
        rem_ss << ")";
        printf("presses: %zu (total %zu) rem %s\n", press.get_presses(),
               total_presses, rem_ss.str().c_str());
#endif

        if (joltages_is_zero(*rem)) {
#ifndef NDEBUG
          printf("found solution with %zu presses\n", total_presses);
#endif
          result = std::min(total_presses, result);
        } else {
          auto next = joltage_press_t{.total_presses = total_presses,
                                      .next_press_shifts =
                                          current.next_press_shifts + 1,
                                      .joltages = joltages_half(*rem)};
          explore.push(next);
        }
      }

      return false;
    });
  }

  return result;
}

int main() {
  std::vector<Machine> machines;
  {
    char line[256];
    while (fgets(line, sizeof(line) - 1, stdin)) {
      machines.push_back(Machine::from_line(line));
    }
  }

  size_t p1 = 0;
  for (auto &machine : machines) {
    p1 += solve_p1(machine);
  }
  printf("p1: %zu\n", p1);

  // handle p2
  for (auto it = machines.begin(); it != machines.end(); ++it) {
    *it = it->fit_to_joltage();
  }
  size_t p2 = 0;
  for (auto &machine : machines) {
    auto p2_part = solve_p2(machine);
    p2 += p2_part;
#ifndef NDEBUG
    printf("p2 machine: %zu\n", p2_part);
#endif
  }
  printf("p2: %zu\n", p2);

  return 0;
}
