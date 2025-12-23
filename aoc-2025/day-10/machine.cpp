// Parse machine from line to internal states of a Machine,
//   and be able to serialize back to string.

#include "machine.h"

#include <algorithm>
#include <cstring>

enum class PARSE_STATE {
  START,
  READ_TARGET_STATE,
  READ_BUTTON_OR_JOLTAGES,
  READ_BUTTONS,
  READ_JOLTAGES,
  END,
};

Machine Machine::from_line(const char *line) {
  const char *p = line;
  const char *end = p + strlen(line);

  int target_state_shifts = 0;
  btn_state_t target_state = 0;
  btn_state_t temp_button = 0;
  std::vector<btn_state_t> buttons;
  std::vector<int> joltages;
  int temp = 0;

  auto state = PARSE_STATE::START;
  for (; p < end && state != PARSE_STATE::END; p++) {
    char c = *p;
    // ignore whitespace
    if (c == ' ' || c == '\n' || c == '\r' || c == '\t') {
      continue;
    }

    switch (state) {
    case PARSE_STATE::START:
      if (c == '[') {
        state = PARSE_STATE::READ_TARGET_STATE;
      }
      break;

    case PARSE_STATE::READ_TARGET_STATE:
      if (c != ']') {
        int bitmask = c == '#' ? 1 : 0;
        target_state |= bitmask << target_state_shifts++;
        break;
      }
      state = PARSE_STATE::READ_BUTTON_OR_JOLTAGES;
      break;

    case PARSE_STATE::READ_BUTTON_OR_JOLTAGES:
      if (c == '(') {
        state = PARSE_STATE::READ_BUTTONS;
        break;
      } else if (c == '{') {
        state = PARSE_STATE::READ_JOLTAGES;
        break;
      }
      break;

    case PARSE_STATE::READ_BUTTONS: {
      if (c == ')' || c == ',') {
        temp_button |= 1 << temp;
        temp = 0;

        if (c == ')') {
          buttons.push_back(temp_button);
          temp_button = 0;
          state = PARSE_STATE::READ_BUTTON_OR_JOLTAGES;
        }
        break;
      }

      if (c >= '0' && c <= '9') {
        temp = temp * 10 + (c - '0');
        break;
      }

      throw std::runtime_error("Invalid character in button definition");
    }

    case PARSE_STATE::READ_JOLTAGES: {
      if (c == '}' || c == ',') {
        joltages.push_back(temp);
        temp = 0;

        if (c == '}') {
          state = PARSE_STATE::END;
        }
        break;
      }

      if (c >= '0' && c <= '9') {
        temp = temp * 10 + (c - '0');
        break;
      }
      throw std::runtime_error("Invalid character in joltage definition");
    }

    case PARSE_STATE::END:
      break;
    } // switch
  } // for

  return {target_state, buttons, joltages};
}

std::string Machine::to_string() const {
  std::stringstream ss;
  ss << "[";
  for (size_t i = 0; i < button_size(); i++) {
    ss << ((target_state_ & (1 << i)) ? '#' : '.');
  }
  ss << "] ";
  for (const auto &btn : buttons_) {
    ss << "(";
    bool add_comma = false;
    for (size_t i = 0; i < button_size(); i++) {
      if (btn & (1 << i)) {
        if (add_comma) {
          ss << ",";
        }
        ss << i;
        add_comma = true;
      }
    }
    ss << ") ";
  }
  if (!joltages_.empty()) {
    ss << "{";
    bool add_comma = false;
    for (const auto &joltage : joltages_) {
      if (add_comma) {
        ss << ",";
      }
      ss << joltage;
      add_comma = true;
    }
    ss << "}";
  }
  return ss.str();
}

Machine Machine::fit_to_joltage() const {
  btn_state_t mask = (btn_state_t{1} << joltages_.size()) - 1;
  auto buttons = buttons_;
  for (auto &btn : buttons) {
    btn &= mask;
  }
  buttons.erase(std::remove_if(buttons.begin(), buttons.end(),
                               [](btn_state_t btn) { return btn == 0; }),
                buttons.end());
  return Machine(target_state_ & mask, buttons, joltages_);
}
