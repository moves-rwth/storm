#ifndef STORM_UTILITY_OSDETECTION_H_
#define STORM_UTILITY_OSDETECTION_H_

#if defined __linux__ || defined __linux
#	define LINUX
#	include <sys/mman.h>
#elif defined TARGET_OS_MAC || defined __apple__ || defined __APPLE__
#	define MACOSX
#	define _DARWIN_USE_64_BIT_INODE
#	include <sys/mman.h>
#	include <unistd.h>
#elif defined _WIN32 || defined _WIN64
#	define WINDOWS
#	define NOMINMAX
#	include <Windows.h>
#	include <winnt.h>
#	define strncpy strncpy_s
#else
#	error Could not detect Operating System
#endif

#endif // STORM_UTILITY_OSDETECTION_H_
