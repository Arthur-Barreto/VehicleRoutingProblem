#include <algorithm>
#include <iostream>
#include <omp.h>
#include <vector>

using namespace std;

vector<vector<int>> valid_paths(vector<vector<int>> &paths, vector<vector<int>> &matrix, int total_capacity, map<int, int> &node_order) {
    vector<vector<int>> possible_paths;
    int n = paths.size();

    // Preparing a container to hold paths processed by each thread
    vector<vector<vector<int>>> temp_paths(omp_get_max_threads());

#pragma omp parallel for shared(paths, matrix, node_order, total_capacity)
    for (int i = 0; i < n; i++) {
        vector<int> new_path;
        new_path.push_back(0); // Start with depot (0)
        int total_capacity_used = 0;

        for (int j = 0; j < paths[i].size() - 1; j++) {
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

vector<int> index_to_permutation(int n, int index) {
    vector<int> factorial(n, 1);
    vector<int> permutation;
    vector<int> elements;

    // Calculate factorial numbers on-the-fly and initialize elements
    for (int i = 1; i < n; i++) {
        factorial[i] = factorial[i - 1] * i;
        elements.push_back(i);
    }
    elements.push_back(n);

    // Build the permutation using the factorial number system
    index--; // to convert to 0-based index
    for (int i = n - 1; i >= 0; i--) {
        int j = index / factorial[i];
        index %= factorial[i];
        permutation.push_back(elements[j]);
        elements.erase(elements.begin() + j);
    }

    return permutation;
}

void print_permutation(const vector<int> &perm) {
    for (int num : perm) {
        cout << num << " ";
    }
    cout << endl;
}

int factorial(int n) {
    if (n == 0 || n == 1)
        return 1;
    return n * factorial(n - 1);
}

int main() {
    int n = 4;
    int total_permutations = factorial(n);

    for (int i = 1; i <= total_permutations; i++) {
        vector<int> perm = index_to_permutation(n, i);
        {
            cout << "Permutation " << i << ": ";
            print_permutation(perm);
        }
    }

    return 0;
}
