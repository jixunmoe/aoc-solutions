#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

typedef std::vector<std::unordered_set<int>> clusters_t;

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

template <typename T> bool set_intersects(const T &a, const T &b) {
  return std::any_of(a.cbegin(), a.cend(),
                     [&b](const int &elem) { return b.contains(elem); });
}

void insert_to_clusters(clusters_t &clusters, int search_idx, int to_add) {
  // now insert to a cluster, or create a new one
  bool inserted = false;
  for (auto &cluster : clusters) {
    if (cluster.contains(search_idx) || cluster.contains(to_add)) {
      cluster.insert(static_cast<int>(to_add));
      cluster.insert(static_cast<int>(search_idx));
      inserted = true;
    }
  }

  if (!inserted) {
    std::unordered_set<int> new_cluster;
    new_cluster.insert(static_cast<int>(to_add));
    new_cluster.insert(search_idx);
    clusters.push_back(new_cluster);
  }
}

clusters_t merge_clusters(const clusters_t &clusters) {
  bool needs_merge_again = false;
  clusters_t merged_clusters;
  for (const auto &cluster : clusters) {
    bool merged = false;
    for (auto &m_cluster : merged_clusters) {
      if (set_intersects(m_cluster, cluster)) {
        m_cluster.insert(cluster.begin(), cluster.end());
        merged = true;
        needs_merge_again = true;
        break;
      }
    }
    if (!merged) {
      merged_clusters.push_back(cluster);
    }
  }

  if (needs_merge_again) {
    return merge_clusters(merged_clusters);
  } else {
    return merged_clusters;
  }
}

bool is_debug_enabled() {
  if (auto env_var = std::getenv("DEBUG"); env_var) {
    char v = *env_var;
    return v == 'y' || v == 'Y' || v == '1';
  }

  return false;
}

int main() {
  auto is_debug = is_debug_enabled();
  auto iter_count_str = std::getenv("ITER_COUNT");
  auto ITER_COUNT = iter_count_str ? std::stoi(iter_count_str) : 10;
  if (ITER_COUNT <= 0) {
    ITER_COUNT = 10;
  }

  std::vector<Coordinate> cords;
  cords.reserve(1000);

  while (true) {
    int32_t x, y, z;
    if (scanf("%d, %d, %d", &x, &y, &z) != 3) {
      break;
    }
    cords.emplace_back(x, y, z);
  }

  auto len = cords.size();

  std::vector<std::vector<double>> dist_matrix(len,
                                               std::vector<double>(len, 0.0));

  for (size_t i = 0; i < len; ++i) {
    dist_matrix[i][i] = std::numeric_limits<double>::max();
    for (size_t j = i + 1; j < len; ++j) {
      double dist = cords[i].distance(cords[j]);
      dist_matrix[i][j] = dist;
      dist_matrix[j][i] = dist;
    }
  }

  if (is_debug) {
    printf("Distance Matrix:\n");
    for (size_t i = 0; i < len; ++i) {
      for (size_t j = 0; j < len; ++j) {
        printf("%f,", dist_matrix[i][j]);
      }
      printf("\n");
    }
  }

  clusters_t clusters;
  constexpr double kMaxDouble = std::numeric_limits<double>::max();

  for (int z = 0; z < ITER_COUNT; z++) {
    int idx1 = -1, idx2 = -1;
    double shortest_dist = kMaxDouble;

    for (size_t i = 0; i < cords.size(); i++) {
      for (size_t j = i + 1; j < cords.size(); j++) {
        if (dist_matrix[i][j] < shortest_dist) {
          shortest_dist = dist_matrix[i][j];
          idx1 = static_cast<int>(i);
          idx2 = static_cast<int>(j);
        }
      }
    }

    if (idx1 == -1 || idx2 == -1) {
      break;
    }

    insert_to_clusters(clusters, idx1, idx2);

    dist_matrix[idx1][idx2] = kMaxDouble;
    dist_matrix[idx2][idx1] = kMaxDouble;
  }

  clusters_t merged_clusters = merge_clusters(clusters);

  std::vector<size_t> cluster_sizes;
  cluster_sizes.reserve(merged_clusters.size());
  for (const auto &cluster : merged_clusters) {
    cluster_sizes.push_back(cluster.size());
  }
  std::sort(cluster_sizes.begin(), cluster_sizes.end(), std::greater<size_t>());

  if (is_debug) {
    printf("Clusters found: %zu\n", merged_clusters.size());
    for (size_t i = 0; i < merged_clusters.size(); ++i) {
      printf("Cluster %zu: ", i);
      for (const auto &index : merged_clusters[i]) {
        printf("%d ", index);
      }
      printf("\n");
    }

    printf("Sorted cluster sizes: ");
    for (auto &&size : cluster_sizes) {
      std::cout << size << ' ';
    }
    std::cout << std::endl;
  }

  auto p1 = cluster_sizes[0] * cluster_sizes[1] * cluster_sizes[2];
  printf("p1: %zu\n", p1);

  return 0;
}
