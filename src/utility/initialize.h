#ifndef STORM_UTILITY_INITIALIZE_H
#define	STORM_UTILITY_INITIALIZE_H

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
         * Sets up the logging to file.
         */
        void initializeFileLogging();

    }
}

#endif	/* STORM_UTILITY_INITIALIZE_H */

