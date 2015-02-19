#ifndef STORM_CUDAFORSTORM_VERSION_H_
#define STORM_CUDAFORSTORM_VERSION_H_

// Library exports
#include "cudaForStorm.h"

#include <string>

size_t getStormCudaPluginVersionMajor();
size_t getStormCudaPluginVersionMinor();
size_t getStormCudaPluginVersionPatch();
size_t getStormCudaPluginVersionCommitsAhead();
const char* getStormCudaPluginVersionHash();
bool getStormCudaPluginVersionIsDirty();

#endif // STORM_CUDAFORSTORM_VERSION_H_