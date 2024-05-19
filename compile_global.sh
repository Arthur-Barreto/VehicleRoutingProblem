#!/bin/bash
set -e

# Compile the utils.cpp file without OpenMP support
g++ -fopenmp -c utils.cpp

# Compile and link the global_search.cpp file without OpenMP support
g++ -fopenmp global_search.cpp utils.o -o global_search

# Compile and link the parallel_search.cpp file with OpenMP support
g++ -fopenmp parallel_search.cpp utils.o -o parallel_search
