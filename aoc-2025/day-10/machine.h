#pragma once
#include <cstdint>
#include <cstdio>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <vector>

typedef uint32_t btn_state_t;

class Machine {
public:
  Machine(btn_state_t target_state, const std::vector<btn_state_t> &buttons,
          const std::vector<int> &joltages)
      : target_state_(target_state), buttons_(buttons), joltages_(joltages) {}

  inline size_t size() const { return joltages_.size(); }
  inline int joltage_at(size_t idx) const { return joltages_[idx]; }
  inline btn_state_t target_state() const { return target_state_; }
  inline btn_state_t button_at(size_t idx) const { return buttons_[idx]; }
  inline btn_state_t press(btn_state_t state, size_t btn_idx) const {
    return state ^ buttons_[btn_idx];
  }

  static Machine from_line(const char *line);
  std::string to_string() const;

  operator std::string() const { return to_string(); }

private:
  btn_state_t target_state_;
  std::vector<btn_state_t> buttons_;
  std::vector<int> joltages_;
};
