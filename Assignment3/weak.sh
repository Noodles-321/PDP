#!/bin/bash -l

sbatch -n 1 weak1.sh
sbatch -n 4 weak4.sh
sbatch -n 9 weak9.sh
sbatch -n 16 weak16.sh
sbatch -p node -n 25 weak25.sh

