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

std::string demangle(const char* symbol) {
	int status;
	// Attention: sscanf format strings rely on size being 128
	char temp[128];
	char* demangled;
	// Check for C++ symbol
	if (sscanf(symbol, "%*[^(]%*[^_]%127[^)+]", temp) == 1) {
		if (NULL != (demangled = abi::__cxa_demangle(temp, NULL, NULL, &status))) {
			std::string result(demangled);
			free(demangled);
			return result;
		}
	}
	// Check for C symbol
	if (sscanf(symbol, "%127s", temp) == 1) {
		return temp;
	}
	// Return plain symbol
	return symbol;
}

void signalHandler(int sig) {
#define SIZE 128
	LOG4CPLUS_FATAL(logger, "We recieved a segfault. To start with, here is a backtrace.");

	void *buffer[SIZE];
 	char **strings;
	int nptrs;
	nptrs = backtrace(buffer, SIZE);

	strings = backtrace_symbols(buffer, nptrs);
	if (strings == nullptr) {
		std::cerr << "Obtaining the backtrace symbols failed. Well, shit." << std::endl;
		exit(2);
	}
	// j = 2: skip the handler itself.
	for (int j = 1; j < nptrs; j++) {
		LOG4CPLUS_FATAL(logger, nptrs-j << ": " << demangle(strings[j]));
	}
	free(strings);
	LOG4CPLUS_FATAL(logger, "That's all we can do. Bye.");
	exit(2);
}

void installSignalHandler() {
	signal(SIGSEGV, signalHandler);
}

#endif	/* ERRORHANDLING_H */

