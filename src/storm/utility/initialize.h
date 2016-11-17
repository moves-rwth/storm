#ifndef STORM_UTILITY_INITIALIZE_H
#define	STORM_UTILITY_INITIALIZE_H

#include "src/storm/utility/logging.h"

namespace storm {
    namespace utility {
        /*!
         * Initializes the logging framework and sets up logging to console.
         */
        void initializeLogger();
        /*!
         * Performs some necessary initializations.
         */
        void setUp();

        /*!
         * Performs some necessary clean-up.
         */
        void cleanUp();

        /*!
         * Set the global log level
         */
        void setLogLevel(l3pp::LogLevel level);

        /*!
         * Sets up the logging to file.
         */
        void initializeFileLogging();

    }
}

#endif	/* STORM_UTILITY_INITIALIZE_H */

