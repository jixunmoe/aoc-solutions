#!/bin/bash

year=$(date +%Y)
toc=""
for i in $(seq 25); do
  day="day-$(printf "%02d" $i)"

  prev="[< prev]"
  next="[next >]"

  if [[ $i -ne 1 ]]; then
    prev="[[< prev]](../day-$(printf "%02d" $((i-1)))/README.MD)"
  fi
  if [[ $i -ne 25 ]]; then
    next="[[next >]](../day-$(printf "%02d" $((i+1)))/README.MD)"
  fi

  if [[ ! -f "aoc-$year/$day/README.MD" ]]; then
    mkdir -p "aoc-$year/$day"
    cat > "aoc-$year/$day/README.MD" <<EOF
# Day $i: TBD

[[^ up]](../../README.asciidoc) $prev $next <!-- [[solution ✨]](./solve.py) -->

<!-- article begin -->

<!-- article end -->

---

* Puzzle: https://adventofcode.com/$year/day/$i
* Input: https://adventofcode.com/$year/day/$i/input

EOF
  fi
done
