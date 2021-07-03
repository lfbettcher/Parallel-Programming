#!/bin/bash

# Multicore+SIMD Multicore
methods=(1 2)
threads=(1 2 4)
arraysize=(1000 5000 10000 50000 100000 500000 1000000 4000000 8000000)

# print header row
printf "Method,Threads," > extracredit.csv
for n in "${arraysize[@]}"
do
  printf "%d," $n >> extracredit.csv
done
printf "\n" >> extracredit.csv

# for-loop with no multicore or SIMD
# run each array size
echo "no multicore or SIMD"
for n in "${arraysize[@]}"
do
  echo SIZE = $n
  g++ -o extracredit -DMETHOD=0 -DNUMT=1 -DARRAYSIZE=$n extracredit.cpp simd.asm.cpp -lm -fopenmp
	./extracredit >> extracredit.csv
	echo ""
done
printf "\n" >> extracredit.csv

# for methods Multicore+SIMD and Multicore
for m in "${methods[@]}"
do
  echo METHOD $m
  # number of threads
  for t in "${threads[@]}"
  do
    echo THREADS = $t
    # run each array size
    for n in "${arraysize[@]}"
    do
      echo SIZE = $n
      g++ -o extracredit -DMETHOD=$m -DNUMT=$t -DARRAYSIZE=$n extracredit.cpp simd.asm.cpp -lm -fopenmp
	    ./extracredit >> extracredit.csv
	    echo ""
    done
    printf "\n" >> extracredit.csv
  done
done

