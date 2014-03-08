#include "basicValueIteration.h"

#include <iostream>
#include <chrono>

#include <cuda_runtime.h>
#include "cusparse_v2.h"

#include "cuspExtension.h"

__global__ void cuda_kernel_basicValueIteration_mvReduce(int const * const A, int * const B) {
	*B = *A;
}

template <typename IndexType, typename ValueType>
void basicValueIteration_mvReduce(uint_fast64_t const maxIterationCount, std::vector<IndexType> const& matrixRowIndices, std::vector<std::pair<IndexType, ValueType>> columnIndicesAndValues, std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<IndexType> const& nondeterministicChoiceIndices) {
	IndexType* device_matrixRowIndices = nullptr;
	IndexType* device_matrixColIndicesAndValues = nullptr;
	ValueType* device_x = nullptr;
	ValueType* device_b = nullptr;
	ValueType* device_multiplyResult = nullptr;
	IndexType* device_nondeterministicChoiceIndices = nullptr;

	cudaError_t cudaMallocResult;

	cudaMallocResult = cudaMalloc<IndexType>(&device_matrixRowIndices, matrixRowIndices.size());
	if (cudaMallocResult != cudaSuccess) {
		std::cout << "Could not allocate memory for Matrix Row Indices, Error Code " << cudaMallocResult << "." << std::endl;
		goto cleanup;
	}

	cudaMallocResult = cudaMalloc<IndexType>(&device_matrixColIndicesAndValues, columnIndicesAndValues.size() * 2);
	if (cudaMallocResult != cudaSuccess) {
		std::cout << "Could not allocate memory for Matrix Column Indices and Values, Error Code " << cudaMallocResult << "." << std::endl;
		goto cleanup;
	}

	cudaMallocResult = cudaMalloc<ValueType>(&device_x, x.size());
	if (cudaMallocResult != cudaSuccess) {
		std::cout << "Could not allocate memory for Vector x, Error Code " << cudaMallocResult << "." << std::endl;
		goto cleanup;
	}

	cudaMallocResult = cudaMalloc<ValueType>(&device_b, b.size());
	if (cudaMallocResult != cudaSuccess) {
		std::cout << "Could not allocate memory for Vector b, Error Code " << cudaMallocResult << "." << std::endl;
		goto cleanup;
	}

	cudaMallocResult = cudaMalloc<ValueType>(&device_multiplyResult, b.size());
	if (cudaMallocResult != cudaSuccess) {
		std::cout << "Could not allocate memory for Vector multiplyResult, Error Code " << cudaMallocResult << "." << std::endl;
		goto cleanup;
	}

	cudaMallocResult = cudaMalloc<IndexType>(&device_nondeterministicChoiceIndices, nondeterministicChoiceIndices.size());
	if (cudaMallocResult != cudaSuccess) {
		std::cout << "Could not allocate memory for Nondeterministic Choice Indices, Error Code " << cudaMallocResult << "." << std::endl;
		goto cleanup;
	}

	// Memory allocated, copy data to device
	cudaError_t cudaCopyResult;

	cudaCopyResult = cudaMemcpy(device_matrixRowIndices, matrixRowIndices.data(), sizeof(IndexType) * matrixRowIndices.size(), cudaMemcpyHostToDevice);
	if (cudaCopyResult != cudaSuccess) {
		std::cout << "Could not copy data for Matrix Row Indices, Error Code " << cudaCopyResult << std::endl;
		goto cleanup;
	}

	cudaCopyResult = cudaMemcpy(device_matrixColIndicesAndValues, columnIndicesAndValues.data(), (sizeof(IndexType) * columnIndicesAndValues.size()) + (sizeof(ValueType) * columnIndicesAndValues.size()), cudaMemcpyHostToDevice);
	if (cudaCopyResult != cudaSuccess) {
		std::cout << "Could not copy data for Matrix Column Indices and Values, Error Code " << cudaCopyResult << std::endl;
		goto cleanup;
	}

	cudaCopyResult = cudaMemcpy(device_x, x.data(), sizeof(ValueType) * x.size(), cudaMemcpyHostToDevice);
	if (cudaCopyResult != cudaSuccess) {
		std::cout << "Could not copy data for Vector x, Error Code " << cudaCopyResult << std::endl;
		goto cleanup;
	}

	cudaCopyResult = cudaMemcpy(device_b, b.data(), sizeof(ValueType) * b.size(), cudaMemcpyHostToDevice);
	if (cudaCopyResult != cudaSuccess) {
		std::cout << "Could not copy data for Vector b, Error Code " << cudaCopyResult << std::endl;
		goto cleanup;
	}

	cudaCopyResult = cudaMemcpy(device_nondeterministicChoiceIndices, nondeterministicChoiceIndices.data(), sizeof(IndexType) * nondeterministicChoiceIndices.size(), cudaMemcpyHostToDevice);
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
 * Declare and implement all exported functions for these Kernels here
 *
 */

void cudaForStormTestFunction(int a, int b) {
	std::cout << "Cuda for Storm: a + b = " << (a+b) << std::endl;
}

void basicValueIteration_mvReduce_uint64_double(uint_fast64_t const maxIterationCount, std::vector<uint_fast64_t> const& matrixRowIndices, std::vector<std::pair<uint_fast64_t, double>> columnIndicesAndValues, std::vector<double>& x, std::vector<double> const& b, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices) {
	basicValueIteration_mvReduce<uint_fast64_t, double>(maxIterationCount, matrixRowIndices, columnIndicesAndValues, x, b, nondeterministicChoiceIndices);
}