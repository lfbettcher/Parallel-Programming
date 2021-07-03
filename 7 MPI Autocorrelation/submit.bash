#!/bin/bash

num_cpus=(1 2 4 8 16 32)

mpic++ -o project7 project7.cpp

for n in "${num_cpus[@]}"
do
	mpiexec -mca btl self,tcp -np $n project7 >> performance.csv
done
