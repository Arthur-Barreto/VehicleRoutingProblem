#!/bin/bash
#SBATCH --partition=espec
#SBATCH --nodes=2
#SBATCH --cpus-per-task=5
#SBATCH --mem=2G
#SBATCH --output=cluster_log/12_stores.out

mpirun ./cluster_search input_files/grafo12.txt 15
