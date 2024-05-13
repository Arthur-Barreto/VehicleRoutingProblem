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