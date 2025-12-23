#include "machine.h"

int main() {

  std::vector<Machine> machines;
  {
    char line[256];
    while (fgets(line, sizeof(line) - 1, stdin)) {
      machines.push_back(Machine::from_line(line));
    }
  }

  // test my parser
  for (size_t i = 0; i < machines.size(); i++) {
    auto str = machines[i].to_string();
    printf("Machine %zu: %s\n", i, str.c_str());
  }
  return 0;
}