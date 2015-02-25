#include <iostream>
#include <list>
#include <string>

#include "gtest/gtest.h"
#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
#include "log4cplus/consoleappender.h"
#include "log4cplus/fileappender.h"

#include "src/settings/SettingsManager.h"

log4cplus::Logger logger;

/*!
 * Initializes the logging framework.
 */
void setUpLogging() {
	logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("main"));
	logger.setLogLevel(log4cplus::ERROR_LOG_LEVEL);
	log4cplus::SharedAppenderPtr fileLogAppender(new log4cplus::FileAppender("storm-functional-tests.log"));
	fileLogAppender->setName("mainFileAppender");
	fileLogAppender->setThreshold(log4cplus::FATAL_LOG_LEVEL);
	fileLogAppender->setLayout(std::auto_ptr<log4cplus::Layout>(new log4cplus::PatternLayout("%-5p - %D{%H:%M} (%r ms) - %F:%L : %m%n")));
	logger.addAppender(fileLogAppender);

	// Uncomment these lines to enable console logging output
	// log4cplus::SharedAppenderPtr consoleLogAppender(new log4cplus::ConsoleAppender());
	// consoleLogAppender->setName("mainConsoleAppender");
	// consoleLogAppender->setLayout(std::auto_ptr<log4cplus::Layout>(new log4cplus::PatternLayout("%-5p - %D{%H:%M:%s} (%r ms) - %F:%L : %m%n")));
	// logger.addAppender(consoleLogAppender);
}

int main(int argc, char* argv[]) {
	setUpLogging();
	std::cout << "StoRM (Functional) Testing Suite" << std::endl;
	
	testing::InitGoogleTest(&argc, argv);

    int result = RUN_ALL_TESTS();
    
    logger.closeNestedAppenders();

	std::list<std::string> untestedModules;
#ifndef STORM_HAVE_GUROBI
	untestedModules.push_back("Gurobi");
#endif
#ifndef STORM_HAVE_CUDA
	untestedModules.push_back("CUDA");
#endif
#ifndef STORM_HAVE_GLPK
	untestedModules.push_back("GLPK");
#endif
#ifndef STORM_HAVE_Z3
	untestedModules.push_back("Z3");
#endif
#ifndef STORM_HAVE_MSAT
	untestedModules.push_back("MathSAT");
#endif
#ifndef STORM_HAVE_INTELTBB
	untestedModules.push_back("Intel TBB");
#endif
	
	if (result == 0) {
		if (untestedModules.empty()) {
			std::cout << std::endl << "ALL TESTS PASSED!" << std::endl;
		} else{
			std::cout << std::endl << "StoRM was built without the following optional dependencies: ";
			auto iter = untestedModules.begin();
			while (iter != untestedModules.end()) {
				std::cout << *iter;
				++iter;
				if (iter != untestedModules.end()) {
					std::cout << ", ";
				}
			}
			std::cout << std::endl << "Functionality using that modules could not be tested." << std::endl << std::endl << "TESTS PASSED!" << std::endl;
		}
	} else{
		std::cout << std::endl << "TESTS FAILED!" << std::endl;
	}

    return result;
}
