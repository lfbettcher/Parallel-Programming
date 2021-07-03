#!/bin/bash

threads=(1 2 4 8)
nodes=(1 2 4 8 16 30 50 100 200 400 1000 5000)

# print header row
printf "," >> results.csv
for n in "${nodes[@]}"
do
  printf "%d," $n >> results.csv
done
printf "\n" >> results.csv

# number of threads:
for t in "${threads[@]}"
do
  printf "%d," $t >> results.csv
	echo NUMT = $t
	# number of nodes:
	for s in "${nodes[@]}"
  do
		echo NUMNODES = $s
		g++ -DNUMT=$t -DNUMNODES=$s Project2.cpp -o Project2 -lm -fopenmp
		./Project2 >> results.csv
		echo ""
	done
	printf "\n" >> results.csv
done
