#ifndef STORM_CUDAFORSTORM_UTILITY_H_
#define STORM_CUDAFORSTORM_UTILITY_H_

// Library exports
#include "cudaForStorm.h"

size_t getFreeCudaMemory();
size_t getTotalCudaMemory();
bool resetCudaDevice();
int getRuntimeCudaVersion();

#endif // STORM_CUDAFORSTORM_UTILITY_H_