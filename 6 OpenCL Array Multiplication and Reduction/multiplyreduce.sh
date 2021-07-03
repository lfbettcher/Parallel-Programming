#!/bin/bash
#SBATCH -J multiplyreduce 
#SBATCH -A cs475-575
#SBATCH -p class
#SBATCH --gres=gpu:1
#SBATCH -o multiplyreduce.out
#SBATCH -e multiplyreduce.err
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --mail-user=bettchem@oregonstate.edu

libOpenCL=/usr/local/apps/cuda/cuda-10.1/lib64/libOpenCL.so.1.1

# vary the size of the input array from 1K to 8M
num_elements=(1024 2048 4096 8192 16384 32768 65536 131072 262144 524288 1048576 2097152 4194304 8388608 33554432)
# local work sizes (at least 3, no smaller than 32, no larger than 256)
local_sizes=(32 64 128 256)

# print header
printf "\nDGX,Num elements\n" >> multiplyreduce.csv
printf "Local Work Size,," >> multiplyreduce.csv
for n in "${num_elements[@]}"
do
        printf "%d," $n >> multiplyreduce.csv
done
printf "\n" >> multiplyreduce.csv

# for each local work size
for s in "${local_sizes[@]}"
do
        printf ",%d," $s >> multiplyreduce.csv
        # for each global array size
        for n in "${num_elements[@]}"
        do
                echo NUM_ELEMENTS = $n LOCAL_SIZE = $s
                g++ -o multiplyreduce -DNUM_ELEMENTS=$n -DLOCAL_SIZE=$s multiplyreduce.cpp $libOpenCL -lm -fopenmp
                ./multiplyreduce >> multiplyreduce.csv
                echo ""
        done
        printf "\n" >> multiplyreduce.csv
done
