#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <limits>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

typedef std::vector<std::unordered_set<size_t>> clusters_t;
typedef std::unordered_map<int, std::unordered_set<int>> circuits_mapping_t;

class Coordinate {
public:
  int32_t x{};
  int32_t y{};
  int32_t z{};

  Coordinate() = default;
  Coordinate(int32_t x, int32_t y, int32_t z) : x(x), y(y), z(z) {}

  double distance(const Coordinate &other) const {
    int64_t dx = x - other.x;
    int64_t dy = y - other.y;
    int64_t dz = z - other.z;
    return sqrt(static_cast<double>(dx * dx + dy * dy + dz * dz));
  }

  std::string to_string() const {
    return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " +
           std::to_string(z) + ")";
  }
};

class Circuits {
public:
  Circuits(const std::vector<Coordinate> &coords) { set_coords(coords); };

  void set_coords(const std::vector<Coordinate> &coords) {
    coords_ = coords;
    connections_.clear();

    auto len = coords.size();
    dist_matrix_.clear();
    dist_matrix_.resize(len, std::vector<double>(len, 0.0));

    for (size_t i = 0; i < len; ++i) {
      connections_[i] = {};
      dist_matrix_[i][i] = std::numeric_limits<double>::max();
      for (size_t j = i + 1; j < len; ++j) {
        double dist = coords[i].distance(coords[j]);
        dist_matrix_[i][j] = dist;
      }
    }
  }

  size_t connect_shortest_n_times(size_t n) {
    size_t count = 0;
    for (; count < n; count++) {
      if (!connect_next_shortest()) {
        break;
      }
    }
    return count;
  }

  std::optional<std::pair<int, int>> connect_next_shortest() {
    // find shortest distance
    double shortest_dist = std::numeric_limits<double>::max();
    int idx1 = -1, idx2 = -1;
    size_t len = dist_matrix_.size();

    for (size_t i = 0; i < len; i++) {
      for (size_t j = i + 1; j < len; j++) {
        if (dist_matrix_[i][j] < shortest_dist) {
          shortest_dist = dist_matrix_[i][j];
          idx1 = static_cast<int>(i);
          idx2 = static_cast<int>(j);
        }
      }
    }

    if (idx1 == -1 || idx2 == -1) {
      return std::nullopt;
    }

#ifndef NDEBUG
    printf("connect: %d - %d\n", idx1, idx2);
#endif

    connections_[idx1].insert(idx2);
    connections_[idx2].insert(idx1);
    dist_matrix_[idx1][idx2] = std::numeric_limits<double>::max();
    return std::make_optional(std::make_pair(idx1, idx2));
  }

  clusters_t get_clusters() const {
    clusters_t clusters;
    std::unordered_set<size_t> visited;

    for (const auto &pair : connections_) {
      size_t node = pair.first;
      if (visited.contains(node)) {
        continue;
      }

      std::unordered_set<size_t> cluster;
      std::unordered_set<size_t> stack;
      stack.insert(node);

      while (!stack.empty()) {
        size_t current = *stack.begin();
        stack.erase(stack.begin());

        if (visited.contains(current)) {
          continue;
        }

        visited.insert(current);
        cluster.insert(current);

        for (const auto &neighbor : connections_.at(current)) {
          stack.insert(neighbor);
        }
      }

      clusters.push_back(cluster);
    }

    return clusters;
  }

  bool is_fully_connected() const {
    const auto len = connections_.size();
    std::unordered_set<size_t> visited;
    visited.reserve(len);
    std::unordered_set<size_t> queue;
    queue.insert(0);

    while (!queue.empty()) {
      size_t current = *queue.begin();
      queue.erase(queue.begin());

      if (visited.contains(current)) {
        continue;
      }

      visited.insert(current);
      for (const auto &neighbor : connections_.at(current)) {
        queue.insert(neighbor);
      }
    }
    return visited.size() == len;
  }

  int64_t get_top_3_cluster_product() const {
    clusters_t clusters = get_clusters();
    std::vector<size_t> cluster_sizes;

    for (const auto &cluster : clusters) {
      cluster_sizes.push_back(cluster.size());

#ifndef NDEBUG
      std::cout << "cluster " << cluster_sizes.size() << ": " << cluster.size()
                << "\n";
#endif
    }

    std::sort(cluster_sizes.begin(), cluster_sizes.end(),
              std::greater<size_t>());

    if (cluster_sizes.size() < 3) {
      return 0;
    }

    return static_cast<int64_t>(cluster_sizes[0]) *
           static_cast<int64_t>(cluster_sizes[1]) *
           static_cast<int64_t>(cluster_sizes[2]);
  }

  Coordinate get_coordinate(int index) const { return coords_.at(index); }

private:
  std::vector<Coordinate> coords_{};
  std::unordered_map<int, std::unordered_set<int>> connections_{};
  std::vector<std::vector<double>> dist_matrix_{};
};

int main() {
  std::vector<Coordinate> cords;
  cords.reserve(1000);

  while (true) {
    int32_t x, y, z;
    if (scanf("%d, %d, %d", &x, &y, &z) != 3) {
      break;
    }
    cords.emplace_back(x, y, z);
  }

  Circuits circuits(cords);

  circuits.connect_shortest_n_times(10);
  auto p1_10 = circuits.get_top_3_cluster_product();
  printf("p1 (step=10): %zu\n", p1_10);

  circuits.connect_shortest_n_times(1000 - 10);
  if (circuits.is_fully_connected()) {
    printf("[WARN] Circuits are fully connected (sample data?), reset.\n");
    circuits.set_coords(cords);
  } else {
    auto p1_1000 = circuits.get_top_3_cluster_product();
    printf("p1 (step=1000): %zu\n", p1_1000);
  }

  std::optional<std::pair<int, int>> result;
  while (!circuits.is_fully_connected()) {
    result = circuits.connect_next_shortest();
    if (!result) {
      break;
    }
  }

  if (!result) {
    printf("[ERR ] Failed to connect the circuit.\n");
  } else {
    auto [idx1, idx2] = *result;
    auto pt1 = circuits.get_coordinate(idx1);
    auto pt2 = circuits.get_coordinate(idx2);

#ifndef NDEBUG
    auto pt1_str = pt1.to_string();
    auto pt2_str = pt2.to_string();
    printf("Last connect: %s(idx=%d) - %s(idx=%d)\n", pt1_str.c_str(),
           static_cast<int>(idx1), pt2_str.c_str(), static_cast<int>(idx2));
#endif

    auto p2 = pt1.x * pt2.x;
    printf("p2: %d\n", p2);
  }

  return 0;
}
