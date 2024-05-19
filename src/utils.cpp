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

        for (std::vector<int>::size_type j = 0; j < paths[i].size() - 1; j++) {
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

    for (std::vector<int>::size_type i = 0; i < paths.size(); i++) {
        vector<int> path = paths[i];
        int cost = 0;
        for (std::vector<int>::size_type j = 0; j < path.size() - 1; j++) {
            int node = path[j];
            int next_node = path[j + 1];
            cost += matrix[node][next_node];
        }
        // cout << "Cost of path " << i << ": " << cost << endl;
        if (cost < best_cost) {
            best_cost = cost;
            best_path = path;
        }
    }

    return best_path;
}

vector<int> get_permutations_parallel(int n, int index) {
    vector<int> factorial(n, 1);
    vector<int> permutation;
    vector<int> elements;

#pragma omp parallel for
    for (int i = 1; i < n; i++) {
        factorial[i] = factorial[i - 1] * i;
        elements.push_back(i);
    }
    elements.push_back(n);

    index--; // to convert to 0-based index
#pragma omp parallel for
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

vector<vector<int>> valid_paths_parallel(vector<vector<int>> &paths, vector<vector<int>> &matrix, int total_capacity, map<int, int> &node_order) {
    vector<vector<int>> possible_paths;
    int n = paths.size();

    // Preparing a container to hold paths processed by each thread
    vector<vector<vector<int>>> temp_paths(omp_get_max_threads());

#pragma omp parallel for shared(paths, matrix, node_order, total_capacity)
    for (int i = 0; i < n; i++) {
        vector<int> new_path;
        new_path.push_back(0); // Start with depot (0)
        int total_capacity_used = 0;

        for (std::vector<int>::size_type j = 0; j < paths[i].size() - 1; j++) {
            int node = paths[i][j];
            int next_node = paths[i][j + 1];

            if (matrix[node][next_node] == -1 || total_capacity_used + node_order[next_node] > total_capacity) {
                if (new_path.back() != 0) { // Prevent consecutive zeros
                    new_path.push_back(0);  // Return to depot to reset capacity
                }
                total_capacity_used = 0; // Reset capacity after returning to depot

                if (matrix[0][next_node] == -1) { // If no path from depot to next_node, skip
                    continue;
                }
            }

            if (total_capacity_used + node_order[next_node] <= total_capacity) {
                new_path.push_back(next_node);
                total_capacity_used += node_order[next_node];
            }
        }

        if (new_path.back() != 0) { // Avoid appending 0 if already at depot
            new_path.push_back(0);
        }

        int thread_id = omp_get_thread_num();
        temp_paths[thread_id].push_back(new_path);
    }

    // Combine results from all threads
    for (auto &thread_paths : temp_paths) {
        possible_paths.insert(possible_paths.end(), thread_paths.begin(), thread_paths.end());
    }

    return possible_paths;
}

vector<int> best_path_parallel(vector<vector<int>> &matrix, vector<vector<int>> &paths) {

    vector<int> best_path;
    int best_cost = INT_MAX;

#pragma omp parallel for
    for (std::vector<int>::size_type i = 0; i < paths.size(); i++) {
        vector<int> path = paths[i];
        int cost = 0;
        for (std::vector<int>::size_type j = 0; j < path.size() - 1; j++) {
            int node = path[j];
            int next_node = path[j + 1];
            cost += matrix[node][next_node];
        }
#pragma omp critical
        {
            if (cost < best_cost) {
                best_cost = cost;
                best_path = path;
            }
        }
    }

    return best_path;
}

vector<vector<int>> valid_paths_heurist(vector<vector<int>> &paths, vector<vector<int>> &matrix, int total_capacity, map<int, int> &node_order) {
    vector<vector<int>> possible_paths;

    // Shuffle the paths randomly
    random_shuffle(paths.begin(), paths.end());

    // Determine the number of paths to consider (half of the total paths)
    int num_paths = paths.size();
    int num_paths_to_consider = max(1, min(num_paths, static_cast<int>(num_paths * 0.2))); // 20% of total paths or 1 if less than 5% of total paths

    for (int i = 0; i < num_paths_to_consider; i++) {
        const auto &path = paths[i];
        vector<int> new_path;
        new_path.push_back(0); // Start with depot (0)
        int total_capacity_used = 0;

        for (std::vector<int>::size_type j = 0; j < path.size() - 1; j++) {
            int node = path[j];
            int next_node = path[j + 1];

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

vector<int> best_path_heuristic(vector<vector<int>> &matrix, vector<vector<int>> &paths) {

    vector<int> best_path;
    int best_cost = INT_MAX;

    // Sort the paths based on a heuristic
    sort(paths.begin(), paths.end(), [&](const vector<int> &path1, const vector<int> &path2) {
        // Calculate the cost of each path
        int cost1 = 0;
        for (std::vector<int>::size_type j = 0; j < path1.size() - 1; j++) {
            int node = path1[j];
            int next_node = path1[j + 1];
            cost1 += matrix[node][next_node];
        }

        int cost2 = 0;
        for (std::vector<int>::size_type j = 0; j < path2.size() - 1; j++) {
            int node = path2[j];
            int next_node = path2[j + 1];
            cost2 += matrix[node][next_node];
        }

        // Sort in ascending order of cost
        return cost1 < cost2;
    });

    // Determine the number of paths to consider
    int num_paths = paths.size();
    int num_paths_to_consider = max(1, min(num_paths, static_cast<int>(num_paths * 0.05))); // 5% of total paths or 1 if less than 5% of total paths

    // Iterate over the paths up to the specified number
    for (int i = 0; i < num_paths_to_consider; i++) {
        vector<int> path = paths[i];
        int cost = 0;
        for (std::vector<int>::size_type j = 0; j < path.size() - 1; j++) {
            int node = path[j];
            int next_node = path[j + 1];
            cost += matrix[node][next_node];
        }

        // Update the best path if the cost is lower
        if (cost < best_cost) {
            best_cost = cost;
            best_path = path;
        }
    }

    return best_path;
}