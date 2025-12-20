#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef int8_t cell_t;
typedef int64_t weight_t;

#define CELL_START_LOCATION ('S')
#define CELL_FREE_SPACE ('.')
#define CELL_SPLITER ('^')
#define CELL_BEAM ('|')

#define GRID_MAX_WIDTH (150)
#define GRID_MAX_HEIGHT (150)

#define LINE_BUFFER_SIZE (160)

#if !defined(MAX) || !defined(MIN)
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

struct grid_t {
  cell_t cells[GRID_MAX_HEIGHT][GRID_MAX_WIDTH];
  int width;
  int height;
};
struct grid_cache_t {
  weight_t weights[GRID_MAX_HEIGHT][GRID_MAX_WIDTH];
  cell_t cells[GRID_MAX_HEIGHT][GRID_MAX_WIDTH];
  int width;
  int height;
};

void grid_cache_init(struct grid_cache_t *out, const struct grid_t *grid_src) {
  memset(out->weights, -1, sizeof(out->weights));
  memcpy(&out->cells, grid_src->cells, sizeof(out->cells));
  out->width = grid_src->width;
  out->height = grid_src->height;
}

weight_t grid_cache_calculate(struct grid_cache_t *grid, int row, int col) {
  int width = grid->width;
  int height = grid->height;

  if (row < 0 || row >= height || col < 0 || col >= width) {
    return 0;
  }

  // use cached value
  if (grid->weights[row][col] != -1) {
    return grid->weights[row][col];
  }

  // let's call ourself recursively to left and right
  if (grid->cells[row][col] == CELL_SPLITER) {
    weight_t left = MAX(1, grid_cache_calculate(grid, row, col - 1));
    weight_t right = MAX(1, grid_cache_calculate(grid, row, col + 1));
    weight_t final_weight = left + right;
    grid->weights[row][col] = left + right;
    return final_weight;
  }

  // keep move down
  weight_t weight = MAX(1, grid_cache_calculate(grid, row + 1, col));
  grid->weights[row][col] = weight;
  return weight;
}

void grid_print(const struct grid_t *grid) {
  for (int i = 0; i < grid->height; i++) {
    for (int j = 0; j < grid->width; j++) {
      putchar((char)grid->cells[i][j]);
    }
    putchar('\n');
  }
  fflush(stdout);
}

void grid_parse(struct grid_t *grid, FILE *stream) {
  char buffer[LINE_BUFFER_SIZE];
  grid->width = 0;
  grid->height = 0;

  int width = 0, height = 0;

  for (int i = 0; i < GRID_MAX_HEIGHT; i++, height++) {
    if (!fgets(buffer, sizeof(buffer) - 1, stream)) {
      break;
    }

    width = 0;
    cell_t *row_curr = grid->cells[i];

    for (int j = 0; j < GRID_MAX_WIDTH; j++, width++) {
      cell_t cell = (cell_t)buffer[j];
      if (cell != CELL_FREE_SPACE && cell != CELL_SPLITER &&
          cell != CELL_BEAM && cell != CELL_START_LOCATION) {
        // skip rest of line
        break;
      }
      row_curr[j] = cell;
    }
  }

  grid->width = width;
  grid->height = height;
}

int solve_p1(const struct grid_t *grid_src) {
  struct grid_t grid;
  memcpy(&grid, grid_src, sizeof(struct grid_t));

  int split_count = 0;
  int width = grid.width;
  int height = grid.height;

  cell_t *row_prev;
  cell_t *row_curr = grid.cells[0];
  for (int i = 1; i < height; i++) {
    row_prev = row_curr;
    row_curr = grid.cells[i];

    for (int col = 0; col < width; col++) {
      cell_t cell_up = row_prev[col];
      if (cell_up == CELL_BEAM || cell_up == CELL_START_LOCATION) {
        if (row_curr[col] == CELL_SPLITER) {
          // split to left and right
          row_curr[col - 1] = CELL_BEAM;
          row_curr[col + 1] = CELL_BEAM;
          split_count++;
        } else if (row_curr[col] == CELL_FREE_SPACE) {
          // continue down
          row_curr[col] = CELL_BEAM;
        }
      }
    }
  }
  return split_count;
}

int64_t solve_p2(const struct grid_t *grid_src) {
  struct grid_cache_t grid;
  grid_cache_init(&grid, grid_src);

  // find start cord, always at the first line.
  int start_col = -1;
  for (int col = 0; col < grid.width; col++) {
    if (grid.cells[0][col] == CELL_START_LOCATION) {
      start_col = col;
      break;
    }
  }
  assert(start_col != -1 && "start location not found");

  weight_t result = grid_cache_calculate(&grid, 0, start_col);
#ifndef NDEBUG
  printf("Weights grid:\n");
  for (int i = 0; i < grid.height; i++) {
    for (int j = 0; j < grid.width; j++) {
      printf("%" PRId64 ",", grid.weights[i][j]);
    }
    printf("\n");
  }
#endif
  return result;
}

int main() {
  struct grid_t grid;
  grid_parse(&grid, stdin);

#ifndef NDEBUG
  printf("Parsed grid:\n");
  grid_print(&grid);
#endif

  int p1 = solve_p1(&grid);
  printf("p1: %d\n", p1);

  weight_t p2 = solve_p2(&grid);
  printf("p2: %" PRId64 "\n", p2);

  return 0;
}
