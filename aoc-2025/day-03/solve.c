#include <assert.h>
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef int8_t digit_t;
int clean_string(digit_t *out, const char *in) {
  int len = 0;
  for (; isdigit(*in); in++) {
    *out++ = *in - '0';
    len++;
  }
  *out = 0;
  return len;
}

int64_t solve_value(const digit_t *digits, size_t len, int digits_left,
                    int64_t prev_result) {
  if (digits_left == 0) {
    return prev_result;
  }

  // reserve (digits_left - 1) digits from scanning.
  int highest_idx = -1;
  digit_t highest_value = -1;
  for (int i = len - 1 - digits_left + 1; i >= 0; i--) {
    if (digits[i] >= highest_value) {
      highest_value = digits[i];
      highest_idx = i;
    }
  }
  //   printf("found: idx=%d value=%d\n", highest_idx, highest_value);

  assert(highest_value >= 0);

  return solve_value(&digits[highest_idx + 1], len - (highest_idx + 1),
                     digits_left - 1, prev_result * 10 + highest_value);
}

int main() {
  char buffer[256] = {};
  digit_t digits[256] = {};

  int64_t result_p1 = 0;
  int64_t result_p2 = 0;

  while (fgets(buffer, sizeof(buffer) - 1, stdin)) {
    size_t len = clean_string(digits, buffer);
    int64_t p1 = solve_value(digits, len, 2, 0);
    int64_t p2 = solve_value(digits, len, 12, 0);
    // int64_t p2 = 0;
    result_p1 += p1;
    result_p2 += p2;

#ifndef NDEBUG
    int buffer_len = strlen(buffer);
    if (buffer[buffer_len - 1] == '\n') {
      buffer[buffer_len - 1] = '\0';
    }
    printf("p1 %s -> %ld\n", buffer, p1);
    printf("p2 %s -> %ld\n", buffer, p2);
#endif
  }

  printf("p1: sum=%ld\n", result_p1);
  printf("p2: sum=%ld\n", result_p2);

  return 0;
}
