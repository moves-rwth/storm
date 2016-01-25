#ifndef STORM_UTILITY_INITIALIZE_H
#define	STORM_UTILITY_INITIALIZE_H




#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
#include "log4cplus/consoleappender.h"
#include "log4cplus/fileappender.h"
#include "macros.h"

#include "src/settings/SettingsManager.h"


namespace storm {
    namespace utility {
        /*!
         * Initializes the logging framework and sets up logging to console.
         */
        void initializeLogger();
        void initializeLogger(log4cplus::LogLevel const&);

        /*!
         * Performs some necessary initializations.
         */
        void setUp();

        /*!
         * Performs some necessary clean-up.
         */
        void cleanUp();

        /*!
         * Sets up the logging to file.
         */
        void initializeFileLogging();

    }
}

#endif	/* INITIALIZE_H */

