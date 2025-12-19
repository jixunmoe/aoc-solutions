#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define MAX_GRID_SIZE (137)
#define LINE_BUFFER_SIZE (MAX_GRID_SIZE + 1)

#define CELL_EMPTY '.'
#define CELL_BOX '@'

struct grid_t {
  uint8_t cells[MAX_GRID_SIZE][MAX_GRID_SIZE];
  int rows;
  int cols;
};

void grid_from_stream(struct grid_t *grid, FILE *stream) {
  char buffer[LINE_BUFFER_SIZE] = {};
  memset(grid, 0, sizeof(*grid));

  int row = 0;
  int col = 0;
  while (row < MAX_GRID_SIZE && fgets(buffer, LINE_BUFFER_SIZE, stream)) {
    col = 0;
    for (char *p = buffer; *p; p++, col++) {
      if (*p == CELL_EMPTY) {
        grid->cells[row][col] = CELL_EMPTY;
      } else if (*p == CELL_BOX) {
        grid->cells[row][col] = CELL_BOX;
      } else {
        grid->cells[row][col] = 0;
        break;
      }
    }
    row++;
  }
  grid->rows = row;
  grid->cols = col;
}

void grid_print(const struct grid_t *grid) {
  for (int r = 0; r < grid->rows; r++) {
    for (int c = 0; c < grid->cols; c++) {
      fputc(grid->cells[r][c], stdout);
    }
    fputc('\n', stdout);
    fflush(stdout);
  }
}

bool grid_is_box_at(const struct grid_t *grid, int row, int col) {
  if (row < 0 || row >= grid->rows || col < 0 || col >= grid->cols) {
    return false;
  }
  return grid->cells[row][col] == CELL_BOX;
}

int grid_count_surrounded_boxes(const struct grid_t *grid, int row, int col) {
  int count = 0;

  // find the number of boxes around the cord.
  static const int directions[8][2] = {
      {-1, -1}, {-1, 0}, {-1, 1}, //
      {0, -1},  {0, 1},           //
      {1, -1},  {1, 0},  {1, 1},  //
  };
  for (int i = 0; i < 8; i++) {
    int dr = directions[i][0];
    int dc = directions[i][1];
    if (grid_is_box_at(grid, row + dr, col + dc)) {
      count++;
    }
  }
  return count;
}

int solve(struct grid_t *next, const struct grid_t *grid) {
  next->cols = grid->cols;
  next->rows = grid->rows;

  int box_removed = 0;

  for (int r = 0; r < grid->rows; r++) {
    for (int c = 0; c < grid->cols; c++) {
      if (grid->cells[r][c] == CELL_BOX) {
        int surrounded_boxes = grid_count_surrounded_boxes(grid, r, c);

        if (surrounded_boxes < 4) {
          next->cells[r][c] = CELL_EMPTY;
          box_removed += 1;
        } else {
          next->cells[r][c] = CELL_BOX;
        }
      } else {
        next->cells[r][c] = CELL_EMPTY;
      }
    }
  }

  return box_removed;
}

int main() {
  struct grid_t grid = {};
  struct grid_t next_grid = {};
  grid_from_stream(&grid, stdin);

  int p1 = solve(&next_grid, &grid);

  int p2 = p1;
  while (true) {
    memcpy(&grid, &next_grid, sizeof(grid));
    int next_removed = solve(&next_grid, &grid);
    if (next_removed == 0) {
      break;
    }
    p2 += next_removed;
  }

  printf("p1: %d\n", p1);
  printf("p2: %d\n", p2);

  return 0;
}
