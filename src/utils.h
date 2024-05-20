#ifndef UTILS_H
#define UTILS_H

#include <algorithm>
#include <climits>
#include <cmath>
#include <iostream>
#include <map>
// #include <mpi.h>
#include <tuple>
#include <vector>

typedef std::tuple<int, int, int> Edge; // Define a type alias for the edge tuple

// Function declaration for reading the graph from a file
void read_graph(const std::string filename, std::vector<Edge> *edges, std::map<int, int> *node_order);
void generate_matrix(const std::vector<std::tuple<int, int, int>> &edges, std::vector<std::vector<int>> &matrix);
int factorial(int n);
std::vector<int> get_permutations(int n, int index);
std::vector<std::vector<int>> valid_paths(std::vector<std::vector<int>> &paths, std::vector<std::vector<int>> &matrix, int total_capacity, std::map<int, int> &node_order);
std::vector<int> best_path(std::vector<std::vector<int>> &matrix, std::vector<std::vector<int>> &paths);
std::vector<int> get_permutations_parallel(int n, int index);
std::vector<std::vector<int>> valid_paths_parallel(std::vector<std::vector<int>> &paths, std::vector<std::vector<int>> &matrix, int total_capacity, std::map<int, int> &node_order);
std::vector<int> best_path_parallel(std::vector<std::vector<int>> &matrix, std::vector<std::vector<int>> &paths);
std::vector<std::vector<int>> valid_paths_heurist(std::vector<std::vector<int>> &paths, std::vector<std::vector<int>> &matrix, int total_capacity, std::map<int, int> &node_order);
std::vector<int> best_path_heuristic(std::vector<std::vector<int>> &matrix, std::vector<std::vector<int>> &paths);
#endif // UTILS_H
