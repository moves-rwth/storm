#include <cuda.h>
#include "srcCuda/allCudaKernels.h"

#include <iostream>
#include <chrono>
#include <random>

void cudaShowDevices() {
	// Todo
}

void cudaSimpleAddTest(int a, int b) {
	std::cout << "Running cudaSimpleAddTest:" << std::endl;
	std::cout << "a = " << a << ", b = " << b << "" << std::endl;

	int c = cuda_basicAdd(a, b);
	
	std::cout << "Result: " << c << "" << std::endl;
}

void cudaArrayFmaTest(int N) {
	std::cout << "Running cudaArrayFmaTest:" << std::endl;
	std::cout << "N is " << N << ", resulting in " << (5 * sizeof(int) * N) << " Bytes of Data." << std::endl;

	std::cout << "Generating random input arrays." << std::endl;

	std::default_random_engine generator;
	std::uniform_int_distribution<int> distribution(0, INT32_MAX);
	int dice_roll = distribution(generator);

	auto start_time = std::chrono::high_resolution_clock::now();

	int* arrayA = new int[N];
	int* arrayB = new int[N];
	int* arrayC = new int[N];
	int* arrayD = new int[N];
	int* arrayD_CPU = new int[N];

	for (int i = 0; i < N; ++i) {
		//arrayA[i] = distribution(generator);
		//arrayB[i] = distribution(generator);
		//arrayC[i] = distribution(generator);
		arrayA[i] = i * 1000 + 137;
		arrayB[i] = i * 7000 + 1537;
		arrayC[i] = i * 15000 + 97;
		arrayD[i] = 0;
		arrayD_CPU[i] = 0;
	}

	auto end_time = std::chrono::high_resolution_clock::now();
	std::cout << "Array generation took " << std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() << "micros" << std::endl;

	std::cout << "Running FMA test on CPU." << std::endl;

	start_time = std::chrono::high_resolution_clock::now();	
	cuda_arrayFmaHelper(arrayA, arrayB, arrayC, arrayD_CPU, N);
	end_time = std::chrono::high_resolution_clock::now();
	std::cout << "FMA on CPU took " << std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() << "micros" << std::endl;

	start_time = std::chrono::high_resolution_clock::now();
	cuda_arrayFma(arrayA, arrayB, arrayC, arrayD, N);
	end_time = std::chrono::high_resolution_clock::now();
	std::cout << "FMA on GPU took " << std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() << "micros" << std::endl;

	int errors = 0;
	for (int i = 0; i < N; ++i) {
		if (arrayD[i] != arrayD_CPU[i]) {
			std::cout << "Error in Entry " << i << ": GPU has " << arrayD[i] << " but CPU has " << arrayD_CPU[i] << "!" << std::endl;
			++errors;
		}
	}
	std::cout << "Checked Arrays for Errors: " << errors << " Errors occured." << std::endl;
}

void cudaArrayFmaOptimizedTest(int N, int M) {
	std::cout << "Running cudaArrayFmaTest:" << std::endl;
	std::cout << "N is " << N << ", resulting in " << (4 * sizeof(int) * N) << " Bytes of Data." << std::endl;

	size_t freeCudaMemory = getFreeCudaMemory();
	size_t totalCudaMemory = getTotalCudaMemory();
	int freeProzent = static_cast<int>(((double)freeCudaMemory)/((double)totalCudaMemory) * 100);

	std::cout << "CUDA Device has " << freeCudaMemory << " Bytes of " << totalCudaMemory << " Bytes free (" << (freeProzent) << "%)." << std::endl;

	std::cout << "Generating random input arrays." << std::endl;

	std::default_random_engine generator;
	std::uniform_int_distribution<int> distribution(0, INT32_MAX);

	auto start_time = std::chrono::high_resolution_clock::now();

	int* arrayA = new int[4 * N];
	int* arrayA_CPU = new int[4 * N];

	for (int i = 0; i < 4*N; ++i) {
		arrayA[i] = i * 1000 + i + (357854878 % (i+1));
		arrayA_CPU[i] = arrayA[i];
	}

	auto end_time = std::chrono::high_resolution_clock::now();
	std::cout << "Array generation took " << std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() << "micros" << std::endl;

	start_time = std::chrono::high_resolution_clock::now();	
	cuda_arrayFmaOptimizedHelper(arrayA_CPU, N);
	end_time = std::chrono::high_resolution_clock::now();
	std::cout << "FMA on CPU took " << std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() << "micros" << std::endl;

	start_time = std::chrono::high_resolution_clock::now();
	cuda_arrayFmaOptimized(arrayA, N, M);
	end_time = std::chrono::high_resolution_clock::now();
	std::cout << "FMA on GPU took " << std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() << "micros" << std::endl;

	int errors = 0;
	for (int i = 0; i < N; i+=4) {
		if (arrayA[i+3] != arrayA_CPU[i+3]) {
			//std::cout << "Error in Entry " << i << ": GPU has " << arrayA[i+3] << " but CPU has " << arrayA_CPU[i+3] << "!" << std::endl;
			++errors;
		}
	}
	std::cout << "Checked Arrays for Errors: " << errors << " Errors occured." << std::endl;

	delete[] arrayA;
	delete[] arrayA_CPU;
}