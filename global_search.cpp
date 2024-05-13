#include "utils.h"
#include <climits>
using namespace std;

int factorial(int n) {
    if (n == 0 || n == 1)
        return 1;
    return n * factorial(n - 1);
}

void print_permutation(const vector<int> &perm) {
    for (int num : perm) {
        cout << num << " ";
    }
    cout << endl;
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

vector<vector<int>> valid_paths(vector<vector<int>> &paths, vector<vector<int>> &matrix, int &total_capacity, map<int, int> &node_order) {

    vector<vector<int>> possible_paths;

    int total_capacity_used = 0;

    for (int i = 0; i < paths.size(); i++) {
        vector<int> path = paths[i];
        vector<int> new_path;
        new_path.push_back(0); // Add the starting node (0) to the new path
        for (int j = 0; j < path.size() - 1; j++) {
            int node = path[j];
            int next_node = path[j + 1];
            if (matrix[node][next_node] == -1) {
                new_path.push_back(0);
                new_path.push_back(next_node);
                total_capacity_used = 0;
            } else {
                new_path.push_back(next_node);
                total_capacity_used += node_order[next_node];
                if (total_capacity_used > total_capacity) {
                    new_path.push_back(0);
                    new_path.push_back(next_node);
                    total_capacity_used = node_order[next_node];
                }
            }
        }
        if (total_capacity_used <= total_capacity) {
            possible_paths.push_back(new_path);
        }
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

int main() {

    vector<Edge> edges;
    map<int, int> node_order;

    read_graph("grafo.txt", &edges, &node_order);

    cout << "Number of edges: " << edges.size() << endl;
    cout << "Number of nodes: " << node_order.size() + 1 << endl;

    int num_nodes = node_order.size() + 1;
    vector<vector<int>> matrix(num_nodes);

    generate_matrix(edges, matrix);

    for (int i = 0; i < num_nodes; i++) {
        for (int j = 0; j < num_nodes; j++) {
            cout << matrix[i][j] << " ";
        }
        cout << "\n";
    }

    int capacity = 15;
    vector<vector<int>> routes;

    int total_permutations = factorial(num_nodes - 1);

    for (int i = 1; i <= total_permutations; i++) {
        vector<int> paths = get_permutations(num_nodes - 1, i);
        {
            routes.push_back(paths);
        }
    }

    // print the routes
    for (int i = 0; i < routes.size(); i++) {
        cout << "Route " << i << ": ";
        for (int j = 0; j < routes[i].size(); j++) {
            cout << routes[i][j] << " ";
        }
        cout << endl;
    }

    vector<vector<int>> valid_routes = valid_paths(routes, matrix, capacity, node_order);

    // print the valid routes
    for (int i = 0; i < valid_routes.size(); i++) {
        cout << "Valid Route " << i << ": ";
        for (int j = 0; j < valid_routes[i].size(); j++) {
            cout << valid_routes[i][j] << " ";
        }
        cout << endl;
    }

    // get the best path
    vector<int> best_route = best_path(matrix, valid_routes);

    // print the best route
    cout << "Best Route: ";
    for (int i = 0; i < best_route.size(); i++) {
        cout << best_route[i] << " ";
    }
    cout << endl;

    // compute the cost from the best route
    int cost = 0;
    for (int i = 0; i < best_route.size() - 1; i++) {
        int node = best_route[i];
        int next_node = best_route[i + 1];
        cost += matrix[node][next_node];
    }

    cout << "Cost of the best route: " << cost << endl;

    return 0;
}