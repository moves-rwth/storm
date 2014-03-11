#ifndef STORM_CUDAFORSTORM_UTILITY_H_
#define STORM_CUDAFORSTORM_UTILITY_H_

// Library exports
#include "cudaForStorm_Export.h"

cudaForStorm_EXPORT size_t getFreeCudaMemory();
cudaForStorm_EXPORT size_t getTotalCudaMemory();
cudaForStorm_EXPORT bool resetCudaDevice();
cudaForStorm_EXPORT int getRuntimeCudaVersion();

#endif // STORM_CUDAFORSTORM_UTILITY_H_