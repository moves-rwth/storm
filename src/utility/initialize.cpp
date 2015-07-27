#include "initialize.h"

log4cplus::Logger logger;
log4cplus::Logger printer;

namespace storm {
    namespace utility {
       void initializeLogger() {
            logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("main"));
            log4cplus::SharedAppenderPtr consoleLogAppender(new log4cplus::ConsoleAppender());
            consoleLogAppender->setName("mainConsoleAppender");
            consoleLogAppender->setLayout(std::auto_ptr<log4cplus::Layout>(new log4cplus::PatternLayout("%-5p - %D{%H:%M:%S} (%r ms) - %b:%L: %m%n")));
            logger.addAppender(consoleLogAppender);
            auto loglevel = storm::settings::debugSettings().isTraceSet() ? log4cplus::TRACE_LOG_LEVEL : storm::settings::debugSettings().isDebugSet() ? log4cplus::DEBUG_LOG_LEVEL : log4cplus::WARN_LOG_LEVEL;
            logger.setLogLevel(loglevel);
            consoleLogAppender->setThreshold(loglevel);
        }

        void setUp() {
            initializeLogger();
            std::cout.precision(10);
        }

        void cleanUp() {
            // Intentionally left empty.
        }

        void initializeFileLogging() {
            log4cplus::SharedAppenderPtr fileLogAppender(new log4cplus::FileAppender(storm::settings::debugSettings().getLogfilename()));
            fileLogAppender->setName("mainFileAppender");
            fileLogAppender->setLayout(std::auto_ptr<log4cplus::Layout>(new log4cplus::PatternLayout("%-5p - %D{%H:%M:%S} (%r ms) - %F:%L: %m%n")));
            logger.addAppender(fileLogAppender);
        }

    }
}
