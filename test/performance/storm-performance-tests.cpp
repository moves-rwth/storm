#include <iostream>

#include "gtest/gtest.h"
#include "storm-config.h"


#include "src/settings/SettingsManager.h"
#ifdef STORM_LOGGING_FRAMEWORK
#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
#include "log4cplus/consoleappender.h"
#include "log4cplus/fileappender.h"

log4cplus::Logger logger;
#endif

/*!
 * Initializes the logging framework.
 */
void setUpLogging() {
#ifdef STORM_LOGGING_FRAMEWORK
	logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("main"));
	logger.setLogLevel(log4cplus::WARN_LOG_LEVEL);
	log4cplus::SharedAppenderPtr fileLogAppender(new log4cplus::FileAppender("storm-performance-tests.log"));
	fileLogAppender->setName("mainFileAppender");
	fileLogAppender->setThreshold(log4cplus::WARN_LOG_LEVEL);
	fileLogAppender->setLayout(std::auto_ptr<log4cplus::Layout>(new log4cplus::PatternLayout("%-5p - %D{%H:%M} (%r ms) - %F:%L : %m%n")));
	logger.addAppender(fileLogAppender);
#endif 
	// Uncomment these lines to enable console logging output
	// log4cplus::SharedAppenderPtr consoleLogAppender(new log4cplus::ConsoleAppender());
	// consoleLogAppender->setName("mainConsoleAppender");
	// consoleLogAppender->setLayout(std::auto_ptr<log4cplus::Layout>(new log4cplus::PatternLayout("%-5p - %D{%H:%M:%s} (%r ms) - %F:%L : %m%n")));
	// logger.addAppender(consoleLogAppender);
}

int main(int argc, char* argv[]) {
	setUpLogging();
	storm::settings::initializeAll("StoRM (Performance) Testing Suite", "storm-performance-tests");
	std::cout << "StoRM (Performance) Testing Suite" << std::endl;
	
	testing::InitGoogleTest(&argc, argv);

    int result = RUN_ALL_TESTS();
#ifdef STORM_LOGGING_FRAMEWORK
    logger.closeNestedAppenders();
#endif
    return result;
}
