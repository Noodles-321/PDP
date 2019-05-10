#!/bin/bash -l
#SBATCH -A g2019005
#SBATCH -t 10:00

module load gcc openmpi
mpirun -np 16 ./quicksort /proj/g2019005/nobackup/qsort_indata/input125000000.txt output 1
