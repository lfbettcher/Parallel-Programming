#!/bin/bash

arraysize=(1000 5000 10000 50000 100000 500000 1000000 4000000 8000000)
# method=(NonSimdMul SimdMul NonSimdMulSum SimdMulSum)
method=(0 1 2 3)

# print header row
printf "ArraySize," > intrinsics_results.csv
for n in "${arraysize[@]}"
do
  printf "%d," $n >> intrinsics_results.csv
done
printf "\n" >> intrinsics_results.csv

# run each method
for m in "${method[@]}"
do
  echo METHOD = $m
  # run each array size
  for n in "${arraysize[@]}"
  do
    echo SIZE = $n
    g++ -o arraymult_intrinsics -DMETHOD=$m -DSIZE=$n arraymult.cpp simd.intrinsics.cpp -lm -fopenmp
	  ./arraymult_intrinsics >> intrinsics_results.csv
	  echo ""
  done
  printf "\n" >> intrinsics_results.csv
done
