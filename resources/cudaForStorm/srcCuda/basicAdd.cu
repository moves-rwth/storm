#include <cuda.h>
#include <stdlib.h>
#include <stdio.h>

#include <chrono>
#include <iostream>

__global__ void cuda_kernel_basicAdd(int a, int b, int *c) { 
	*c = a + b; 
}

__global__ void cuda_kernel_arrayFma(int const * const A, int const * const B, int const * const C, int * const D, int const N) {
	// Fused Multiply Add:
	// A * B + C => D

	/*
     *Die Variable i dient für den Zugriff auf das Array. Da jeder Thread die Funktion VecAdd
     *ausführt, muss i für jeden Thread unterschiedlich sein. Ansonsten würden unterschiedliche
     *Threads auf denselben Index im Array schreiben. blockDim.x ist die Anzahl der Threads der x-Komponente
     *des Blocks, blockIdx.x ist die x-Koordinate des aktuellen Blocks und threadIdx.x ist die x-Koordinate des
     *Threads, der die Funktion gerade ausführt.
    */
    int i = blockDim.x * blockIdx.x + threadIdx.x;

	if (i < N) {
		D[i] = A[i] * B[i] + C[i];
	}
}

__global__ void cuda_kernel_arrayFmaOptimized(int * const A, int const N, int const M) {
	// Fused Multiply Add:
	// A * B + C => D

	// Layout:
	// A B C D A B C D A B C D

    int i = blockDim.x * blockIdx.x + threadIdx.x;

	if ((i*M) < N) {
		for (int j = i*M; j < i*M + M; ++j) {
			A[j*4 + 3] = A[j*4] * A[j*4 + 1] + A[j*4 + 2];
		}
	}
}

extern "C" int cuda_basicAdd(int a, int b) {
	int c = 0;
	int *dev_c;
	cudaMalloc((void**)&dev_c, sizeof(int));
	cuda_kernel_basicAdd<<<1, 1>>>(a, b, dev_c);
	cudaMemcpy(&c, dev_c, sizeof(int), cudaMemcpyDeviceToHost);
	//printf("%d + %d + 42 is %d\n", a, b, c);
	cudaFree(dev_c);
	return c;
}

void cpp_cuda_bandwidthTest(int entryCount, int N) {
	// Size of the Arrays
	size_t arraySize = entryCount * sizeof(int);
	
	int* deviceIntArray;
	int* hostIntArray = new int[arraySize];

	// Allocate space on the device
	auto start_time = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < N; ++i) {
		if (cudaMalloc((void**)&deviceIntArray, arraySize) != cudaSuccess) {
			std::cout << "Error in cudaMalloc while allocating " << arraySize << " Bytes!" << std::endl;
			delete[] hostIntArray;
			return;
		}
		// Free memory on device
		if (cudaFree(deviceIntArray) != cudaSuccess) {
			std::cout << "Error in cudaFree!" << std::endl;
			delete[] hostIntArray;
			return;
		}
	}
	auto end_time = std::chrono::high_resolution_clock::now();
	auto copyTime = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
	double mBytesPerSecond = (((double)(N * arraySize)) / copyTime) * 0.95367431640625;
	std::cout << "Allocating the Array " << N << " times took " << copyTime << " Microseconds." << std::endl;
	std::cout << "Resulting in " << mBytesPerSecond << " MBytes per Second Allocationspeed." << std::endl;

	if (cudaMalloc((void**)&deviceIntArray, arraySize) != cudaSuccess) {
		std::cout << "Error in cudaMalloc while allocating " << arraySize << " Bytes for copyTest!" << std::endl;
		delete[] hostIntArray;
		return;
	}
	
	// Prepare data
	for (int i = 0; i < N; ++i) {
		hostIntArray[i] = i * 333 + 123;
	}

	// Copy data TO device
	start_time = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < N; ++i) {
		if (cudaMemcpy(deviceIntArray, hostIntArray, arraySize, cudaMemcpyHostToDevice) != cudaSuccess) {
			std::cout << "Error in cudaMemcpy while copying " << arraySize << " Bytes to device!" << std::endl;
			// Free memory on device
			if (cudaFree(deviceIntArray) != cudaSuccess) {
				std::cout << "Error in cudaFree!" << std::endl;
			}
			delete[] hostIntArray;
			return;
		}
	}
	end_time = std::chrono::high_resolution_clock::now();
	copyTime = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
	mBytesPerSecond = (((double)(N * arraySize)) / copyTime) * 0.95367431640625;
	std::cout << "Copying the Array " << N << " times took " << copyTime << " Microseconds." << std::endl;
	std::cout << "Resulting in " << mBytesPerSecond << " MBytes per Second TO device." << std::endl;

	// Copy data FROM device
	start_time = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < N; ++i) {
		if (cudaMemcpy(hostIntArray, deviceIntArray, arraySize, cudaMemcpyDeviceToHost) != cudaSuccess) {
			std::cout << "Error in cudaMemcpy while copying " << arraySize << " Bytes to host!" << std::endl;
			// Free memory on device
			if (cudaFree(deviceIntArray) != cudaSuccess) {
				std::cout << "Error in cudaFree!" << std::endl;
			}
			delete[] hostIntArray;
			return;
		}
	}
	end_time = std::chrono::high_resolution_clock::now();
	copyTime = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
	mBytesPerSecond = (((double)(N * arraySize)) / copyTime) * 0.95367431640625;
	std::cout << "Copying the Array " << N << " times took " << copyTime << " Microseconds." << std::endl;
	std::cout << "Resulting in " << mBytesPerSecond << " MBytes per Second FROM device." << std::endl;

	// Free memory on device
	if (cudaFree(deviceIntArray) != cudaSuccess) {
		std::cout << "Error in cudaFree!" << std::endl;
	}
	delete[] hostIntArray;
}

extern "C" void cuda_arrayFma(int const * const A, int const * const B, int const * const C, int * const D, int const N) {
	// Size of the Arrays
	size_t arraySize = N * sizeof(int);
	
	int* deviceIntArrayA;
	int* deviceIntArrayB;
	int* deviceIntArrayC;
	int* deviceIntArrayD;

	// Allocate space on the device
	if (cudaMalloc((void**)&deviceIntArrayA, arraySize) != cudaSuccess) {
		printf("Error in cudaMalloc1!\n");
		return;
	}
	if (cudaMalloc((void**)&deviceIntArrayB, arraySize) != cudaSuccess) {
		printf("Error in cudaMalloc2!\n");
		cudaFree(deviceIntArrayA);
		return;
	}
	if (cudaMalloc((void**)&deviceIntArrayC, arraySize) != cudaSuccess) {
		printf("Error in cudaMalloc3!\n");
		cudaFree(deviceIntArrayA);
		cudaFree(deviceIntArrayB);
		return;
	}
	if (cudaMalloc((void**)&deviceIntArrayD, arraySize) != cudaSuccess) {
		printf("Error in cudaMalloc4!\n");
		cudaFree(deviceIntArrayA);
		cudaFree(deviceIntArrayB);
		cudaFree(deviceIntArrayC);
		return;
	}
	
	// Copy data TO device
	if (cudaMemcpy(deviceIntArrayA, A, arraySize, cudaMemcpyHostToDevice) != cudaSuccess) {
		printf("Error in cudaMemcpy!\n");
		cudaFree(deviceIntArrayA);
		cudaFree(deviceIntArrayB);
		cudaFree(deviceIntArrayC);
		cudaFree(deviceIntArrayD);
		return;
	}
	if (cudaMemcpy(deviceIntArrayB, B, arraySize, cudaMemcpyHostToDevice) != cudaSuccess) {
		printf("Error in cudaMemcpy!\n");
		cudaFree(deviceIntArrayA);
		cudaFree(deviceIntArrayB);
		cudaFree(deviceIntArrayC);
		cudaFree(deviceIntArrayD);
		return;
	}
	if (cudaMemcpy(deviceIntArrayC, C, arraySize, cudaMemcpyHostToDevice) != cudaSuccess) {
		printf("Error in cudaMemcpy!\n");
		cudaFree(deviceIntArrayA);
		cudaFree(deviceIntArrayB);
		cudaFree(deviceIntArrayC);
		cudaFree(deviceIntArrayD);
		return;
	}
	
    // Festlegung der Threads pro Block
    int threadsPerBlock = 512;
    // Es werden soviele Blöcke benötigt, dass alle Elemente der Vektoren abgearbeitet werden können
    int blocksPerGrid = (N + threadsPerBlock - 1) / threadsPerBlock;

	// Run kernel
	cuda_kernel_arrayFma<<<blocksPerGrid, threadsPerBlock>>>(deviceIntArrayA, deviceIntArrayB, deviceIntArrayC, deviceIntArrayD, N);

	// Copy data FROM device
	if (cudaMemcpy(D, deviceIntArrayD, arraySize, cudaMemcpyDeviceToHost) != cudaSuccess) {
		printf("Error in cudaMemcpy!\n");
		cudaFree(deviceIntArrayA);
		cudaFree(deviceIntArrayB);
		cudaFree(deviceIntArrayC);
		cudaFree(deviceIntArrayD);
		return;
	}

	// Free memory on device
	cudaFree(deviceIntArrayA);
	cudaFree(deviceIntArrayB);
	cudaFree(deviceIntArrayC);
	cudaFree(deviceIntArrayD);
}

extern "C" void cuda_arrayFmaOptimized(int * const A, int const N, int const M) {
	// Size of the Arrays
	size_t arraySize = N * sizeof(int) * 4;
	
	int* deviceIntArrayA;

	// Allocate space on the device
	if (cudaMalloc((void**)&deviceIntArrayA, arraySize) != cudaSuccess) {
		printf("Error in cudaMalloc1!\n");
		return;
	}

#define ONFAILFREE0() do { } while(0)
#define ONFAILFREE1(a) do { cudaFree(a); } while(0)
#define ONFAILFREE2(a, b) do { cudaFree(a); cudaFree(b); } while(0)
#define ONFAILFREE3(a, b, c) do { cudaFree(a); cudaFree(b); cudaFree(c); } while(0)
#define ONFAILFREE4(a, b, c, d) do { cudaFree(a); cudaFree(b); cudaFree(c); cudaFree(d); } while(0)
#define CHECKED_CUDA_CALL(func__, freeArgs, ...) do { int retCode = cuda##func__ (__VA_ARGS__); if (retCode != cudaSuccess) { freeArgs; printf("Error in func__!\n"); return; } } while(0)

	// Copy data TO device

	CHECKED_CUDA_CALL(Memcpy, ONFAILFREE1(deviceIntArrayA), deviceIntArrayA, A, arraySize, cudaMemcpyHostToDevice);

	/*if (cudaMemcpy(deviceIntArrayA, A, arraySize, cudaMemcpyHostToDevice) != cudaSuccess) {
		printf("Error in cudaMemcpy!\n");
		cudaFree(deviceIntArrayA);
		return;
	}*/
	
    // Festlegung der Threads pro Block
    int threadsPerBlock = 512;
    // Es werden soviele Blöcke benötigt, dass alle Elemente der Vektoren abgearbeitet werden können
    int blocksPerGrid = (N + threadsPerBlock - 1) / threadsPerBlock;

	// Run kernel
	cuda_kernel_arrayFmaOptimized<<<blocksPerGrid, threadsPerBlock>>>(deviceIntArrayA, N, M);

	// Copy data FROM device
	if (cudaMemcpy(A, deviceIntArrayA, arraySize, cudaMemcpyDeviceToHost) != cudaSuccess) {
		printf("Error in cudaMemcpy!\n");
		cudaFree(deviceIntArrayA);
		return;
	}

	// Free memory on device
	if (cudaFree(deviceIntArrayA) != cudaSuccess) {
		printf("Error in cudaFree!\n");
		return;
	}
}

extern "C" void cuda_arrayFmaHelper(int const * const A, int const * const B, int const * const C, int * const D, int const N) {
	for (int i = 0; i < N; ++i) {
		D[i] = A[i] * B[i] + C[i];
	}
}

extern "C" void cuda_arrayFmaOptimizedHelper(int * const A, int const N) {
	for (int i = 0; i < N; i += 4) {
		A[i+3] = A[i] * A[i+1] + A[i+2];
	}
}