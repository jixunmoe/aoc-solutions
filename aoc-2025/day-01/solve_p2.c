#include <stdio.h>

struct dial_t {
  int position;
  int clicks;
};

void dial_init(struct dial_t *dial) {
  dial->position = 50;
  dial->clicks = 0;
}

void dial_turn_left(struct dial_t *dial) {
  if (--dial->position == 0) {
    dial->clicks += 1;
  }
  if (dial->position < 0) {
    dial->position = 99;
  }
}
void dial_turn_right(struct dial_t *dial) {
  if (++dial->position == 100) {
    dial->clicks += 1;
    dial->position = 0;
  }
}
void dial_turn(struct dial_t *dial, int distance) {
  for (; distance < 0; distance++) {
    dial_turn_left(dial);
  }
  for (; distance > 0; distance--) {
    dial_turn_right(dial);
  }
}

int main(int argc, char *argv[]) {
  char direction;
  int distance;
  struct dial_t dial;
  dial_init(&dial);

  while (scanf(" %c%d", &direction, &distance) == 2) {
    printf("input: %c%d\n", direction, distance);
    if (direction == 'L') {
      dial_turn(&dial, -distance);
    } else if (direction == 'R') {
      dial_turn(&dial, distance);
    }
  }

  printf("dial: position=%d clicks=%d\n", dial.position, dial.clicks);

  return 0;
}