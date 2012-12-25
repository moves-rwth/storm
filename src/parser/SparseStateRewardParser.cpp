/*!
 * SparseStateRewardParser.cpp
 *
 *  Created on: 23.12.2012
 *      Author: Christian Dehnert
 */

#include "src/parser/SparseStateRewardParser.h"
#include "src/exceptions/WrongFileFormatException.h"
#include "src/exceptions/FileIoException.h"

#include "src/utility/OsDetection.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <clocale>
#include <iostream>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <locale.h>

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
extern log4cplus::Logger logger;

namespace storm {
namespace parser {


/*!
 *	Reads a state reward file and puts the result in a state reward vector.
 *
 *	@param stateCount The number of states.
 *	@param filename The filename of the state reward file.
 *	@return A pointer to the created state reward vector.
 */
SparseStateRewardParser::SparseStateRewardParser(uint_fast64_t stateCount, std::string const & filename)
	: stateRewards(nullptr) {
	// Open file.
	MappedFile file(filename.c_str());
	char* buf = file.data;

	// Create state reward vector with given state count.
	this->stateRewards = std::shared_ptr<std::vector<double>>(new std::vector<double>(stateCount));
	
	{
		// Now parse state reward assignments.
		uint_fast64_t state;
		double reward;

		// Iterate over states.
		while (buf[0] != '\0') {
			// Parse state number and reward value.
			state = checked_strtol(buf, &buf);
			reward = strtod(buf, &buf);
			if (reward < 0.0) {
				LOG4CPLUS_ERROR(logger, "Expected positive probability but got \"" << std::string(buf, 0, 16) << "\".");
				throw storm::exceptions::WrongFileFormatException() << "State reward file specifies illegal reward value.";
			}

			(*this->stateRewards)[state] = reward;

			buf = trimWhitespaces(buf);
		}
	}
}

} //namespace parser

} //namespace storm
