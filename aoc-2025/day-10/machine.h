#pragma once
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <execution>
#include <functional>
#include <iterator>
#include <memory>
#include <numeric>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <vector>

typedef uint32_t btn_state_t;
typedef int32_t joltage_t;
typedef std::vector<joltage_t> joltages_t;

class Machine {
public:
  Machine(btn_state_t target_state, const std::vector<btn_state_t> &buttons,
          const joltages_t &joltages)
      : target_state_(target_state), buttons_(buttons), joltages_(joltages) {}

  inline joltages_t const &joltages() const { return joltages_; }
  inline size_t joltage_size() const { return joltages_.size(); }
  inline size_t button_size() const { return buttons_.size(); }
  inline int joltage_at(size_t idx) const { return joltages_[idx]; }
  inline btn_state_t target_state() const { return target_state_; }
  inline btn_state_t press(btn_state_t state, size_t btn_idx) const {
    return state ^ buttons_[btn_idx];
  }

  static Machine from_line(const char *line);
  std::string to_string() const;

  operator std::string() const { return to_string(); }

  Machine fit_to_joltage() const;

  joltage_t joltage_by_buttons(btn_state_t btn_presses,
                               size_t joltage_idx) const {
    joltage_t result{0};

    auto btn_mask = btn_presses;
    for (auto btn : buttons_) {
      result += btn_mask & 1 & (btn >> joltage_idx);
      btn_mask >>= 1;
    }
    return result;
  }
  joltages_t joltages_by_buttons(btn_state_t btn_presses) const {
    size_t n = joltage_size();
    joltages_t result(n, 0);
    for (size_t i = 0; i < n; i++) {
      result[i] = joltage_by_buttons(btn_presses, i);
    }
    return result;
  }

private:
  btn_state_t target_state_;
  std::vector<btn_state_t> buttons_;
  joltages_t joltages_;
};

inline int joltage_cmp(const joltages_t &&a, const joltages_t &&b) {
  if (auto delta = a.size() - b.size(); delta != 0) {
    return delta;
  }

  auto ita = a.cbegin();
  auto itb = b.cbegin();
  while (ita != a.cend()) {
    if (auto delta = *ita++ - *itb++; delta != 0) {
      return delta;
    }
  }
  return 0;
}

// subtract joltage data, return nullopt if any value would go negative
inline std::optional<joltages_t> joltage_sub(const joltages_t &a,
                                             const joltages_t &b) {
  joltages_t result(a.size(), 0);
  std::transform(std::execution::par, a.cbegin(), a.cend(), b.cbegin(),
                 result.begin(),
                 [](joltage_t ja, joltage_t jb) { return ja - jb; });
  if (std::any_of(std::execution::par, result.cbegin(), result.cend(),
                  [](joltage_t j) { return j < 0; })) {
    return {};
  }
  return result;
}

inline btn_state_t joltages_to_button_state(const joltages_t &joltages) {
  btn_state_t bitmap{0};

  size_t i = 0;
  for (const auto &joltage : joltages) {
    bitmap |= ((static_cast<btn_state_t>(joltage) & 1) << i++);
  }

  return bitmap;
}

inline joltages_t joltages_half(const joltages_t &joltages) {
  joltages_t result(joltages.size(), 0);

  std::transform(std::execution::par, joltages.cbegin(), joltages.cend(),
                 result.begin(), [](joltage_t j) { return j >> 1; });

  return result;
}

inline bool joltages_is_zero(const joltages_t &joltages) {
  return std::transform_reduce(std::execution::par, joltages.cbegin(),
                               joltages.cend(), true, std::logical_and<>{},
                               [](joltage_t j) { return j == 0; });
}
