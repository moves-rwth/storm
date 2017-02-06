#ifndef STORM_UTILITY_OSDETECTION_H_
#define STORM_UTILITY_OSDETECTION_H_

#if defined __linux__ || defined __linux
#	define LINUX
#   define NOEXCEPT noexcept
#	include <sys/mman.h>
#	include <unistd.h>
#include <execinfo.h> // Required by ErrorHandling.h
#include <cxxabi.h> // Required by ErrorHandling.h
#include <sys/time.h> // Required by storm.cpp, Memory Usage
#include <sys/resource.h> // Required by storm.cpp, Memory Usage
#	define GetCurrentDir getcwd
#elif defined TARGET_OS_MAC || defined __apple__ || defined __APPLE__
#	define MACOSX
#   define MACOS
#   define NOEXCEPT noexcept
#	define _DARWIN_USE_64_BIT_INODE
#	include <sys/mman.h>
#	include <unistd.h>
#	include <execinfo.h> // Required by ErrorHandling.h
#	include <cxxabi.h> // Required by ErrorHandling.h
#	include <sys/time.h> // Required by storm.cpp, Memory Usage
#	include <sys/resource.h> // Required by storm.cpp, Memory Usage
#	define GetCurrentDir getcwd
#elif defined _WIN32 || defined _WIN64
#	define WINDOWS
#   define NOEXCEPT throw()
#	ifndef NOMINMAX
#		define NOMINMAX
#		undef min
#		undef max
#	endif
#	include <Windows.h>
#	include <winnt.h>
#	include <DbgHelp.h>
#	include <Psapi.h>
#	include <direct.h>
#	define strncpy strncpy_s
#	define sscanf sscanf_s
#	define GetCurrentDir _getcwd

// This disables Warning C4250 - Diamond Inheritance Dominance
#pragma warning(disable:4250)

#else
#	error Could not detect Operating System
#endif

#endif // STORM_UTILITY_OSDETECTION_H_
