#!/bin/bash
#SBATCH --partition=express
#SBATCH --nodes=2
#SBATCH --cpus-per-task=5
#SBATCH --mem=2G
#SBATCH --output=4_stores.out

mpirun ./cluster_search input_files/grafo4.txt 15
