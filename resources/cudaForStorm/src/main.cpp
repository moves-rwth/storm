#include "cudaForStorm.h"

#include <stdio.h>  
#include <stdlib.h>

#include <iostream>
#include <chrono>
#include <random>

#include "cudaTests.h"

int cudaForStormTest(int value) {
	return value + 42;
}


int main_Test12345(int argc, char **argv){
	resetCudaDevice();

	int testNumber = 0;
	int N = 10000;
	int M = 402653184;
	if (argc > 1) {
		testNumber = atoi(argv[1]);
		if (argc > 2) {
			N = atoi(argv[2]);
			if (argc > 3) {
				M = atoi(argv[3]);
			}
		}
	}

	switch (testNumber) {
		case 1:
			cudaSimpleAddTest(N, M);
			break;
		case 2:
			cudaArrayFmaTest(N);
			break;
		case 3:
			cudaArrayFmaOptimizedTest(N, M);
			break;
		case 4:
			cpp_cuda_bandwidthTest(M, N);
			break;
		case 5:
			kernelSwitchTest(N);
			break;
			break;
		// DEFAULT AND 0
		case 0:
		default:
			std::cout << "Available functions are:" << std::endl;
			std::cout << "0 - Show this  overview" << std::endl;
			std::cout << "1 - cuda   simpleAddTest(N, M)" << std::endl;
			std::cout << "2 - cuda   arrayFmaTest(N)" << std::endl;
			std::cout << "3 - cuda   arrayFmaOptimizedTest(N, M)" << std::endl;
			std::cout << "4 - cuda   bandwidthTest(M, N)" << std::endl;
			std::cout << "5 - cuda   kernelSwitchTest(N)" << std::endl;
			std::cout << std::endl;
			std::cout << "Call: " << argv[0] << " Selection [N [M]]" << std::endl;
			std::cout << "Defaults:" <<std::endl;
			std::cout << "N: 10000" << std::endl;
			std::cout << "M: 402653184" << std::endl;
			break;
	}

    return 0;
}