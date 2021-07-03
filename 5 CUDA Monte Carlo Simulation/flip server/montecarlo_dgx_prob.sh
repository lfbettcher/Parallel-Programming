#!/bin/bash
#SBATCH -J MonteCarloProb
#SBATCH -A cs475-575
#SBATCH -p class
#SBATCH --gres=gpu:1
#SBATCH -o montecarloprob.out
#SBATCH -e montecarloprob.err
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --mail-user=bettchem@oregonstate.edu

CUDA_NVCC=/usr/local/apps/cuda/cuda-10.1/bin/nvcc

# number of threads per block (executes 32 at a time so 8 and 16 are bad)
blocksizes=(8 16 32 64 128)
# must be a multiple of 1024 (2K to 2M)
numtrials=(2048 4096 8192 16384 32768 65536 131072 262144 524288 1048576 2097152 4194304 8388608)

# print header 
printf "\n,DGX probability,Numtrials\n" >> dgx_results_prob.csv
printf "Blocksize,," >> dgx_results_prob.csv
for n in "${numtrials[@]}"
do
	printf "%d," $n >> dgx_results_prob.csv
done
printf "\n" >> dgx_results_prob.csv

# for each blocksize
for b in "${blocksizes[@]}"
do
	printf ",%d," $b >> dgx_results_prob.csv
	# for each numtrials
	for n in "${numtrials[@]}"
	do
		echo BLOCKSIZE = $b NUMTRIALS = $n
		$CUDA_NVCC -o montecarlo_prob -DBLOCKSIZE=$b -DNUMTRIALS=$n montecarlo_prob.cu
		./montecarlo_prob >> dgx_results_prob.csv
		echo ""
	done
	printf "\n" >> dgx_results_prob.csv
done
