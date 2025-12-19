#include <ctype.h>
#include <stdint.h>
#include <stdio.h>

int digit_len_and_clean(char *s) {
  int len = 0;
  char *p = s;
  for (; isdigit(*p); p++) {
    len++;
  }
  *p = 0;
  return len;
}

struct indexed_value_t {
  char value;
  int16_t idx;
};
void indexed_value_reset(struct indexed_value_t *iv) {
  iv->value = -1;
  iv->idx = -1;
}
int indexed_value_get(const struct indexed_value_t *a,
                      const struct indexed_value_t *b) {
  return (int)(a->value - '0') * 10 + (int)(b->value - '0');
}

int main() {
  char buffer[256] = {};
  struct indexed_value_t highest = {};
  struct indexed_value_t second_highest = {};

  int64_t sum = 0;

  while (fgets(buffer, sizeof(buffer) - 1, stdin)) {
    indexed_value_reset(&highest);
    indexed_value_reset(&second_highest);

    int len = digit_len_and_clean(buffer);

    // Find the highest digit from [0:-1]
    for (char *p = &buffer[len - 2]; p >= buffer; p--) {
      if (*p >= highest.value) {
        second_highest = highest;
        highest.idx = p - buffer;
        highest.value = *p;
      }
    }

    if (buffer[len - 1] >= second_highest.value) {
      second_highest.idx = len - 1;
      second_highest.value = buffer[len - 1];
    }

    int combined_value = indexed_value_get(&highest, &second_highest);
    sum += combined_value;

#ifndef NDEBUG
    printf("highest %s -> %d (idx=%d, %d)\n", buffer, combined_value,
           highest.idx, second_highest.idx);
#endif
  }

  printf("p1: sum=%ld\n", sum);

  return 0;
}
