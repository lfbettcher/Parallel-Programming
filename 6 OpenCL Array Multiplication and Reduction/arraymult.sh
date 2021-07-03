#!/bin/bash
#SBATCH -J first 
#SBATCH -A cs475-575
#SBATCH -p class
#SBATCH --gres=gpu:1
#SBATCH -o first.out
#SBATCH -e first.err
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --mail-user=bettchem@oregonstate.edu

libOpenCL=/usr/local/apps/cuda/cuda-10.1/lib64/libOpenCL.so.1.1

# vary the size of the input array from 1K to 8M
num_elements=(1024 2048 4096 8192 16384 32768 65536 131072 262144 524288 1048576 2097152 4194304 8388608 33554432)
# local work sizes (at least) 8 to 512
local_sizes=(8 16 32 64 128 256 512 1024)

# print header
printf "\nDGX,Num elements\n" >> arraymult.csv
printf "Local Work Size,," >> arraymult.csv
for n in "${num_elements[@]}"
do
        printf "%d," $n >> arraymult.csv
done
printf "\n" >> arraymult.csv

# for each local work size
for s in "${local_sizes[@]}"
do
        printf ",%d," $s >> arraymult.csv
        # for each global array size
        for n in "${num_elements[@]}"
        do
                echo NUM_ELEMENTS = $n LOCAL_SIZE = $s
                g++ -o arraymult -DNUM_ELEMENTS=$n -DLOCAL_SIZE=$s first.cpp $libOpenCL -lm -fopenmp
                ./arraymult >> arraymult.csv
                echo ""
        done
        printf "\n" >> arraymult.csv
done
