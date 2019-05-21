#!/bin/bash -l

sbatch -n 3 weak3.sh
sbatch -n 13 weak13.sh
sbatch -n 18 weak18.sh
sbatch -p node -n 25 weak25.sh

