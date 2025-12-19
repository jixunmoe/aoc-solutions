# Day 3: Lobby

[[^ up]](../../README.asciidoc) [[< prev]](../day-02/README.MD) [[next >]](../day-04/README.MD) [[solution ✨]](./solve.c)

<!-- article begin -->

## Part 1

Scan for the highest digit (exclude the last one), then pick the highest digit afterwards.

## Part 2

Recurssively find the highest digit exclude `target_length - 1` digits, then keep going from the next digit until the end.

In this case, exclude the last 11 digites and find the highest, then from the next digit until the last 10 digits, and so on, until all digits required were used.

<!-- article end -->

---

* Puzzle: https://adventofcode.com/2025/day/3
* Input: https://adventofcode.com/2025/day/3/input

