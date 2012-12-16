#pragma once

#if defined __linux__ || defined __linux
#	define LINUX
#elif defined TARGET_OS_MAC || defined __apple__ || defined __APPLE__
#	define MACOSX
#	define _DARWIN_USE_64_BIT_INODE
#elif defined _WIN32 || defined _WIN64
#	define WINDOWS
#	define NOMINMAX
#	include <Windows.h>
#	include <winnt.h>
#else
#	error Could not detect Operating System
#endif
