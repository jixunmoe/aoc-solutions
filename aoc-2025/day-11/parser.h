#pragma once

#include <cstdint>
#include <set>
#include <unordered_map>
#include <utility>
#include <vector>

typedef uint32_t node_t;
typedef std::unordered_map<node_t, std::set<node_t>> graph_t;

std::pair<node_t, std::set<node_t>> parse_reactor_node_line(const char *line);
graph_t parse_reactor_graph();
void print_reactor_graph(const graph_t &graph);

inline constexpr node_t node_chars_to_id(char a, char b, char c) {
  return static_cast<node_t>((a << 16) | (b << 8) | c);
}
inline node_t node_chars_to_id(const char *text) {
  return node_chars_to_id(text[0], text[1], text[2]);
}

inline void node_id_to_chars(char *text, node_t node) {
  text[0] = static_cast<char>((node >> 16) & 0xFF);
  text[1] = static_cast<char>((node >> 8) & 0xFF);
  text[2] = static_cast<char>(node & 0xFF);
  text[3] = '\0';
}

constexpr node_t kNodeYou = node_chars_to_id('y', 'o', 'u');
constexpr node_t kNodeOut = node_chars_to_id('o', 'u', 't');
constexpr node_t kNodeServerRack = node_chars_to_id('s', 'v', 'r');
constexpr node_t kNodeAnchor1 = node_chars_to_id('f', 'f', 't');
constexpr node_t kNodeAnchor2 = node_chars_to_id('d', 'a', 'c');
