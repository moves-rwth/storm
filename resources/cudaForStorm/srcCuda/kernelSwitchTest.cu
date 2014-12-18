#include <iostream>
#include <chrono>

__global__ void cuda_kernel_kernelSwitchTest(int const * const A, int * const B) {
	*B = *A;
}

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
}