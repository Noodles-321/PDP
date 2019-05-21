#!/bin/bash -l

for i in 1 2 3 4 5 6 8 9 10 12 15 16; do
	sbatch -n $i 3600.sh
done
