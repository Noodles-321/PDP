#!/bin/bash
for i in {1..32}
do
	echo "$i in 32"
	(mpirun -np $i 2dintegral 100000000) >> time_threads.txt
done
