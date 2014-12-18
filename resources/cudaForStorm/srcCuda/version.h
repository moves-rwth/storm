#ifndef STORM_CUDAFORSTORM_VERSION_H_
#define STORM_CUDAFORSTORM_VERSION_H_

// Library exports
#include "cudaForStorm_Export.h"

#include <string>

cudaForStorm_EXPORT size_t getStormCudaPluginVersionMajor();
cudaForStorm_EXPORT size_t getStormCudaPluginVersionMinor();
cudaForStorm_EXPORT size_t getStormCudaPluginVersionPatch();
cudaForStorm_EXPORT size_t getStormCudaPluginVersionCommitsAhead();
cudaForStorm_EXPORT const char* getStormCudaPluginVersionHash();
cudaForStorm_EXPORT bool getStormCudaPluginVersionIsDirty();

#endif // STORM_CUDAFORSTORM_VERSION_H_