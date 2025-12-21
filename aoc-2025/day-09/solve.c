#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_COORD_SIZE (500)

struct point_t {
  int x;
  int y;
};

double find_x_of_intersection(int64_t y, const struct point_t *a,
                              const struct point_t *b) {
  // gradient of (a -> b) and ((x, y) -> b) should be the same. Now find x.
  return (double)(y - a->y) * (b->x - a->x) / (b->y - a->y) + a->x;
}

double cross_product_direct(int64_t ax, int64_t ay, int64_t bx, int64_t by,
                            int64_t cx, int64_t cy) {
  // Vector AB x AC
  return (bx - ax) * (cy - ay) - (by - ay) * (cx - ax);
}

int64_t points_area(const struct point_t *p1, const struct point_t *p2) {
  int64_t dx = labs(p2->x - p1->x) + 1;
  int64_t dy = labs(p2->y - p1->y) + 1;
  return dx * dy;
}

bool point_equal(const struct point_t *p1, const struct point_t *p2) {
  return p1->x == p2->x && p1->y == p2->y;
}

struct polygon_t {
  size_t size;
  struct point_t points[MAX_COORD_SIZE];
};

void polygon_parse(struct polygon_t *polygon) {
  memset(polygon, 0, sizeof(*polygon));

  struct point_t *p = polygon->points;
  while (scanf(" %d, %d", &p->x, &p->y) == 2) {
    p++;
  }
  polygon->size = p - polygon->points;
}

bool polygon_contains_point(const struct polygon_t *polygon,
                            const struct point_t *point) {
  const struct point_t *a, *b;
  const struct point_t *end = &polygon->points[polygon->size];

  bool is_inside = false;
  a = &polygon->points[0];
  b = &polygon->points[polygon->size - 1];

  for (; a < end; b = a++) {
    // if the point is on a vertex, it is considered inside
    if (point_equal(a, point)) {
      return true;
    }

    // ray casting here!
    // - only consider *half* open intervals
    //     (i.e. in a v shaped corner, count both or none.)
    // - this also excludes horizontal edges if it hits - it will be a noop.
    if ((a->y <= point->y && point->y < b->y) ||
        (b->y <= point->y && point->y < a->y)) {

      // check point of intersection, it should be to the right of the point
      double x_of_intersection = find_x_of_intersection(point->y, a, b);
      if (point->x < x_of_intersection) {
        is_inside = !is_inside;
      }
    }
  }
  return is_inside;
}

// only consider when they properly intersect
// exclude cases when they overlap.
bool segment_intersets_within(const int64_t x1, const int64_t y1,
                              const int64_t x2, const int64_t y2,
                              const int64_t x3, const int64_t y3,
                              const int64_t x4, const int64_t y4) {
  double cp1 = cross_product_direct(x1, y1, x2, y2, x3, y3);
  double cp2 = cross_product_direct(x1, y1, x2, y2, x4, y4);
  double cp3 = cross_product_direct(x3, y3, x4, y4, x1, y1);
  double cp4 = cross_product_direct(x3, y3, x4, y4, x2, y2);

  if (((cp1 < 0 && cp2 > 0) || (cp1 > 0 && cp2 < 0)) &&
      ((cp3 < 0 && cp4 > 0) || (cp3 > 0 && cp4 < 0))) {
    return true;
  }
  return false;
}

// check if a given segment intersecs with any of the polygon edges
//   (ignores the case where they might extend to the same line)
bool polygon_intersects_within(const struct polygon_t *polygon,
                               const struct point_t segment[2]) {
  const struct point_t *a, *b;
  const struct point_t *end = &polygon->points[polygon->size];

  const struct point_t *c = &segment[0];
  const struct point_t *d = &segment[1];

  a = &polygon->points[0];
  b = &polygon->points[polygon->size - 1];
  for (; a < end; b = a++) {
    if (segment_intersets_within(a->x, a->y, b->x, b->y, c->x, c->y, d->x,
                                 d->y)) {
      return true;
    }
  }
  return false;
}

bool polygon_can_fit_rect(const struct polygon_t *polygon,
                          const struct point_t *p_tl,
                          const struct point_t *p_br) {
  // check all 4 corners are within the polygon
  struct point_t p_bl = {p_tl->x, p_br->y};
  struct point_t p_tr = {p_br->x, p_tl->y};

  if (!polygon_contains_point(polygon, &p_bl) ||
      !polygon_contains_point(polygon, &p_tr)) {
    return false;
  }

  // check 4 edges does not intersect with polygon edges
  const struct point_t edges[4][2] = {
      {*p_tl, p_tr},
      {p_tr, *p_br},
      {*p_br, p_bl},
      {p_bl, *p_tl},
  };

  for (size_t i = 0; i < 4; i++) {
    if (polygon_intersects_within(polygon, edges[i])) {
      return false;
    }
  }

  return true;
}

int64_t solve_p1(const struct polygon_t *polygon) {
  int64_t highest_area = 0;
  for (size_t i = 0; i < polygon->size; i++) {
    for (size_t j = i + 1; j < polygon->size; j++) {
      int64_t area = points_area(&polygon->points[i], &polygon->points[j]);
      if (area > highest_area) {
        highest_area = area;
      }
    }
  }
  return highest_area;
}

int64_t solve_p2(const struct polygon_t *polygon) {
  int64_t highest_area = 0;

  for (size_t i = 0; i < polygon->size; i++) {
    for (size_t j = i + 1; j < polygon->size; j++) {
      // check if the other 2 pts are within the polygon.

      if (polygon_can_fit_rect(polygon, &polygon->points[i],
                               &polygon->points[j])) {
        int64_t area = points_area(&polygon->points[i], &polygon->points[j]);
        if (area > highest_area) {
          highest_area = area;
#ifndef NDEBUG
          printf("highest pt: (%d,%d) (%d,%d) area=%" PRIu64 "\r",
                 polygon->points[i].x, polygon->points[i].y,
                 polygon->points[j].x, polygon->points[j].y, area);
#endif
        }
      }
    }
  }
#ifndef NDEBUG
  putchar('\n');
  fflush(stdout);
#endif
  return highest_area;
}

int main() {
  struct polygon_t polygon;
  polygon_parse(&polygon);

  int64_t p1 = solve_p1(&polygon);
  printf("p1: %" PRIu64 "\n", p1);

  int64_t p2 = solve_p2(&polygon);
  printf("p2: %" PRIu64 "\n", p2);

  return 0;
}
