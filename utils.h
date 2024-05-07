#ifndef UTILS_H
#define UTILS_H

#include <fstream>
#include <iostream>
#include <iterator> // For advance
#include <map>
#include <sstream> // For stringstream parsing
#include <string>
#include <tuple>
#include <vector>

typedef std::tuple<int, int, int> Edge; // Define a type alias for the edge tuple

// Function declaration for reading the graph from a file
void read_graph(const std::string filename, std::vector<Edge> *edges, std::map<int, int> *node_order);
void generate_matrix(const std::vector<std::tuple<int, int, int>> &edges, std::vector<std::vector<int>> &matrix);
#endif // UTILS_H
