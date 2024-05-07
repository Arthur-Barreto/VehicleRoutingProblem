#include "utils.h"
using namespace std;

int main() {

    vector<Edge> edges;
    map<int, int> node_order;

    read_graph("grafo.txt", &edges, &node_order);

    cout << "Edges:" << endl;
    for (const auto &edge : edges) {
        cout << get<0>(edge) << " " << get<1>(edge) << " " << get<2>(edge) << endl;
    }

    cout << "Node order:" << endl;
    for (const auto &node : node_order) {
        cout << node.first << " " << node.second << endl;
    }

    // print the numedges and numnodes
    cout << "Number of edges: " << edges.size() << endl;
    cout << "Number of nodes: " << node_order.size() << endl;

    // generate the matrix
    int num_nodes = node_order.size() + 1;
    vector<vector<int>> matrix(num_nodes);

    // Call generate matrix
    generate_matrix(edges, matrix);

    // Print the matrix
    for (int i = 0; i < num_nodes; i++) {
        for (int j = 0; j < num_nodes; j++) {
            cout << matrix[i][j] << " ";
        }
        cout << "\n";
    }

    // for (int i = 0; i < num_nodes; i++) {
    //     for (int j = 0; j < num_nodes; j++) {
    //         matrix[i][j] = -1;
    //     }
    // }

    // // fill the matrix using the edges information
    // // line column value
    // for (const auto &edge : edges) {
    //     matrix[get<0>(edge)][get<1>(edge)] = get<2>(edge);
    // }

    // // print the matrix

    // cout << "Matrix:" << endl;
    // for (int i = 0; i < num_nodes; i++) {
    //     for (int j = 0; j < num_nodes; j++) {
    //         cout << matrix[i][j] << " ";
    //     }
    //     cout << endl;
    // }

    return 0;
}