# Day 8: Playground

[[^ up]](../../README.asciidoc) [[< prev]](../day-07/README.MD) [[next >]](../day-09/README.MD) [[solution in cpp ✨]](./solve.cpp)

<!-- article begin -->

## Part 1

Initially went with a complicated "build to loose clusters and merge later" approch, this had worked well for a small number of steps (1000 in this case).

Precalulate `std::vector<std::vector<double>> dist_matrix` for the distances of all plugs to speed up look up of the next shortest connection.

Ideally, use the meethod from part 2 instead.

## Part 2

Keep a track of the following:

```cpp
  std::vector<Coordinate> coords_{};
  std::unordered_map<int, std::unordered_set<int>> connections_{};
  std::vector<std::vector<double>> dist_matrix_{};
```

- `coords_`: keep track of all coordinates, as we need the `X` coordinate for the final product.
- `connections_`: 2 way dict that tracks which plugs are connected.
- `dist_matrix_`: calculate the distance of all plugs to speed up calculations for later.

With this structure, once we connected a plug, we can check if the entire circuit board has been connected cheaply by explore the plugs, start from any plug, and see if it eventually visits all plugs.

<!-- article end -->

---

* Puzzle: https://adventofcode.com/2025/day/8
* Input: https://adventofcode.com/2025/day/8/input

