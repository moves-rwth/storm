/*!
 * SparseStateRewardParser.cpp
 *
 *  Created on: 23.12.2012
 *      Author: Christian Dehnert
 */

#include "src/parser/SparseStateRewardParser.h"

#include "src/exceptions/WrongFormatException.h"
#include "src/utility/cstring.h"
#include "src/parser/MappedFile.h"
#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
extern log4cplus::Logger logger;

namespace storm {
	namespace parser {

		using namespace storm::utility::cstring;

		std::vector<double> SparseStateRewardParser::parseSparseStateReward(uint_fast64_t stateCount, std::string const & filename) {
			// Open file.
			if (!MappedFile::fileExistsAndIsReadable(filename.c_str())) {
				LOG4CPLUS_ERROR(logger, "Error while parsing " << filename << ": File does not exist or is not readable.");
				throw storm::exceptions::WrongFormatException();
			}

			MappedFile file(filename.c_str());
			char* buf = file.getData();

			// Create state reward vector with given state count.
			std::vector<double> stateRewards(stateCount);

			// Now parse state reward assignments.
			uint_fast64_t state;
			double reward;

			// Iterate over states.
			while (buf[0] != '\0') {
				// Parse state number and reward value.
				state = checked_strtol(buf, &buf);
				reward = checked_strtod(buf, &buf);
				if (reward < 0.0) {
					LOG4CPLUS_ERROR(logger, "Expected positive reward value but got \"" << reward << "\".");
					throw storm::exceptions::WrongFormatException() << "State reward file specifies illegal reward value.";
				}

				stateRewards[state] = reward;

				buf = trimWhitespaces(buf);
			}
			return stateRewards;
		}

	}  // namespace parser
}  // namespace storm
