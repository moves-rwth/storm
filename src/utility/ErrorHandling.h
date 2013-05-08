/* 
 * File:   ErrorHandling.h
 * Author: Gereon Kremer
 *
 * Created on March 15, 2013, 4:10 PM
 */

#ifndef ERRORHANDLING_H
#define	ERRORHANDLING_H

#include <signal.h>
#include <execinfo.h>
#include <cxxabi.h>

/*
 * Demangles the given string. This is needed for the correct display of backtraces.
 *
 * @param symbol The name of the symbol that is to be demangled.
 */
std::string demangle(char const* symbol) {
	int status;
    
	// Attention: sscanf format strings rely on the size being 128.
	char temp[128];
	char* demangled;
    
	// Check for C++ symbol.
	if (sscanf(symbol, "%*[^(]%*[^_]%127[^)+]", temp) == 1) {
		if (NULL != (demangled = abi::__cxa_demangle(temp, NULL, NULL, &status))) {
			std::string result(demangled);
			free(demangled);
			return result;
		}
	}
	// Check for C symbol.
	if (sscanf(symbol, "%127s", temp) == 1) {
		return temp;
	}
    
	// Return plain symbol if none of the above cases matched.
	return symbol;
}

/*
 * Handles the given signal. This will display the received signal and a backtrace.
 *
 * @param sig The code of the signal that needs to be handled.
 */
void signalHandler(int sig) {
#define SIZE 128
	LOG4CPLUS_FATAL(logger, "The program received signal " << sig << ". The following backtrace shows the status upon reception of the signal.");

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
		LOG4CPLUS_FATAL(logger, nptrs-j << ": " << demangle(strings[j]));
	}
	free(strings);
	LOG4CPLUS_FATAL(logger, "Exiting.");
	exit(2);
}

/*
 * Registers some signal handlers so that we can display a backtrace upon erroneuous termination.
 */
void installSignalHandler() {
	signal(SIGSEGV, signalHandler);
}

#endif	/* ERRORHANDLING_H */

