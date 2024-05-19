#include "utils.h"
using namespace std;

int main(int argc, char *argv[]) {

    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <graph_file>" << endl;
        return 1;
    }

    string graph_file = argv[1];

    vector<Edge> edges;
    map<int, int> node_order;

    read_graph(graph_file, &edges, &node_order);

    cout << "Number of edges: " << edges.size() << endl;
    cout << "Number of nodes: " << node_order.size() + 1 << endl;

    int num_nodes = node_order.size() + 1;
    vector<vector<int>> matrix(num_nodes);

    generate_matrix(edges, matrix);

    // for (int i = 0; i < num_nodes; i++) {
    //     for (int j = 0; j < num_nodes; j++) {
    //         cout << matrix[i][j] << " ";
    //     }
    //     cout << "\n";
    // }

    int capacity = 15;
    vector<vector<int>> routes;

    int total_permutations = factorial(num_nodes - 1);

    #pragma omp parallel for
    for (int i = 1; i <= total_permutations; i++) {
        vector<int> paths = get_permutations_parallel(num_nodes - 1, i);
        {
            #pragma omp critical
            {
                routes.push_back(paths);
            }
        }
    }

    // print the routes
    // for (int i = 0; i < routes.size(); i++) {
    //     cout << "Route " << i << ": ";
    //     for (int j = 0; j < routes[i].size(); j++) {
    //         cout << routes[i][j] << " ";
    //     }
    //     cout << endl;
    // }

    vector<vector<int>> valid_routes = valid_paths_parallel(routes, matrix, capacity, node_order);

    // print the valid routes
    // for (int i = 0; i < valid_routes.size(); i++) {
    //     cout << "Valid Route " << i << ": ";
    //     for (int j = 0; j < valid_routes[i].size(); j++) {
    //         cout << valid_routes[i][j] << " ";
    //     }
    //     cout << endl;
    // }

    // get the best path
    vector<int> best_route = best_path_parallel(matrix, valid_routes);

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