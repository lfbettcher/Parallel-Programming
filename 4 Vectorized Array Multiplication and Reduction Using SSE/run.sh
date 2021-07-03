#!/bin/bash

arraysize=(1000 5000 10000 50000 100000 500000 1000000 4000000 8000000)
# method=(NonSimdMul SimdMul NonSimdMulSum SimdMulSum)
method=(0 1 2 3)

# print header row
printf "ArraySize," > results.csv
for n in "${arraysize[@]}"
do
  printf "%d," $n >> results.csv
done
printf "\n" >> results.csv

# run each method
for m in "${method[@]}"
do
  echo METHOD = $m
  # run each array size
  for n in "${arraysize[@]}"
  do
    echo SIZE = $n
    g++ -o arraymult -DMETHOD=$m -DSIZE=$n arraymult.cpp simd.asm.cpp -lm -fopenmp
	  ./arraymult >> results.csv
	  echo ""
  done
  printf "\n" >> results.csv
done

