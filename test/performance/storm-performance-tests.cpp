#include <iostream>

#include "gtest/gtest.h"
#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
#include "log4cplus/consoleappender.h"
#include "log4cplus/fileappender.h"

#include "src/utility/Settings.h"
#include "src/modelchecker/GmmxxDtmcPrctlModelChecker.h"

log4cplus::Logger logger;

/*!
 * Initializes the logging framework.
 */
void setUpLogging() {
	logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("main"));
	logger.setLogLevel(log4cplus::WARN_LOG_LEVEL);
	log4cplus::SharedAppenderPtr fileLogAppender(new log4cplus::FileAppender("storm-performance-tests.log"));
	fileLogAppender->setName("mainFileAppender");
	fileLogAppender->setThreshold(log4cplus::WARN_LOG_LEVEL);
	fileLogAppender->setLayout(std::auto_ptr<log4cplus::Layout>(new log4cplus::PatternLayout("%-5p - %D{%H:%M} (%r ms) - %F:%L : %m%n")));
	logger.addAppender(fileLogAppender);

	// Uncomment these lines to enable console logging output
	// log4cplus::SharedAppenderPtr consoleLogAppender(new log4cplus::ConsoleAppender());
	// consoleLogAppender->setName("mainConsoleAppender");
	// consoleLogAppender->setLayout(std::auto_ptr<log4cplus::Layout>(new log4cplus::PatternLayout("%-5p - %D{%H:%M:%s} (%r ms) - %F:%L : %m%n")));
	// logger.addAppender(consoleLogAppender);
}

/*!
 * Creates an empty settings object as the standard instance of the Settings class.
 */
void createEmptyOptions() {
    storm::settings::Settings::registerModule<storm::modelchecker::GmmxxDtmcPrctlModelChecker<double>>();
    const char* newArgv[] = {"storm-performance-tests"};
    storm::settings::Settings* s = storm::settings::newInstance(1, newArgv, nullptr, true);
}

int main(int argc, char* argv[]) {
	setUpLogging();
	createEmptyOptions();
	std::cout << "StoRM (Performance) Testing Suite" << std::endl;
	
	testing::InitGoogleTest(&argc, argv);

    int result = RUN_ALL_TESTS();
    
    logger.closeNestedAppenders();
    return result;
}
