#ifndef STORM_UTILITY_INITIALIZE_H_
#define STORM_UTILITY_INITIALIZE_H_

#include "InitializeLogging.h"

/*!
 * Performs some necessary initializations.
 */
void setUp() {
    // Increase the precision of output.
	std::cout.precision(10);
}

/*!
 * Performs some necessary clean-up.
 */
void cleanUp() {
    // Intentionally left empty.
}


#endif