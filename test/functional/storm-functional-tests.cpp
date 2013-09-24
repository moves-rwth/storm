#include <iostream>

#include "gtest/gtest.h"
#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
#include "log4cplus/consoleappender.h"
#include "log4cplus/fileappender.h"

#include "src/settings/Settings.h"
#include "src/utility/StormOptions.h" // Registers all standard options

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

/*!
 * Creates an empty settings object as the standard instance of the Settings class.
 */
void createEmptyOptions() {
    const char* newArgv[] = {"storm-functional-tests"};
	storm::settings::Settings* s = storm::settings::Settings::getInstance();
	try {
		storm::settings::Settings::parse(1, newArgv);
	} catch (storm::exceptions::OptionParserException& e) {
		std::cout << "Could not recover from settings error: " << e.what() << "." << std::endl;
		std::cout << std::endl << s->getHelpText();
	}
}

int main(int argc, char* argv[]) {
	setUpLogging();
	createEmptyOptions();
	std::cout << "StoRM (Functional) Testing Suite" << std::endl;
	
	testing::InitGoogleTest(&argc, argv);
    
	// now all Google Test Options have been removed
	storm::settings::Settings* instance = storm::settings::Settings::getInstance();

	try {
		storm::settings::Settings::parse(argc, argv);
	} catch (storm::exceptions::OptionParserException& e) {
		std::cout << "Could not recover from settings error: " << e.what() << "." << std::endl;
		std::cout << std::endl << instance->getHelpText();
		return false;
	}

    int result = RUN_ALL_TESTS();
    
    logger.closeNestedAppenders();
    return result;
}
