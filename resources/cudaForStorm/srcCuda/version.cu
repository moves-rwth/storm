#include "version.h"

#include "storm-cudaplugin-config.h"

size_t getStormCudaPluginVersionMajor() {
	return STORM_CUDAPLUGIN_VERSION_MAJOR;
}

size_t getStormCudaPluginVersionMinor() {
	return STORM_CUDAPLUGIN_VERSION_MINOR;
}

size_t getStormCudaPluginVersionPatch() {
	return STORM_CUDAPLUGIN_VERSION_PATCH;
}

size_t getStormCudaPluginVersionCommitsAhead() {
	return STORM_CUDAPLUGIN_VERSION_COMMITS_AHEAD;
}

const char* getStormCudaPluginVersionHash() {
	static const std::string versionHash = STORM_CUDAPLUGIN_VERSION_HASH;
	return versionHash.c_str();
}

bool getStormCudaPluginVersionIsDirty() {
	return ((STORM_CUDAPLUGIN_VERSION_DIRTY) != 0);
}