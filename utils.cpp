#include "utils.h"
using namespace std;

// Function to read the graph from a file
void read_graph(string filename, vector<tuple<int, int, int>> *edges, map<int, int> *node_order) {
    ifstream file(filename);

    if (!file.is_open()) {
        cout << "Error opening file: " << filename << endl;
        return;
    }

    int num_nodes;
    string line;

    // Read the number of nodes
    if (getline(file, line)) {
        try {
            num_nodes = stoi(line) - 1;
            if (num_nodes <= 0) {
                cerr << "Error: Invalid number of nodes: " << num_nodes << endl;
                return;
            }
        } catch (const invalid_argument &e) {
            cerr << "Error: Could not parse number of nodes from line: " << line << endl;
            return;
        }
    } else {
        cerr << "Error: Could not read the number of nodes" << endl;
        return;
    }

    // Read node information
    for (int i = 0; i < num_nodes; i++) {
        if (getline(file, line)) {
            int node, n_order;
            istringstream iss(line); // Create a stringstream for parsing

            // Extract node and order using iterators
            if (iss >> node >> n_order) {
                node_order->insert(pair<int, int>(node, n_order));
            } else {
                cerr << "Error: Invalid node information line: " << line << endl;
            }
        } else {
            cerr << "Error: Unexpected end of file while reading node information" << endl;
            return;
        }
    }

    int num_edges;

    // Read the number of edges
    if (getline(file, line)) {
        try {
            num_edges = stoi(line);
            if (num_edges <= 0) {
                cerr << "Error: Invalid number of edges: " << num_edges << endl;
                return;
            }
        } catch (const invalid_argument &e) {
            cerr << "Error: Could not parse number of edges from line: " << line << endl;
            return;
        }
    } else {
        cerr << "Error: Could not read the number of edges" << endl;
        return;
    }

    // Read edge information
    for (int i = 0; i < num_edges; i++) {
        if (getline(file, line)) {
            int start, end, weight;
            istringstream iss(line); // Create a stringstream for parsing

            // Extract start, end, and weight using iterators
            if (iss >> start >> end >> weight) {
                edges->push_back(make_tuple(start, end, weight));
            } else {
                cerr << "Error: Invalid edge information line: " << line << endl;
            }
        } else {
            cerr << "Error: Unexpected end of file while reading edge information" << endl;
            return;
        }
    }

    file.close();
}

void generate_matrix(const std::vector<std::tuple<int, int, int>> &edges, std::vector<std::vector<int>> &matrix) {

    int n = matrix.size();

    // Initialize the matrix with -1
    for (int i = 0; i < n; i++) {
        matrix[i].resize(n, -1);
    }

    // Fill the matrix using the edges information
    for (const auto &edge : edges) {
        int u = std::get<0>(edge);
        int v = std::get<1>(edge);
        int w = std::get<2>(edge);
        matrix[u][v] = w;
    }
}

int factorial(int n) {
    if (n == 0 || n == 1)
        return 1;
    return n * factorial(n - 1);
}

vector<int> get_permutations(int n, int index) {
    vector<int> factorial(n, 1);
    vector<int> permutation;
    vector<int> elements;

    for (int i = 1; i < n; i++) {
        factorial[i] = factorial[i - 1] * i;
        elements.push_back(i);
    }
    elements.push_back(n);

    index--; // to convert to 0-based index
    for (int i = n - 1; i >= 0; i--) {
        int j = index / factorial[i];
        index %= factorial[i];
        permutation.push_back(elements[j]);
        elements.erase(elements.begin() + j);
    }

    permutation.insert(permutation.begin(), 0);
    permutation.push_back(0);

    return permutation;
}

vector<vector<int>> valid_paths(vector<vector<int>> &paths, vector<vector<int>> &matrix, int total_capacity, map<int, int> &node_order) {
    vector<vector<int>> possible_paths;
    int n = paths.size();

    for (int i = 0; i < n; i++) {
        vector<int> new_path;
        new_path.push_back(0); // Start with depot (0)
        int total_capacity_used = 0;

        for (int j = 0; j < paths[i].size() - 1; j++) {
            int node = paths[i][j];
            int next_node = paths[i][j + 1];

            if (matrix[node][next_node] != -1 && total_capacity_used + node_order[next_node] <= total_capacity) {
                new_path.push_back(next_node);
                total_capacity_used += node_order[next_node];
            } else {
                if (new_path.back() != 0) { // Prevent consecutive zeros
                    new_path.push_back(0);  // Return to depot to reset capacity
                }
                total_capacity_used = 0; // Reset capacity after returning to depot

                if (matrix[0][next_node] != -1) { // If there is a path from depot to next_node
                    new_path.push_back(next_node);
                    total_capacity_used += node_order[next_node];
                }
            }
        }

        if (new_path.back() != 0) { // Avoid appending 0 if already at depot
            new_path.push_back(0);
        }

        possible_paths.push_back(new_path);
    }

    return possible_paths;
}

vector<int> best_path(vector<vector<int>> &matrix, vector<vector<int>> &paths) {

    vector<int> best_path;
    int best_cost = INT_MAX;

    for (int i = 0; i < paths.size(); i++) {
        vector<int> path = paths[i];
        int cost = 0;
        for (int j = 0; j < path.size() - 1; j++) {
            int node = path[j];
            int next_node = path[j + 1];
            cost += matrix[node][next_node];
        }
        cout << "Cost of path " << i << ": " << cost << endl;
        if (cost < best_cost) {
            best_cost = cost;
            best_path = path;
        }
    }

    return best_path;
}
