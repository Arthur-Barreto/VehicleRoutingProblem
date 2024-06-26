#include <algorithm>
#include <chrono>
#include <climits>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <mpi.h>
#include <sstream>
#include <tuple>
#include <vector>

using namespace std;

typedef tuple<int, int, int> Edge; // Define a type alias for the edge tuple

// Helper function to distribute tasks
void distribute_tasks(int num_tasks, int num_procs, int rank, int &start_task, int &end_task);
void read_graph(string filename, vector<Edge> *edges, map<int, int> *node_order);
void generate_matrix(const vector<Edge> &edges, vector<vector<int>> &matrix);
vector<int> get_permutations(int n, int index);
vector<vector<int>> valid_paths(vector<vector<int>> &paths, vector<vector<int>> &matrix, int total_capacity, map<int, int> &node_order, MPI_Comm comm);
int factorial(int n);
vector<int> best_path(vector<vector<int>> &matrix, vector<vector<int>> &paths);
double compute_cost(const vector<int> &path, const vector<vector<int>> &matrix);

int main(int argc, char *argv[]) {

    // START THE CHONOS CLOCK
    auto start = chrono::high_resolution_clock::now();

    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 3) {
        if (rank == 0) {
            cout << "Usage: " << argv[0] << " <graph_file> <capacity>" << endl;
        }
        MPI_Finalize();
        return 1;
    }

    string graph_file = argv[1];
    int capacity = atoi(argv[2]);

    vector<Edge> edges;
    map<int, int> node_order;
    int num_nodes;

    if (rank == 0) {
        read_graph(graph_file, &edges, &node_order);
        num_nodes = node_order.size() + 1;

        // cout << "Number of edges: " << edges.size() << endl;
        // cout << "Number of nodes: " << num_nodes << endl;
    }

    // Broadcast num_nodes to all processes
    MPI_Bcast(&num_nodes, 1, MPI_INT, 0, MPI_COMM_WORLD);

    vector<vector<int>> matrix(num_nodes, vector<int>(num_nodes, -1));
    if (rank == 0) {
        generate_matrix(edges, matrix);

        // for (int i = 0; i < num_nodes; i++) {
        //     for (int j = 0; j < num_nodes; j++) {
        //         cout << matrix[i][j] << " ";
        //     }
        //     cout << "\n";
        // }
    }

    for (int i = 0; i < num_nodes; i++) {
        MPI_Bcast(matrix[i].data(), num_nodes, MPI_INT, 0, MPI_COMM_WORLD);
    }

    int total_permutations = factorial(num_nodes - 1);
    int start_task, end_task;
    distribute_tasks(total_permutations, size, rank, start_task, end_task);

    vector<vector<int>> local_permutations;

    for (int i = start_task; i <= end_task; i++) {
        local_permutations.push_back(get_permutations(num_nodes - 1, i));
    }

    // Flatten local permutations
    vector<int> local_permutations_flat;
    for (const auto &perm : local_permutations) {
        local_permutations_flat.insert(local_permutations_flat.end(), perm.begin(), perm.end());
    }

    // Prepare to gather all permutations
    int local_size = local_permutations_flat.size();
    vector<int> recv_counts(size);
    MPI_Allgather(&local_size, 1, MPI_INT, recv_counts.data(), 1, MPI_INT, MPI_COMM_WORLD);

    vector<int> displs(size, 0);
    int total_size = recv_counts[0];
    for (int i = 1; i < size; i++) {
        displs[i] = displs[i - 1] + recv_counts[i - 1];
        total_size += recv_counts[i];
    }

    vector<int> all_permutations(total_size);
    MPI_Allgatherv(local_permutations_flat.data(), local_size, MPI_INT,
                   all_permutations.data(), recv_counts.data(), displs.data(), MPI_INT, MPI_COMM_WORLD);

    vector<vector<int>> all_permutations_2d(total_permutations, vector<int>(local_permutations[0].size()));
    int offset = 0;
    for (int i = 0; i < total_permutations; ++i) {
        copy(all_permutations.begin() + offset, all_permutations.begin() + offset + local_permutations[0].size(), all_permutations_2d[i].begin());
        offset += local_permutations[0].size();
    }

    // print all permutations
    // if (rank == 0) {
    //     for (int i = 0; i < all_permutations_2d.size(); i++) {
    //         cout << "Permutation " << i << ": ";
    //         for (int j = 0; j < all_permutations_2d[i].size(); j++) {
    //             cout << all_permutations_2d[i][j] << " ";
    //         }
    //         cout << endl;
    //     }
    // }

    // Calculate valid paths locally
    vector<vector<int>> local_valid_paths = valid_paths(local_permutations, matrix, capacity, node_order, MPI_COMM_WORLD);

    // print the local valid paths and the rank
    // cout << "better Rank: " << rank << endl;
    // for (int i = 0; i < local_valid_paths.size(); i++) {
    //     cout << "Path " << i << ": ";
    //     for (int j = 0; j < local_valid_paths[i].size(); j++) {
    //         cout << local_valid_paths[i][j] << " ";
    //     }
    //     cout << endl;
    // }

    // compute the best path locally
    vector<int> local_best_path = best_path(matrix, local_valid_paths);
    double local_best_cost = compute_cost(local_best_path, matrix);

    // print the best local path and the rank
    // cout << "Rank: " << rank << endl;
    // cout << "Best Path: ";
    // for (int i = 0; i < local_best_path.size(); i++) {
    //     cout << local_best_path[i] << " ";
    // }
    // cout << endl;

    // Gather all costs at the root process
    vector<double> all_costs(size);
    MPI_Gather(&local_best_cost, 1, MPI_DOUBLE, all_costs.data(), 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // If rank 0, determine the process with the minimum cost
    int min_cost_index = 0;
    if (rank == 0) {
        min_cost_index = min_element(all_costs.begin(), all_costs.end()) - all_costs.begin();
        // cout << "Process " << min_cost_index << " has the minimum cost: " << all_costs[min_cost_index] << endl;
    }

    // Broadcast the index of the process with the minimum cost
    MPI_Bcast(&min_cost_index, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Gather the best path from the process with the minimum cost
    vector<int> global_best_path;
    if (rank == min_cost_index) {
        // Send the size of the path first
        int path_size = local_best_path.size();
        MPI_Send(&path_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(local_best_path.data(), path_size, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    if (rank == 0) {
        int path_size;
        MPI_Recv(&path_size, 1, MPI_INT, min_cost_index, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        global_best_path.resize(path_size);
        MPI_Recv(global_best_path.data(), path_size, MPI_INT, min_cost_index, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Print the global best path
        cout << "Global Best Path from Process " << min_cost_index << ": ";
        for (int i = 0; i < global_best_path.size(); i++) {
            cout << global_best_path[i] << " ";
        }
        cout << endl;
    }

    MPI_Finalize();

    // STOP THE CHRONO CLOCK
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    if (rank == 0) {
        cout << "Time taken: " << duration.count() << " milliseconds" << endl;
    }

    return 0;
}

void read_graph(string filename, vector<Edge> *edges, map<int, int> *node_order) {
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

void generate_matrix(const vector<Edge> &edges, vector<vector<int>> &matrix) {
    int n = matrix.size();

    // Initialize the matrix with -1
    for (int i = 0; i < n; i++) {
        matrix[i].resize(n, -1);
    }

    // Fill the matrix using the edges information
    for (const auto &edge : edges) {
        int u = get<0>(edge);
        int v = get<1>(edge);
        int w = get<2>(edge);
        matrix[u][v] = w;
    }
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

vector<vector<int>> valid_paths(vector<vector<int>> &paths, vector<vector<int>> &matrix, int total_capacity, map<int, int> &node_order, MPI_Comm comm) {
    vector<vector<int>> possible_paths;
    int n = paths.size();

    for (int i = 0; i < n; i++) {
        vector<int> new_path;
        new_path.push_back(0); // Start with depot (0)
        int total_capacity_used = 0;

        for (size_t j = 0; j < paths[i].size() - 1; j++) {
            int node = paths[i][j];
            int next_node = paths[i][j + 1];

            if (matrix[node][next_node] != -1 && total_capacity_used + node_order[next_node] <= total_capacity) {
                new_path.push_back(next_node);
                total_capacity_used += node_order[next_node];
            } else {
                if (new_path.back() != 0) {
                    new_path.push_back(0);
                }
                total_capacity_used = 0;

                if (matrix[0][next_node] != -1) {
                    new_path.push_back(next_node);
                    total_capacity_used += node_order[next_node];
                }
            }
        }

        if (new_path.back() != 0) {
            new_path.push_back(0);
        }

        possible_paths.push_back(new_path);
    }

    return possible_paths;
}

vector<int> best_path(vector<vector<int>> &matrix, vector<vector<int>> &paths) {
    vector<int> best_path;
    int best_cost = INT_MAX;

    for (size_t i = 0; i < paths.size(); i++) {
        vector<int> path = paths[i];
        int cost = 0;
        for (size_t j = 0; j < path.size() - 1; j++) {
            int node = path[j];
            int next_node = path[j + 1];
            cost += matrix[node][next_node];
        }
        if (cost < best_cost) {
            best_cost = cost;
            best_path = path;
        }
    }

    return best_path;
}

double compute_cost(const vector<int> &path, const vector<vector<int>> &matrix) {
    double cost = 0.0;
    for (size_t i = 0; i < path.size() - 1; ++i) {
        cost += matrix[path[i]][path[i + 1]];
    }
    return cost;
}

int factorial(int n) {
    if (n == 0 || n == 1)
        return 1;
    return n * factorial(n - 1);
}

void distribute_tasks(int num_tasks, int num_procs, int rank, int &start_task, int &end_task) {
    int tasks_per_proc = num_tasks / num_procs;
    int remaining_tasks = num_tasks % num_procs;

    start_task = rank * tasks_per_proc + min(rank, remaining_tasks) + 1;
    end_task = start_task + tasks_per_proc - 1;

    if (rank < remaining_tasks) {
        end_task++;
    }
}