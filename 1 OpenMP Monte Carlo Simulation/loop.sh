#!/bin/bash

# number of threads:
for t in 1 2 4 8
do
	echo NUMT = $t
	# monte carlo trials:
	for s in 1 10 100 1000 5000 10000 100000 500000 1000000
	do
		echo NUMTRIALS = $s
		g++ -DNUMT=$t -DNUMTRIALS=$s Project1.cpp -o Project1 -lm -fopenmp
		./Project1 >> results.csv
		echo ""
	done
	printf "\n" >> results.csv
done
