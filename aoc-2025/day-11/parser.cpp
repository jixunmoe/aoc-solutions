#include "parser.h"

#include <cstdio>
#include <cstring>

inline bool is_valid_char(char c) { return (c >= 'a' && c <= 'z'); }

inline const char *actual_line_end(const char *line) {
  const char *p = line;
  while (true) {
    char c = *p;
    if (c == ':' || c == ' ' || is_valid_char(c)) {
      p++;
    } else {
      break;
    }
  }
  return p;
}

std::pair<node_t, std::set<node_t>> parse_reactor_node_line(const char *line) {
  const char *end = actual_line_end(line);

  node_t temp_id = 0;
  node_t src_id = 0;
  std::set<node_t> dst_ids{};

  for (const auto *p = line; p < end; ++p) {
    if (*p == ':') {
      src_id = temp_id;
      temp_id = 0;
      p++;
      continue;
    } else if (*p == ' ') {
      dst_ids.insert(temp_id);
      temp_id = 0;
      continue;
    }

    temp_id = (temp_id << 8) | static_cast<node_t>(*p);
  }
  if (temp_id != 0) {
    dst_ids.insert(temp_id);
  }
  return {src_id, dst_ids};
}

graph_t parse_reactor_graph() {
  graph_t graph;
  char buffer[128];
  while (fgets(buffer, sizeof(buffer) - 1, stdin)) {
    auto [src_id, dst_ids] = parse_reactor_node_line(buffer);
    graph[src_id] = dst_ids;
  }
  return graph;
}

void print_reactor_graph(const graph_t &graph) {
  char buffer[4];
  for (const auto &[src, dsts] : graph) {
    node_id_to_chars(buffer, src);
    printf("%s:", buffer);

    for (const auto &dst : dsts) {
      node_id_to_chars(buffer, dst);
      printf(" %s", buffer);
    }
    printf("\n");
  }
}