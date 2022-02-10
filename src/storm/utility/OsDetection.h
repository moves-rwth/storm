#ifndef STORM_UTILITY_OSDETECTION_H_
#define STORM_UTILITY_OSDETECTION_H_

#if defined __linux__ || defined __linux
#define LINUX
#define NOEXCEPT noexcept
#include <cxxabi.h>    // Required by ErrorHandling.h
#include <execinfo.h>  // Required by ErrorHandling.h
#include <sys/mman.h>
#include <sys/resource.h>  // Required by storm.cpp, Memory Usage
#include <sys/time.h>      // Required by storm.cpp, Memory Usage
#include <unistd.h>
#define GetCurrentDir getcwd
#elif defined TARGET_OS_MAC || defined __apple__ || defined __APPLE__
#define MACOSX
#define MACOS
#define NOEXCEPT noexcept
#define _DARWIN_USE_64_BIT_INODE
#include <cxxabi.h>    // Required by ErrorHandling.h
#include <execinfo.h>  // Required by ErrorHandling.h
#include <sys/mman.h>
#include <sys/resource.h>  // Required by storm.cpp, Memory Usage
#include <sys/time.h>      // Required by storm.cpp, Memory Usage
#include <unistd.h>
#define GetCurrentDir getcwd
#elif defined _WIN32 || defined _WIN64
#define WINDOWS
#define NOEXCEPT throw()
#ifndef NOMINMAX
#define NOMINMAX
#undef min
#undef max
#endif
#include <DbgHelp.h>
#include <Psapi.h>
#include <Windows.h>
#include <direct.h>
#include <winnt.h>
#define strncpy strncpy_s
#define sscanf sscanf_s
#define GetCurrentDir _getcwd

// This disables Warning C4250 - Diamond Inheritance Dominance
#pragma warning(disable : 4250)

#else
#error Could not detect Operating System
#endif

#endif  // STORM_UTILITY_OSDETECTION_H_
