

#ifndef INITIALIZE_H
#define	INITIALIZE_H


#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
#include "log4cplus/consoleappender.h"
#include "log4cplus/fileappender.h"
log4cplus::Logger logger;
log4cplus::Logger printer;


// Headers that provide auxiliary functionality.
#include "src/utility/storm-version.h"
#include "src/utility/OsDetection.h"
#include "src/settings/SettingsManager.h"

namespace storm {
    namespace utility {
        namespace initialize {
            
            /*!
             * Initializes the logging framework and sets up logging to console.
             */
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
            
            /*!
             * Performs some necessary initializations.
             */
            void setUp() {
                initializeLogger();
                std::cout.precision(10);
            }
            
            /*!
             * Performs some necessary clean-up.
             */
            void cleanUp() {
                // Intentionally left empty.
            }
            
            /*!
             * Sets up the logging to file.
             */
            void initializeFileLogging() {
                log4cplus::SharedAppenderPtr fileLogAppender(new log4cplus::FileAppender(storm::settings::debugSettings().getLogfilename()));
                fileLogAppender->setName("mainFileAppender");
                fileLogAppender->setLayout(std::auto_ptr<log4cplus::Layout>(new log4cplus::PatternLayout("%-5p - %D{%H:%M:%S} (%r ms) - %F:%L: %m%n")));
                logger.addAppender(fileLogAppender);
            }
            
        }
    }
}

#endif	/* INITIALIZE_H */

