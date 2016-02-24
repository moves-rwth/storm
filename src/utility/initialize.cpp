#include "initialize.h"

#include "macros.h"
#include "src/settings/SettingsManager.h"
#include "src/settings/modules/DebugSettings.h"

#ifdef STORM_LOGGING_FRAMEWORK
log4cplus::Logger logger;
log4cplus::Logger printer;
#else 
int storm_runtime_loglevel = STORM_LOGLEVEL_WARN;
#endif


namespace storm {
    namespace utility {


       void initializeLogger() {
#ifdef STORM_LOGGING_FRAMEWORK
            auto loglevel = storm::settings::debugSettings().isTraceSet() ? log4cplus::TRACE_LOG_LEVEL : storm::settings::debugSettings().isDebugSet() ? log4cplus::DEBUG_LOG_LEVEL : log4cplus::WARN_LOG_LEVEL;
            initializeLogger(loglevel);
#endif
       }

#ifdef STORM_LOGGING_FRAMEWORK
        void initializeLogger(log4cplus::LogLevel const& loglevel) {
            logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("main"));
            log4cplus::SharedAppenderPtr consoleLogAppender(new log4cplus::ConsoleAppender());
            consoleLogAppender->setName("mainConsoleAppender");
            consoleLogAppender->setLayout(std::auto_ptr<log4cplus::Layout>(new log4cplus::PatternLayout("%-5p - %D{%H:%M:%S} (%r ms) - %b:%L: %m%n")));
            logger.addAppender(consoleLogAppender);
            logger.setLogLevel(loglevel);
            consoleLogAppender->setThreshold(loglevel);
        }
#endif

        void setUp() {
            initializeLogger();
            std::cout.precision(10);
        }

        void cleanUp() {
            // Intentionally left empty.
        }

        void initializeFileLogging() {
#ifdef STORM_LOGGING_FRAMEWORK
            log4cplus::SharedAppenderPtr fileLogAppender(new log4cplus::FileAppender(storm::settings::debugSettings().getLogfilename()));
            fileLogAppender->setName("mainFileAppender");
            fileLogAppender->setLayout(std::auto_ptr<log4cplus::Layout>(new log4cplus::PatternLayout("%-5p - %D{%H:%M:%S} (%r ms) - %F:%L: %m%n")));
            logger.addAppender(fileLogAppender);
#endif
        }

    }
}
