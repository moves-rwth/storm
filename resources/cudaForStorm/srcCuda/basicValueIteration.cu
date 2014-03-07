#include "basicValueIteration.h"

#include <iostream>
#include <chrono>

#include <cuda_runtime.h>
#include "cusparse_v2.h"


__global__ void cuda_kernel_basicValueIteration_mvReduce(int const * const A, int * const B) {
	*B = *A;
}

void cudaForStormTestFunction(int a, int b) {
	std::cout << "Cuda for Storm: a + b = " << (a+b) << std::endl;
}

void basicValueIteration_mvReduce(uint_fast64_t const maxIterationCount, std::vector<uint_fast64_t> const& matrixRowIndices, std::vector<std::pair<uint_fast64_t, double>> columnIndicesAndValues, std::vector<double>& x, std::vector<double> const& b, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices) {
	if (sizeof(double) != sizeof(uint_fast64_t)) {
		std::cout << "FATAL ERROR - Internal Sizes of Double and uint_fast64_t do NOT match, CUDA acceleration not possible!" << std::endl;
		return;
	}
	
	uint_fast64_t* device_matrixRowIndices = nullptr;
	uint_fast64_t* device_matrixColIndicesAndValues = nullptr;
	double* device_x = nullptr;
	double* device_b = nullptr;
	double* device_multiplyResult = nullptr;
	uint_fast64_t* device_nondeterministicChoiceIndices = nullptr;

	cudaError_t cudaMallocResult;

	cudaMallocResult = cudaMalloc<uint_fast64_t>(&device_matrixRowIndices, matrixRowIndices.size());
	if (cudaMallocResult != cudaSuccess) {
		std::cout << "Could not allocate memory for Matrix Row Indices, Error Code " << cudaMallocResult << "." << std::endl;
		goto cleanup;
	}

	cudaMallocResult = cudaMalloc<uint_fast64_t>(&device_matrixColIndicesAndValues, columnIndicesAndValues.size() * 2);
	if (cudaMallocResult != cudaSuccess) {
		std::cout << "Could not allocate memory for Matrix Column Indices and Values, Error Code " << cudaMallocResult << "." << std::endl;
		goto cleanup;
	}

	cudaMallocResult = cudaMalloc<double>(&device_x, x.size());
	if (cudaMallocResult != cudaSuccess) {
		std::cout << "Could not allocate memory for Vector x, Error Code " << cudaMallocResult << "." << std::endl;
		goto cleanup;
	}

	cudaMallocResult = cudaMalloc<double>(&device_b, b.size());
	if (cudaMallocResult != cudaSuccess) {
		std::cout << "Could not allocate memory for Vector b, Error Code " << cudaMallocResult << "." << std::endl;
		goto cleanup;
	}

	cudaMallocResult = cudaMalloc<double>(&device_multiplyResult, b.size());
	if (cudaMallocResult != cudaSuccess) {
		std::cout << "Could not allocate memory for Vector multiplyResult, Error Code " << cudaMallocResult << "." << std::endl;
		goto cleanup;
	}

	cudaMallocResult = cudaMalloc<uint_fast64_t>(&device_nondeterministicChoiceIndices, nondeterministicChoiceIndices.size());
	if (cudaMallocResult != cudaSuccess) {
		std::cout << "Could not allocate memory for Nondeterministic Choice Indices, Error Code " << cudaMallocResult << "." << std::endl;
		goto cleanup;
	}

	// Memory allocated, copy data to device
	cudaError_t cudaCopyResult;

	cudaCopyResult = cudaMemcpy(device_matrixRowIndices, matrixRowIndices.data(), sizeof(uint_fast64_t) * matrixRowIndices.size(), cudaMemcpyHostToDevice);
	if (cudaCopyResult != cudaSuccess) {
		std::cout << "Could not copy data for Matrix Row Indices, Error Code " << cudaCopyResult << std::endl;
		goto cleanup;
	}

	cudaCopyResult = cudaMemcpy(device_matrixColIndicesAndValues, columnIndicesAndValues.data(), (sizeof(uint_fast64_t) * columnIndicesAndValues.size()) + (sizeof(double) * columnIndicesAndValues.size()), cudaMemcpyHostToDevice);
	if (cudaCopyResult != cudaSuccess) {
		std::cout << "Could not copy data for Matrix Column Indices and Values, Error Code " << cudaCopyResult << std::endl;
		goto cleanup;
	}

	cudaCopyResult = cudaMemcpy(device_x, x.data(), sizeof(double) * x.size(), cudaMemcpyHostToDevice);
	if (cudaCopyResult != cudaSuccess) {
		std::cout << "Could not copy data for Vector x, Error Code " << cudaCopyResult << std::endl;
		goto cleanup;
	}

	cudaCopyResult = cudaMemcpy(device_b, b.data(), sizeof(double) * b.size(), cudaMemcpyHostToDevice);
	if (cudaCopyResult != cudaSuccess) {
		std::cout << "Could not copy data for Vector b, Error Code " << cudaCopyResult << std::endl;
		goto cleanup;
	}

	cudaCopyResult = cudaMemcpy(device_nondeterministicChoiceIndices, nondeterministicChoiceIndices.data(), sizeof(uint_fast64_t) * nondeterministicChoiceIndices.size(), cudaMemcpyHostToDevice);
	if (cudaCopyResult != cudaSuccess) {
		std::cout << "Could not copy data for Vector b, Error Code " << cudaCopyResult << std::endl;
		goto cleanup;
	}

	// Data is on device, start Kernel

	// All code related to freeing memory and clearing up the device
cleanup:
	if (device_matrixRowIndices != nullptr) {
		cudaError_t cudaFreeResult = cudaFree(device_matrixRowIndices);
		if (cudaFreeResult != cudaSuccess) {
			std::cout << "Could not free Memory of Matrix Row Indices, Error Code " << cudaFreeResult << "." << std::endl;
		}
		device_matrixRowIndices = nullptr;
	}
	if (device_matrixColIndicesAndValues != nullptr) {
		cudaError_t cudaFreeResult = cudaFree(device_matrixColIndicesAndValues);
		if (cudaFreeResult != cudaSuccess) {
			std::cout << "Could not free Memory of Matrix Column Indices and Values, Error Code " << cudaFreeResult << "." << std::endl;
		}
		device_matrixColIndicesAndValues = nullptr;
	}
	if (device_x != nullptr) {
		cudaError_t cudaFreeResult = cudaFree(device_x);
		if (cudaFreeResult != cudaSuccess) {
			std::cout << "Could not free Memory of Vector x, Error Code " << cudaFreeResult << "." << std::endl;
		}
		device_x = nullptr;
	}
	if (device_b != nullptr) {
		cudaError_t cudaFreeResult = cudaFree(device_b);
		if (cudaFreeResult != cudaSuccess) {
			std::cout << "Could not free Memory of Vector b, Error Code " << cudaFreeResult << "." << std::endl;
		}
		device_b = nullptr;
	}
	if (device_multiplyResult != nullptr) {
		cudaError_t cudaFreeResult = cudaFree(device_multiplyResult);
		if (cudaFreeResult != cudaSuccess) {
			std::cout << "Could not free Memory of Vector multiplyResult, Error Code " << cudaFreeResult << "." << std::endl;
		}
		device_multiplyResult = nullptr;
	}
	if (device_nondeterministicChoiceIndices != nullptr) {
		cudaError_t cudaFreeResult = cudaFree(device_nondeterministicChoiceIndices);
		if (cudaFreeResult != cudaSuccess) {
			std::cout << "Could not free Memory of Nondeterministic Choice Indices, Error Code " << cudaFreeResult << "." << std::endl;
		}
		device_nondeterministicChoiceIndices = nullptr;
	}
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