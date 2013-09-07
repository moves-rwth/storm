/*
 * StormOptions.h
 * 
 * All shared options are declared here, so that they can be reused in the Tests
 *
 *  Created on: 07.09.2013
 *      Author: Philipp Berger
 */

#ifndef STORM_UTILITY_STORMOPTIONS_H_
#define STORM_UTILITY_STORMOPTIONS_H_

#include "src/settings/Settings.h"


namespace storm {
	namespace utility {

		class StormOptions {
		private:
			StormOptions() {}
			StormOptions(StormOptions& other) {}
			~StormOptions() {}

			static bool optionsRegistered;
		};
		
	}
}

#endif // STORM_UTILITY_STORMOPTIONS_H_