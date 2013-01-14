#include <iostream>

#include "gtest/gtest.h"
#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
#include "log4cplus/consoleappender.h"
#include "log4cplus/fileappender.h"

log4cplus::Logger logger;

/*!
 * Initializes the logging framework.
 */
void setUpLogging() {
	log4cplus::SharedAppenderPtr fileLogAppender(new log4cplus::FileAppender("storm-tests.log"));
	fileLogAppender->setName("mainFileAppender");
	fileLogAppender->setLayout(std::auto_ptr<log4cplus::Layout>(new log4cplus::PatternLayout("%-5p - %D{%H:%M} (%r ms) - %F:%L : %m%n")));
	logger = log4cplus::Logger::getInstance("mainLogger");
	logger.addAppender(fileLogAppender);

	// Uncomment these lines to enable console logging output
	// log4cplus::SharedAppenderPtr consoleLogAppender(new log4cplus::ConsoleAppender());
	// consoleLogAppender->setName("mainConsoleAppender");
	// consoleLogAppender->setLayout(std::auto_ptr<log4cplus::Layout>(new log4cplus::PatternLayout("%-5p - %D{%H:%M:%s} (%r ms) - %F:%L : %m%n")));
	// logger.addAppender(consoleLogAppender);
}

int main(int argc, char** argv) {
	setUpLogging();
	std::cout << "STORM Testing Suite" << std::endl;
	
	testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
