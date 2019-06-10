#!/bin/bash -l

#SBATCH -A g2019005
#SBATCH -t 5:00 

module load gcc openmpi
mpirun matmul /proj/g2019005/nobackup/matmul_indata/input5716.txt output5716.txt
