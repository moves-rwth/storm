#include "basicValueIteration.h"

#include <iostream>
#include <chrono>

#include <cuda_runtime.h>
#include "cusparse_v2.h"


__global__ void cuda_kernel_basicValueIteration_mvReduce(int const * const A, int * const B) {
	*B = *A;
}

void basicValueIteration_mvReduce(uint_fast64_t const maxIterationCount, std::vector<uint_fast64_t> const& matrixRowIndices, std::vector<uint_fast64_t> const& matrixColumnIndices, std::vector<double> const& matrixValues, std::vector<double>& x, std::vector<double> const& b, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices) {
	std::cout << "basicValueIteration_mvReduce is implemented for ValueType == double :)" << std::endl;
}

/*
void kernelSwitchTest(size_t N) {
	int* deviceIntA;
	int* deviceIntB;

	if (cudaMalloc((void**)&deviceIntA, sizeof(int)) != cudaSuccess) {
		std::cout << "Error in cudaMalloc while allocating " << sizeof(int) << " Bytes!" << std::endl;
		return;
	}
	if (cudaMalloc((void**)&deviceIntB, sizeof(int)) != cudaSuccess) {
		std::cout << "Error in cudaMalloc while allocating " << sizeof(int) << " Bytes!" << std::endl;
		return;
	}

	// Allocate space on the device
	auto start_time = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < N; ++i) {
		cuda_kernel_kernelSwitchTest<<<1,1>>>(deviceIntA, deviceIntB);
	}
	auto end_time = std::chrono::high_resolution_clock::now();
	std::cout << "Switching the Kernel " << N << " times took " << std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() << "micros" << std::endl;
	std::cout << "Resulting in " << (std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() / ((double)(N))) << "Microseconds per Kernel Switch" << std::endl;

	// Free memory on device
	if (cudaFree(deviceIntA) != cudaSuccess) {
		std::cout << "Error in cudaFree!" << std::endl;
		return;
	}
	if (cudaFree(deviceIntB) != cudaSuccess) {
		std::cout << "Error in cudaFree!" << std::endl;
		return;
	}
}*/