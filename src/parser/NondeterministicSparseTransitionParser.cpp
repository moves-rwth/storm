/*!
 *	TraParser.cpp
 *
 *	Created on: 20.11.2012
 *		Author: Gereon Kremer
 */

#include "src/parser/NondeterministicSparseTransitionParser.h"

#include <errno.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <locale.h>

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <clocale>
#include <iostream>
#include <utility>
#include <string>

#include "src/utility/Settings.h"
#include "src/exceptions/FileIoException.h"
#include "src/exceptions/WrongFileFormatException.h"
#include "boost/integer/integer_mask.hpp"
#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
extern log4cplus::Logger logger;

namespace storm {
namespace parser {

/*!
 *	@brief	Perform first pass through the file and obtain overall number of
 *	choices, number of non-zero cells and maximum node id.
 *
 *	This method does the first pass through the transition file.
 *
 *	It computes the overall number of nondeterministic choices, i.e. the
 *	number of rows in the matrix that should be created.
 *	It also calculates the overall number of non-zero cells, i.e. the number
 *	of elements the matrix has to hold, and the maximum node id, i.e. the
 *	number of columns of the matrix.
 *
 *	@param buf Data to scan. Is expected to be some char array.
 *	@param choices Overall number of choices.
 *	@param maxnode Is set to highest id of all nodes.
 *	@return The number of non-zero elements.
 */
uint_fast64_t NondeterministicSparseTransitionParser::firstPass(char* buf, uint_fast64_t& choices, int_fast64_t& maxnode) {
	/*
	 *	Check file header and extract number of transitions.
	 */
	buf = strchr(buf, '\n') + 1;  // skip format hint
	if (strncmp(buf, "STATES ", 7) != 0) {
		LOG4CPLUS_ERROR(logger, "Expected \"STATES\" but got \"" << std::string(buf, 0, 16) << "\".");
		return 0;
	}
	buf += 7;  // skip "STATES "
	if (strtol(buf, &buf, 10) == 0) return 0;
	buf = trimWhitespaces(buf);
	if (strncmp(buf, "TRANSITIONS ", 12) != 0) {
		LOG4CPLUS_ERROR(logger, "Expected \"TRANSITIONS\" but got \"" << std::string(buf, 0, 16) << "\".");
		return 0;
	}
	buf += 12;  // skip "TRANSITIONS "
	/*
	 *	Parse number of transitions.
	 *	We will not actually use this value, but we will compare it to the
	 *	number of transitions we count and issue a warning if this parsed
	 *	value is wrong.
	 */
	uint_fast64_t parsed_nonzero = strtol(buf, &buf, 10);

	/*
	 *	Read all transitions.
	 */
	int_fast64_t source, target, choice, lastchoice = -1;
	int_fast64_t lastsource = -1;
	uint_fast64_t nonzero = 0;
	double val;
	choices = 0;
	maxnode = 0;
	while (buf[0] != '\0') {
		/*
		 *	Read source state and choice.
		 */
		source = checked_strtol(buf, &buf);

		// Read the name of the nondeterministic choice.
		choice = checked_strtol(buf, &buf);

		// Check if we encountered a state index that is bigger than all previously seen.
		if (source > maxnode) {
			maxnode = source;
		}

		// If we have skipped some states, we need to reserve the space for the self-loop insertion
		// in the second pass.
		if (source > lastsource + 1) {
			nonzero += source - lastsource - 1;
			choices += source - lastsource - 1;
			parsed_nonzero += source - lastsource - 1;
		} else if (source != lastsource || choice != lastchoice) {
			// If we have switched the source state or the nondeterministic choice, we need to
			// reserve one row more.
			++choices;
		}

		// Read target and check if we encountered a state index that is bigger than all previously
		// seen.
		target = checked_strtol(buf, &buf);
		if (target > maxnode) {
			maxnode = target;
		}

		// Read value and check whether it's positive.
		val = checked_strtod(buf, &buf);
		if ((val < 0.0) || (val > 1.0)) {
			LOG4CPLUS_ERROR(logger, "Expected a positive probability but got \"" << std::string(buf, 0, 16) << "\".");
			return 0;
		}

		lastchoice = choice;
		lastsource = source;

		/*
		 *	Increase number of non-zero values.
		 */
		nonzero++;

		/*
		 *	Proceed to beginning of next line.
		 */
		buf = trimWhitespaces(buf);
	}

	/*
	 *	Check if the number of transitions given in the file is correct.
	 */
	if (nonzero != parsed_nonzero) {
		LOG4CPLUS_WARN(logger, "File states to have " << parsed_nonzero << " transitions, but I counted " << nonzero << ". Maybe want to fix your file?");
	}
	return nonzero;
}



/*!
 *	Reads a .tra file and produces a sparse matrix representing the described Markov Chain.
 *
 *	Matrices created with this method have to be freed with the delete operator.
 *	@param filename input .tra file's name.
 *	@return a pointer to the created sparse matrix.
 */

NondeterministicSparseTransitionParser::NondeterministicSparseTransitionParser(std::string const &filename)
	: matrix(nullptr) {
	/*
	 *	Enforce locale where decimal point is '.'.
	 */
	setlocale(LC_NUMERIC, "C");

	/*
	 *	Open file.
	 */
	MappedFile file(filename.c_str());
	char* buf = file.data;

	/*
	 *	Perform first pass, i.e. obtain number of columns, rows and non-zero elements.
	 */
	int_fast64_t maxnode;
	uint_fast64_t choices;
	uint_fast64_t nonzero = this->firstPass(file.data, choices, maxnode);

	/*
	 *	If first pass returned zero, the file format was wrong.
	 */
	if (nonzero == 0) {
		LOG4CPLUS_ERROR(logger, "Error while parsing " << filename << ": erroneous file format.");
		throw storm::exceptions::WrongFileFormatException();
	}

	/*
	 *	Perform second pass.
	 *	
	 *	From here on, we already know that the file header is correct.
	 */

	/*
	 *	Read file header, ignore values within.
	 */
	buf = strchr(buf, '\n') + 1;  // skip format hint
	buf += 7;  // skip "STATES "
	checked_strtol(buf, &buf);
	buf = trimWhitespaces(buf);
	buf += 12;  // skip "TRANSITIONS "
	checked_strtol(buf, &buf);

	/*
	 *	Create and initialize matrix.
	 *	The matrix should have as many columns as we have nodes and as many rows as we have choices.
	 *	Those two values, as well as the number of nonzero elements, was been calculated in the first run.
	 */
	LOG4CPLUS_INFO(logger, "Attempting to create matrix of size " << choices << " x " << (maxnode+1) << " with " << nonzero << " entries.");
	this->matrix = std::shared_ptr<storm::storage::SparseMatrix<double>>(new storm::storage::SparseMatrix<double>(choices, maxnode + 1));
	if (this->matrix == nullptr) {
		LOG4CPLUS_ERROR(logger, "Could not create matrix of size " << choices << " x " << (maxnode+1) << ".");
		throw std::bad_alloc();
	}
	this->matrix->initialize(nonzero);

	/*
	 *	Create row mapping.
	 */
	this->rowMapping = std::shared_ptr<std::vector<uint_fast64_t>>(new std::vector<uint_fast64_t>(maxnode+2,0));

	/*
	 *	Parse file content.
	 */
	int_fast64_t source, target, lastsource = -1, choice, lastchoice = -1;
	uint_fast64_t curRow = -1;
	double val;
	bool fixDeadlocks = storm::settings::instance()->isSet("fix-deadlocks");
	bool hadDeadlocks = false;

	/*
	 *	Read all transitions from file.
	 */
	while (buf[0] != '\0') {
		/*
		 *	Read source state and choice.
		 */
		source = checked_strtol(buf, &buf);
		choice = checked_strtol(buf, &buf);

		// Increase line count if we have either finished reading the transitions of a certain state
		// or we have finished reading one nondeterministic choice of a state.
		if ((source != lastsource || choice != lastchoice)) {
			++curRow;
		}

		/*
		 *	Check if we have skipped any source node, i.e. if any node has no
		 *	outgoing transitions. If so, insert a self-loop.
		 *	Also add self-loops to rowMapping.
		 */
		for (int_fast64_t node = lastsource + 1; node < source; node++) {
			hadDeadlocks = true;
			if (fixDeadlocks) {
				this->rowMapping->at(node) = curRow;
				this->matrix->addNextValue(curRow, node, 1);
				++curRow;
				LOG4CPLUS_WARN(logger, "Warning while parsing " << filename << ": node " << node << " has no outgoing transitions. A self-loop was inserted.");
			} else {
				LOG4CPLUS_ERROR(logger, "Error while parsing " << filename << ": node " << node << " has no outgoing transitions.");
			}
		}
		if (source != lastsource) {
			/*
			 *	Add this source to rowMapping, if this is the first choice we encounter for this state.
			 */
			this->rowMapping->at(source) = curRow;
		}

		// Read target and value and write it to the matrix.
		target = checked_strtol(buf, &buf);
		val = checked_strtod(buf, &buf);
		this->matrix->addNextValue(curRow, target, val);

		lastsource = source;
		lastchoice = choice;

		/*
		 *	Proceed to beginning of next line in file and next row in matrix.
		 */
		buf = trimWhitespaces(buf);
	}

	this->rowMapping->at(maxnode+1) = curRow;

	if (!fixDeadlocks && hadDeadlocks) throw storm::exceptions::WrongFileFormatException() << "Some of the nodes had deadlocks. You can use --fix-deadlocks to insert self-loops on the fly.";

	/*
	 * Finalize matrix.
	 */	
	this->matrix->finalize();
}

}  // namespace parser
}  // namespace storm
