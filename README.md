# VehicleRoutingProblem

Vehicle Routing Problem - Supercomputing Final Project

## Problem Description

The Vehicle Routing Problem (VRP) is a combinatorial optimization problem that seeks to find the best set of routes for a fleet of vehicles to traverse in order to deliver to a given set of customers. The objective is to minimize the total distance traveled by the vehicles. The problem is NP-hard and has many applications in real-world scenarios such as transportation and logistics.

## Solutions Proposed

- **Brute Force**: The brute force solution is a naive approach that generates all possible permutations of the customers and evaluates the total distance for each permutation. The solution with the minimum total distance is selected as the best solution. This approach is computationally expensive and not feasible for large instances of the problem.

- **Heuristic Algorithm**: The heuristic algorithm is a more efficient approach that uses a greedy strategy to construct the routes. In this algorithm, istead of checking all possible permutatins, the algorithm only check the max value between 2 and num_nodes! / 8.

- **Parallel OpenMP**: It optimize the `Brute Force` algorithm by using OpenMP to parallelize the computation of the total distance for each permutation. The algorithm divides the permutations into chunks and assigns each chunk to a thread for parallel computation. The total distance for each chunk is computed in parallel and the minimum total distance is selected as the best solution.

- **Parallel MPI**: It optimize the `Brute Force` algorithm by using MPI to parallelize the computation of the total distance for each permutation. The algorithm divides the permutations into chunks and assigns each chunk to a process for parallel computation. The total distance for each chunk is computed in parallel and the minimum total distance is selected as the best solution.

## Implementation

To see the details of the implementation, please refer to the source code in the `src` directory.

## How to Run

To compile the code, run the following command:

```bash
make
```

To delete the compiled files, run the following command:

```bash
make clean
```

The `mpi` solution is not on make file, so it need to be compiled with the `compile.sh` script. To compile the `mpi` solution.

- First, give the permission to the script:

```bash
chmod +x compile_mpi.sh
```

- Then, run the script:

```bash
./compile_mpi.sh
```

To run any of the solutions, run the following command:

```bash
./<solution> input_file<graph_orderr>.txt <capacity>
```

Where `<solution>` is the name of the solution to run (`global_search`, `heuristic_search`, `parallel_search`, `cluster_search`), `<graph_orderr>` is the order of the graph to be used as input, and `<capacity>` is the capacity of the vehicles.

## Results

On the notebook `time_compare.ipynb` we compare the time of execution of the different solutions for different graph orders. Go to the notebook to see the results.

## Input Files

The `input_files` are on the `input_files` directory. Thoose files where genrated using the nothebook given by the professor `Supercomp_Projeto_2024.ipynb`.

## Output Files from Cluster

The output files from the cluster are on the `cluster_log` directory. Also the **slurm** file is the `cluster_search.slurm`, which was used to generate the output files.

## References

- [Vehicle Routing Problem - YouTube](https://youtu.be/A1wsIFDKqBk)
- [Vrpy Library](https://vrpy.readthedocs.io/en/master/examples.html)

Also, this project used ai tools, like copilot and chatgpt to help with the code and the documentation.
