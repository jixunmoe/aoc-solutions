#include "parser.h"

#include <cstddef>
#include <cstdio>
#include <queue>
#include <unordered_map>

struct visit_node_t {
  node_t id;
  std::set<node_t> path;
};

std::set<node_t> append_path(const std::set<node_t> &path, node_t new_node) {
  auto new_path = path;
  new_path.insert(new_node);
  return new_path;
}

typedef std::unordered_map<node_t, size_t> cache_t;

template <size_t PART_NUMBER>
size_t solve_nodes(const graph_t &graph, node_t cache_key, cache_t &cache) {
  constexpr node_t kMaskVisitedAnchor1 = (node_t{1} << 24);
  constexpr node_t kMaskVisitedAnchor2 = (node_t{1} << 25);
  node_t start_node = cache_key & node_t{(1 << 24) - 1};
  node_t visited_attr = (node_t{0xFF} << 24) & cache_key;

  if (start_node == kNodeOut) {
    if (PART_NUMBER == 1 || (cache_key >> 24) == 3) {
      return 1;
    } else {
      return 0;
    }
  }

  if (auto hit = cache.find(cache_key); hit != cache.end()) {
    return hit->second;
  }

  size_t paths = 0;
  if (auto it_n = graph.find(start_node); it_n != graph.end()) {
    for (const auto &next : it_n->second) {
      node_t next_cache_key = next | visited_attr;
      next_cache_key |= next == kNodeAnchor1 ? kMaskVisitedAnchor1 : 0;
      next_cache_key |= next == kNodeAnchor2 ? kMaskVisitedAnchor2 : 0;
      paths += solve_nodes<PART_NUMBER>(graph, next_cache_key, cache);
    }
  }
  cache[cache_key] = paths;
  return paths;
}

size_t solve_p1(const graph_t &graph) {
  cache_t cache;
  return solve_nodes<1>(graph, kNodeYou, cache);
}

size_t solve_p2(const graph_t &graph) {
  cache_t cache;
  return solve_nodes<2>(graph, kNodeServerRack, cache);
}

int main() {
  graph_t graph = parse_reactor_graph();

#ifndef NDEBUG
  printf("Parsed reactor graph:\n");
  print_reactor_graph(graph);
#endif

  size_t p1 = solve_p1(graph);
  printf("p1: %zu\n", p1);

  size_t p2 = solve_p2(graph);
  printf("p2: %zu\n", p2);

  return 0;
}
