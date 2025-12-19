#include <algorithm>
#include <cassert>
#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <vector>

#define MAX_RANGE_LIST (400)
#define MAX_ITEMS_LIST (1000)

inline uint64_t u64_min(uint64_t a, uint64_t b) { return a < b ? a : b; }
inline uint64_t u64_max(uint64_t a, uint64_t b) { return a > b ? a : b; }

class range_t {
public:
  uint64_t min{};
  uint64_t max{};
  range_t(uint64_t min, uint64_t max) : min(min), max(max) {}

  bool match(uint64_t item) const { return item >= min && item <= max; }
  uint64_t size() const {
    assert(max >= min && "Invalid range with max < min");
    return max - min + 1;
  }

  bool overlaps(const range_t &other) const {
    return !(other.max < min || other.min > max);
  }

  range_t merge(const range_t &other) const {
    assert(overlaps(other) && "Cannot merge non-overlapping ranges");
    uint64_t new_min = u64_min(min, other.min);
    uint64_t new_max = u64_max(max, other.max);
    return {new_min, new_max};
  }
};

void parse(std::vector<range_t> &ranges, std::vector<uint64_t> &items) {
  char line[256];

  while (true) {
    if (!fgets(line, sizeof(line) - 1, stdin)) {
      break;
    }
    uint64_t range_min, range_max;
    if (sscanf(line, "%" SCNu64 "-%" SCNu64, &range_min, &range_max) != 2) {
      break;
    }
    ranges.emplace_back(range_min, range_max);
  }

  uint64_t item;
  while (scanf("%" SCNu64, &item) == 1) {
    items.push_back(item);
  }
}

void merge_unique_ranges(std::vector<range_t> &ranges, const range_t &value) {
  for (auto &&it = ranges.begin(); it != ranges.end(); ++it) {
    if (value.overlaps(*it)) {
      auto merged_range = value.merge(*it);
      ranges.erase(it);
      merge_unique_ranges(ranges, merged_range);
      return;
    }
  }
  ranges.push_back(value);
}

int main() {
  std::vector<range_t> ranges = {};
  std::vector<uint64_t> items = {};
  parse(ranges, items);

  size_t fresh_count = 0;
  for (size_t i = 0; i < items.size(); i++) {
    auto has_match =
        std::any_of(ranges.begin(), ranges.end(),
                    [&](const range_t &r) { return r.match(items[i]); });
    if (has_match) {
      fresh_count++;
    }
  }
  printf("p1: %zu\n", fresh_count);

  size_t total_range_size = 0;
  std::vector<range_t> unique_ranges{};
  for (auto &r : ranges) {
    merge_unique_ranges(unique_ranges, r);
  }
  for (auto &r : unique_ranges) {
    total_range_size += r.size();
  }
  printf("p2: %zu\n", total_range_size);
  return 0;
}
