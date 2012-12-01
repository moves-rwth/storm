#pragma once

#if defined __linux__ || defined __linux
	#define LINUX
#elif defined TARGET_OS_MAC || defined __apple__
	#define MACOSX
#elif defined _WIN32 || defined _WIN64
	#define WINDOWS
#else
	#error Could not detect Operating System
#endif