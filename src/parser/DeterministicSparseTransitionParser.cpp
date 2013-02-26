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
	uint_fast64_t nonZeroEntryCount = 0;
	/*
	 *	Check file header and extract number of transitions.
	 */
	buf = strchr(buf, '\n') + 1;  // skip format hint

    /*
     * Check all transitions for non-zero diagonal entries and deadlock states.
     */
    int_fast64_t row, lastRow = -1, col;
	uint_fast64_t readTransitionCount = 0;
    bool rowHadDiagonalEntry = false;
    double val;
    maxnode = 0;
    while (buf[0] != '\0') {
            /*
             *      Read row and column.
             */
            row = checked_strtol(buf, &buf);
            col = checked_strtol(buf, &buf);

			if (lastRow != row) {
				if ((lastRow != -1) && (!rowHadDiagonalEntry)) {
					++nonZeroEntryCount;
					rowHadDiagonalEntry = true;
				}
				for (int_fast64_t skippedRow = lastRow + 1; skippedRow < row; ++skippedRow) {
					++nonZeroEntryCount;
				}
				lastRow = row;
				rowHadDiagonalEntry = false;
			}

			if (col == row) {
				rowHadDiagonalEntry = true;
			} else if (col > row && !rowHadDiagonalEntry) {
				rowHadDiagonalEntry = true;
				++nonZeroEntryCount;
			}

            /*
             *      Check if one is larger than the current maximum id.
             */
            if (row > maxnode) maxnode = row;
            if (col > maxnode) maxnode = col;

            /*
             *      Read probability of this transition.
             *      Check, if the value is a probability, i.e. if it is between 0 and 1.
             */
            val = checked_strtod(buf, &buf);
            if ((val < 0.0) || (val > 1.0)) {
                    LOG4CPLUS_ERROR(logger, "Expected a positive probability but got \"" << val << "\".");
                    return 0;
            }
			++nonZeroEntryCount;
			++readTransitionCount;
            buf = trimWhitespaces(buf);
    }

    if (!rowHadDiagonalEntry) {
    	++nonZeroEntryCount;
    }

    return nonZeroEntryCount;
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
	int_fast64_t maxStateId;
	uint_fast64_t nonZeroEntryCount = this->firstPass(file.data, maxStateId);

	LOG4CPLUS_INFO(logger, "First pass on " << filename << " shows " << nonZeroEntryCount << " NonZeros.");

	/*
	 *	If first pass returned zero, the file format was wrong.
	 */
	if (nonZeroEntryCount == 0) {
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

	/*
	 *	Creating matrix here.
	 *	The number of non-zero elements is computed by firstPass().
	 */
	LOG4CPLUS_INFO(logger, "Attempting to create matrix of size " << (maxStateId+1) << " x " << (maxStateId+1) << ".");
	this->matrix = std::shared_ptr<storm::storage::SparseMatrix<double>>(new storm::storage::SparseMatrix<double>(maxStateId + 1));
	if (this->matrix == nullptr) {
		LOG4CPLUS_ERROR(logger, "Could not create matrix of size " << (maxStateId+1) << " x " << (maxStateId+1) << ".");
		throw std::bad_alloc();
	}
	this->matrix->initialize(nonZeroEntryCount);

	int_fast64_t row, lastRow = -1, col;
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

		if (lastRow != row) {
			if ((lastRow != -1) && (!rowHadDiagonalEntry)) {
				if (insertDiagonalEntriesIfMissing) {
					this->matrix->addNextValue(lastRow, lastRow, storm::utility::constGetZero<double>());
					LOG4CPLUS_DEBUG(logger, "While parsing " << filename << ": state " << lastRow << " has no transition to itself. Inserted a 0-transition. (1)");
				} else {
					LOG4CPLUS_WARN(logger, "Warning while parsing " << filename << ": state " << lastRow << " has no transition to itself.");
				}
				// No increment for lastRow
				rowHadDiagonalEntry = true;
			}
			for (int_fast64_t skippedRow = lastRow + 1; skippedRow < row; ++skippedRow) {
				hadDeadlocks = true;
				if (fixDeadlocks) {
					this->matrix->addNextValue(skippedRow, skippedRow, storm::utility::constGetOne<double>());
					rowHadDiagonalEntry = true;
					LOG4CPLUS_WARN(logger, "Warning while parsing " << filename << ": state " << skippedRow << " has no outgoing transitions. A self-loop was inserted.");
				} else {
					LOG4CPLUS_ERROR(logger, "Error while parsing " << filename << ": state " << skippedRow << " has no outgoing transitions.");
					// FIXME Why no exception at this point? This will break the App.
				}
			}
			lastRow = row;
			rowHadDiagonalEntry = false;
		}

		if (col == row) {
			rowHadDiagonalEntry = true;
		} else if (col > row && !rowHadDiagonalEntry) {
			rowHadDiagonalEntry = true;
			if (insertDiagonalEntriesIfMissing) {
				this->matrix->addNextValue(row, row, storm::utility::constGetZero<double>());
				LOG4CPLUS_DEBUG(logger, "While parsing " << filename << ": state " << row << " has no transition to itself. Inserted a 0-transition. (2)");
			} else {
				LOG4CPLUS_WARN(logger, "Warning while parsing " << filename << ": state " << row << " has no transition to itself.");
			}
		}

		this->matrix->addNextValue(row, col, val);
		buf = trimWhitespaces(buf);
	}

	if (!rowHadDiagonalEntry) {
		if (insertDiagonalEntriesIfMissing) {
			this->matrix->addNextValue(lastRow, lastRow, storm::utility::constGetZero<double>());
			LOG4CPLUS_DEBUG(logger, "While parsing " << filename << ": state " << lastRow << " has no transition to itself. Inserted a 0-transition. (3)");
		} else {
			LOG4CPLUS_WARN(logger, "Warning while parsing " << filename << ": state " << lastRow << " has no transition to itself.");
		}
	}

	if (!fixDeadlocks && hadDeadlocks) throw storm::exceptions::WrongFileFormatException() << "Some of the nodes had deadlocks. You can use --fix-deadlocks to insert self-loops on the fly.";

	/*
	 *	Finalize Matrix.
	 */	
	this->matrix->finalize();
}

}  // namespace parser
}  // namespace storm
