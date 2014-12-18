#include "utility.h"

#include <cuda_runtime.h>

size_t getFreeCudaMemory() {
	size_t freeMemory;
	size_t totalMemory;
	cudaMemGetInfo(&freeMemory, &totalMemory);

	return freeMemory;
}

size_t getTotalCudaMemory() {
	size_t freeMemory;
	size_t totalMemory;
	cudaMemGetInfo(&freeMemory, &totalMemory);

	return totalMemory;
}

bool resetCudaDevice() {
	cudaError_t result = cudaDeviceReset();
	return (result == cudaSuccess);
}

int getRuntimeCudaVersion() {
	int result = -1;
	cudaError_t errorResult = cudaRuntimeGetVersion(&result);
	if (errorResult != cudaSuccess) {
		return -1;
	}
	return result;
}