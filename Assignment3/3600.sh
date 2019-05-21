#!/bin/bash -l

#SBATCH -A g2019005
#SBATCH -t 10:00 

module load gcc openmpi
mpirun matmul /proj/g2019005/nobackup/matmul_indata/input3600.txt output3600.txt
