/* 
 * File:   ErrorHandling.h
 * Author: Gereon Kremer
 *
 * Created on March 15, 2013, 4:10 PM
 */

#ifndef ERRORHANDLING_H
#define	ERRORHANDLING_H

#include "src/storm/utility/OsDetection.h"
#include <signal.h>


/*
 * Demangles the given string. This is needed for the correct display of backtraces.
 *
 * @param symbol The name of the symbol that is to be demangled.
 */
std::string demangle(char const* symbol) {    
	// Attention: sscanf format strings rely on the size being 128.
	char temp[128];
    
	// Check for C++ symbol, on Non-MSVC Only
	int scanResult = 0;
#ifdef WINDOWS
	scanResult = sscanf_s(symbol, "%*[^(]%*[^_]%127[^)+]", temp, sizeof(temp));
#else
	int status;
	scanResult = sscanf(symbol, "%*[^(]%*[^_]%127[^)+]", temp);
#endif
	
	if (scanResult == 1) {
#ifndef WINDOWS
		char* demangled;
		if (NULL != (demangled = abi::__cxa_demangle(temp, NULL, NULL, &status))) {
			std::string result(demangled);
			free(demangled);
			return result;
		}
#else
	DWORD  error;
	HANDLE hProcess;

	SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);

	hProcess = GetCurrentProcess();

	if (!SymInitialize(hProcess, NULL, TRUE)) {
		// SymInitialize failed
		error = GetLastError();
		STORM_LOG_ERROR("SymInitialize returned error : " << error);
		return FALSE;
	} else {
		char demangled[1024];
		if (UnDecorateSymbolName(temp, demangled, sizeof(demangled), UNDNAME_COMPLETE)) {
			return std::string(demangled);
		} else {
			// UnDecorateSymbolName failed
			DWORD error = GetLastError();
			STORM_LOG_ERROR("UnDecorateSymbolName returned error: " << error);
		}
	}
#endif
	}

	// Check for C symbol.
	scanResult = 0;
#ifdef WINDOWS
	scanResult = sscanf_s(symbol, "%127s", temp, sizeof(temp));
#else
	scanResult = sscanf(symbol, "%127s", temp);
#endif
	if (scanResult == 1) {
		return temp;
	}
    
	// Return plain symbol if none of the above cases matched.
	return symbol;
}

void showPerformanceStatistics(uint64_t wallclockMilliseconds);

/*
 * Handles the given signal. This will display the received signal and a backtrace.
 *
 * @param sig The code of the signal that needs to be handled.
 */
void signalHandler(int sig) {
	STORM_LOG_ERROR("The program received signal " << sig << ". The following backtrace shows the status upon reception of the signal.");
    showPerformanceStatistics(0);
#ifndef WINDOWS
#	define SIZE 128
	void *buffer[SIZE];
 	char **strings;
	int nptrs;
	nptrs = backtrace(buffer, SIZE);

    // Try to retrieve the backtrace symbols.
	strings = backtrace_symbols(buffer, nptrs);
	if (strings == nullptr) {
		std::cerr << "Obtaining the backtrace symbols failed." << std::endl;
		exit(2);
	}
    
    // Starting this for-loop at j=2 means that we skip the handler itself. Currently this is not
    // done.
	for (int j = 1; j < nptrs; j++) {
		STORM_LOG_ERROR(nptrs-j << ": " << demangle(strings[j]));
	}
	free(strings);
#else
	STORM_LOG_WARN("No Backtrace Support available on Platform Windows!");
#endif
	STORM_LOG_ERROR("Exiting.");
	exit(2);
}

/*
 * Registers some signal handlers so that we can display a backtrace upon erroneuous termination.
 */
void installSignalHandler() {
#ifndef WINDOWS
    struct sigaction sa;
    sa.sa_handler = signalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    
    if (sigaction(SIGSEGV, &sa, nullptr) == -1) {
        std::cerr << "FATAL: Installing a signal handler failed." << std::endl;
    }
    if (sigaction(SIGABRT, &sa, nullptr) == -1) {
        std::cerr << "FATAL: Installing a signal handler failed." << std::endl;
    }
    if (sigaction(SIGINT, &sa, nullptr) == -1) {
        std::cerr << "FATAL: Installing a signal handler failed." << std::endl;
    }
    if (sigaction(SIGTERM, &sa, nullptr) == -1) {
        std::cerr << "FATAL: Installing a signal handler failed." << std::endl;
    }
    if (sigaction(SIGALRM, &sa, nullptr) == -1) {
        std::cerr << "FATAL: Installing a signal handler failed." << std::endl;
    }
#else
	signal(SIGSEGV, signalHandler);
    signal(SIGABRT, signalHandler);
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
#endif
}

#ifdef WINDOWS
// This defines a placeholder-function to be called from SetTimer() which in turn calls the Signal Handler
VOID CALLBACK stormWindowsSetTimerCallBack(
	HWND hwnd,        // handle to window for timer messages 
	UINT message,     // WM_TIMER message 
	UINT_PTR idEvent,
	DWORD dwTime)     // current system time 
{
	// I believe that SIGALRM translates to 14, but it could be wrong!
	signalHandler(14);
}
#endif

void stormSetAlarm(uint_fast64_t timeoutSeconds) {
#ifndef WINDOWS
	alarm(timeoutSeconds);
#else
	// This needs more research (http://msdn.microsoft.com/en-us/library/windows/desktop/ms644906(v=vs.85).aspx)
	UINT_PTR retVal = SetTimer(NULL, 0, static_cast<UINT>(timeoutSeconds * 1000), static_cast<TIMERPROC>(&stormWindowsSetTimerCallBack));
#endif
}

#endif	/* ERRORHANDLING_H */

