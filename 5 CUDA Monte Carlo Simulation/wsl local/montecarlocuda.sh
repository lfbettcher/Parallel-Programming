#!/bin/bash

CUDA_PATH=/usr/local/cuda
CUDA_BIN_PATH=$CUDA_PATH/bin
CUDA_NVCC=$CUDA_BIN_PATH/nvcc
CUDA_HEADERS=$CUDA_PATH/samples/common/inc

# number of threads per block (executes 32 at a time so 8 and 16 are bad)
blocksizes=(8 16 32 64 128)
# must be a multiple of 1024 (2K to 2M)
numtrials=(2048 4096 8192 16384 32768 65536 131072 262144 524288 1048576 2097152)

# print header
printf "\n,WSL,Numtrials\n" >> results.csv
printf "Blocksize,," >> results.csv
for n in "${numtrials[@]}"
do
  printf "%d," $n >> results.csv
done
printf "\n" >> results.csv

# for each blocksize
for b in "${blocksizes[@]}"
do
  printf ",%d," $b >> results.csv
  # for each numtrials
  for n in "${numtrials[@]}"
  do
    echo BLOCKSIZE = $b NUMTRIALS = $n
    $CUDA_NVCC -o montecarlo -DBLOCKSIZE=$b -DNUMTRIALS=$n montecarlo.cu -I$CUDA_HEADERS
    ./montecarlo >> results.csv
	  echo ""
  done
  printf "\n" >> results.csv
done

