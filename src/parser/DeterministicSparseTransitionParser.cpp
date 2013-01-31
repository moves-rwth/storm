/*!
 *	TraParser.cpp
 *
 *	Created on: 20.11.2012
 *		Author: Gereon Kremer
 */

#include "src/parser/DeterministicSparseTransitionParser.h"

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
#include <string>

#include "src/exceptions/FileIoException.h"
#include "src/exceptions/WrongFileFormatException.h"
#include "boost/integer/integer_mask.hpp"
#include "src/utility/Settings.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
extern log4cplus::Logger logger;

namespace storm {
namespace parser {

/*!
 *	@brief	Perform first pass through the file and obtain number of
 *	non-zero cells and maximum node id.
 *
 *	This method does the first pass through the .tra file and computes
 *	the number of non-zero elements.
 *	It also calculates the maximum node id and stores it in maxnode.
 *
 *	@return The number of non-zero elements
 *	@param buf Data to scan. Is expected to be some char array.
 *	@param maxnode Is set to highest id of all nodes.
 */
uint_fast64_t DeterministicSparseTransitionParser::firstPass(char* buf, int_fast64_t& maxnode) {
	uint_fast64_t non_zero = 0;

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
	if ((non_zero = strtol(buf, &buf, 10)) == 0) return 0;

	/*
	 *	Check all transitions for existing diagonal entrys.
	 */
	int_fast64_t row, col;
	int_fast64_t lastDiagonal = -1;
	double val;
	maxnode = 0;
	while (buf[0] != '\0') {
		/*
		 *	Read row and column.
		 */
		row = checked_strtol(buf, &buf);
		col = checked_strtol(buf, &buf);
		/*
		 *	Check if one is larger than the current maximum id.
		 */
		if (row > maxnode) {
			maxnode = row;
		}
		if (col > maxnode) { 
			maxnode = col;
		}
		/*
		 *  Check if a self-loop was skipped. If so, increase non_zero, as the second pass will
		 *	either throw an exception (in this case, it doesn't matter
		 *	anyway) or add a self-loop (in this case, we'll need the
		 *	additional transition).
		 */
		if (lastDiagonal < (row - 1)) {
			non_zero += row - 1 - lastDiagonal;
			lastDiagonal = row - 1;
		}
		/*
		 * Check if this is a self-loop and remember that
		 */
		if (row == col) {
			lastDiagonal = row;
		}

		/*
		 *	Read probability of this transition.
		 *	Check, if the value is a probability, i.e. if it is between 0 and 1.
		 */
		val = checked_strtod(buf, &buf);
		if ((val < 0.0) || (val > 1.0)) {
			LOG4CPLUS_ERROR(logger, "Expected a positive probability but got \"" << val << "\".");
			return 0;
		}
		buf = trimWhitespaces(buf);
	}

	if (lastDiagonal < (row - 1)) {
		non_zero += row - 1 - lastDiagonal;
	}

	return non_zero;
}



/*!
 *	Reads a .tra file and produces a sparse matrix representing the described Markov Chain.
 *
 *	Matrices created with this method have to be freed with the delete operator.
 *	@param filename input .tra file's name.
 *	@return a pointer to the created sparse matrix.
 */

DeterministicSparseTransitionParser::DeterministicSparseTransitionParser(std::string const &filename, bool insertDiagonalEntriesIfMissing)
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
	 *	Perform first pass, i.e. count entries that are not zero.
	 */
	int_fast64_t maxnode;
	uint_fast64_t non_zero = this->firstPass(file.data, maxnode);
	/*
	 *	If first pass returned zero, the file format was wrong.
	 */
	if (non_zero == 0) {
		LOG4CPLUS_ERROR(logger, "Error while parsing " << filename << ": erroneous file format.");
		throw storm::exceptions::WrongFileFormatException();
	}

	/*
	 *	Perform second pass-
	 *	
	 *	From here on, we already know that the file header is correct.
	 */

	/*
	 *	Read file header, extract number of states.
	 */
	buf = strchr(buf, '\n') + 1;  // skip format hint
	buf += 7;  // skip "STATES "
	checked_strtol(buf, &buf);
	buf = trimWhitespaces(buf);
	buf += 12;  // skip "TRANSITIONS "
	checked_strtol(buf, &buf);

	/*
	 *	Creating matrix here.
	 *	The number of non-zero elements is computed by firstPass().
	 */
	LOG4CPLUS_INFO(logger, "Attempting to create matrix of size " << (maxnode+1) << " x " << (maxnode+1) << ".");
	this->matrix = std::shared_ptr<storm::storage::SparseMatrix<double>>(new storm::storage::SparseMatrix<double>(maxnode + 1));
	if (this->matrix == nullptr) {
		LOG4CPLUS_ERROR(logger, "Could not create matrix of size " << (maxnode+1) << " x " << (maxnode+1) << ".");
		throw std::bad_alloc();
	}
	this->matrix->initialize(non_zero);

	int_fast64_t row, lastrow = -1, col;
	double val;
	bool fixDeadlocks = storm::settings::instance()->isSet("fix-deadlocks");
	bool hadDeadlocks = false;
	bool rowHadDiagonalEntry = false;

	/*
	 *	Read all transitions from file. Note that we assume that the
	 *	transitions are listed in canonical order, otherwise this will not
	 *	work, i.e. the values in the matrix will be at wrong places.
	 */
	while (buf[0] != '\0') {
		/*
		 *	Read row, col and value.
		 */
		row = checked_strtol(buf, &buf);
		col = checked_strtol(buf, &buf);
		val = checked_strtod(buf, &buf);

		if (row != lastrow) {
			rowHadDiagonalEntry = false;
		}

		/*
		 *	Check if we skipped a state entirely, i.e. a state does not have any
		 *	outgoing transitions.
		 */
		if ((lastrow + 1) < row) {
			for (int_fast64_t state = lastrow + 1; state < row; ++state) {
				hadDeadlocks = true;
				if (fixDeadlocks) {
					this->matrix->addNextValue(state, state, storm::utility::constGetOne<double>());
					LOG4CPLUS_WARN(logger, "Warning while parsing " << filename << ": state " << state << " has no outgoing transitions. A self-loop was inserted.");
				} else {
					LOG4CPLUS_ERROR(logger, "Error while parsing " << filename << ": state " << state << " has no outgoing transitions.");
				}
			}
		}

		// Insert the missing diagonal value if requested.
		if (col > row && !rowHadDiagonalEntry) {
			if (insertDiagonalEntriesIfMissing) {
				this->matrix->addNextValue(row, row, storm::utility::constGetZero<double>());
			}
			rowHadDiagonalEntry = true;
		}

		/*
		 * Check if the transition is a self-loop
		 */
		if (row == col) {
			rowHadDiagonalEntry = true;
		}

		lastrow = row;

		this->matrix->addNextValue(row, col, val);
		buf = trimWhitespaces(buf);
	}
	if (!fixDeadlocks && hadDeadlocks) throw storm::exceptions::WrongFileFormatException() << "Some of the nodes had deadlocks. You can use --fix-deadlocks to insert self-loops on the fly.";

	/*
	 *	Finalize Matrix.
	 */	
	this->matrix->finalize();
}

}  // namespace parser
}  // namespace storm
