#ifndef STORM_UTILITY_INITIALIZE_H
#define	STORM_UTILITY_INITIALIZE_H




#include "macros.h"

#include "src/settings/SettingsManager.h"


namespace storm {
    namespace utility {
        /*!
         * Initializes the logging framework and sets up logging to console.
         */
        void initializeLogger();
#ifdef STORM_LOGGING_FRAMEWORK
        void initializeLogger(log4cplus::LogLevel const&);
#endif
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

