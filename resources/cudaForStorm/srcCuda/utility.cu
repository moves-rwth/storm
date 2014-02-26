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

void resetCudaDevice() {
	cudaDeviceReset();
}