CUDA_PATH		=	/usr/local/cuda-11.0
CUDA_BIN_PATH	=	$(CUDA_PATH)/bin
CUDA_NVCC		=	$(CUDA_BIN_PATH)/nvcc
CUDA_HEADERS	=	$(CUDA_PATH)/samples/common/inc

montecarlo:		montecarlo.cu
				$(CUDA_NVCC) -o montecarlo montecarlo.cu -I$(CUDA_HEADERS)

