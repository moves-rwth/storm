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

/*!
 * Function that parses the command line options.
 * @param argc The argc argument of main().
 * @param argv The argv argument of main().
 * @return True iff the program should continue to run after parsing the options.
 */
bool parseOptions(int const argc, char const * const argv[]) {
    storm::settings::Settings* s = nullptr;
    try {
        storm::settings::Settings::registerModule<storm::modelchecker::GmmxxDtmcPrctlModelChecker<double>>();
        s = storm::settings::newInstance(argc, argv, nullptr, true);
    } catch (storm::exceptions::InvalidSettingsException& e) {
        std::cout << "Could not recover from settings error: " << e.what() << "." << std::endl;
        std::cout << std::endl << storm::settings::help;
        return false;
    }
    
    if (s->isSet("help")) {
        std::cout << storm::settings::help;
        return false;
    }
    
    return true; 
}

int main(int argc, char* argv[]) {
	setUpLogging();
	if (!parseOptions(argc, argv)) {
		return 0;
	}
	std::cout << "STORM Testing Suite" << std::endl;
	
	testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
