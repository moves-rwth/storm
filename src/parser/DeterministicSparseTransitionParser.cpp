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
#include "src/exceptions/WrongFormatException.h"
#include <cstdint>
#include "src/settings/Settings.h"

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
uint_fast64_t firstPass(char* buf, SupportedLineEndingsEnum lineEndings, uint_fast64_t& maxnode, RewardMatrixInformationStruct* rewardMatrixInformation) {
	bool isRewardMatrix = rewardMatrixInformation != nullptr;

	uint_fast64_t nonZeroEntryCount = 0;

	/*
	 *	Check file header and extract number of transitions.
	 */
	if (!isRewardMatrix) {
		// skip format hint
		buf = storm::parser::forwardToNextLine(buf, lineEndings);
	}

	/*
	 * Check all transitions for non-zero diagonal entries and deadlock states.
	 */
	int_fast64_t lastRow = -1;
	uint_fast64_t row, col;
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

		if (!isRewardMatrix) {
			if (lastRow != (int_fast64_t)row) {
				if ((lastRow != -1) && (!rowHadDiagonalEntry)) {
					++nonZeroEntryCount;
					rowHadDiagonalEntry = true;
				}
				for (uint_fast64_t skippedRow = (uint_fast64_t)(lastRow + 1); skippedRow < row; ++skippedRow) {
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

	if (!rowHadDiagonalEntry && !isRewardMatrix) {
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

storm::storage::SparseMatrix<double> DeterministicSparseTransitionParser(std::string const& filename, bool insertDiagonalEntriesIfMissing, RewardMatrixInformationStruct* rewardMatrixInformation) {
	/*
	 *	Enforce locale where decimal point is '.'.
	 */
	setlocale(LC_NUMERIC, "C");

	bool isRewardMatrix = rewardMatrixInformation != nullptr;

	if (!fileExistsAndIsReadable(filename.c_str())) {
		LOG4CPLUS_ERROR(logger, "Error while parsing " << filename << ": File does not exist or is not readable.");
		throw storm::exceptions::FileIoException() << "The supplied Transition input file \"" << filename << "\" does not exist or is not readable by this process.";
	}

	/*
	 *	Find out about the used line endings.
	 */
	SupportedLineEndingsEnum lineEndings = findUsedLineEndings(filename, true);

	/*
	 *	Open file.
	 */
	MappedFile file(filename.c_str());
	char* buf = file.data;

	/*
	 *	Perform first pass, i.e. count entries that are not zero.
	 */
	uint_fast64_t maxStateId;
	uint_fast64_t nonZeroEntryCount = firstPass(file.data, lineEndings, maxStateId, rewardMatrixInformation);

	LOG4CPLUS_INFO(logger, "First pass on " << filename << " shows " << nonZeroEntryCount << " NonZeros.");

	/*
	 *	If first pass returned zero, the file format was wrong.
	 */
	if (nonZeroEntryCount == 0) {
		LOG4CPLUS_ERROR(logger, "Error while parsing " << filename << ": erroneous file format.");
		throw storm::exceptions::WrongFormatException();
	}

	/*
	 *	Perform second pass-
	 *	
	 *	From here on, we already know that the file header is correct.
	 */

	/*
	 *	Read file header, extract number of states.
	 */
	if (!isRewardMatrix) {
		// skip format hint
		buf = storm::parser::forwardToNextLine(buf, lineEndings);
	}

	// If the matrix that is being parsed is a reward matrix, it should match the size of the
	// transition matrix.
	if (isRewardMatrix) {
		if (maxStateId + 1 > rewardMatrixInformation->rowCount || maxStateId + 1 > rewardMatrixInformation->columnCount) {
			LOG4CPLUS_ERROR(logger, "Reward matrix has more rows or columns than transition matrix.");
			throw storm::exceptions::WrongFormatException() << "Reward matrix has more rows or columns than transition matrix.";
		} else {
			maxStateId = rewardMatrixInformation->rowCount - 1;
		}
	}

	/*
	 *	Creating matrix here.
	 *	The number of non-zero elements is computed by firstPass().
	 */
	LOG4CPLUS_INFO(logger, "Attempting to create matrix of size " << (maxStateId+1) << " x " << (maxStateId+1) << ".");
	storm::storage::SparseMatrix<double> resultMatrix(maxStateId + 1);
	resultMatrix.initialize(nonZeroEntryCount);
	if (!resultMatrix.isInitialized()) {
		LOG4CPLUS_ERROR(logger, "Could not create matrix of size " << (maxStateId+1) << " x " << (maxStateId+1) << ".");
		throw std::bad_alloc();
	}

	int_fast64_t row, lastRow = -1, col;
	double val;
	bool fixDeadlocks = storm::settings::Settings::getInstance()->isSet("fixDeadlocks");
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
				if (insertDiagonalEntriesIfMissing && !isRewardMatrix) {
					resultMatrix.addNextValue(lastRow, lastRow, storm::utility::constGetZero<double>());
					LOG4CPLUS_DEBUG(logger, "While parsing " << filename << ": state " << lastRow << " has no transition to itself. Inserted a 0-transition. (1)");
				} else if (!isRewardMatrix) {
					LOG4CPLUS_WARN(logger, "Warning while parsing " << filename << ": state " << lastRow << " has no transition to itself.");
				}
				// No increment for lastRow
				rowHadDiagonalEntry = true;
			}
			for (int_fast64_t skippedRow = lastRow + 1; skippedRow < row; ++skippedRow) {
				hadDeadlocks = true;
				if (fixDeadlocks && !isRewardMatrix) {
					resultMatrix.addNextValue(skippedRow, skippedRow, storm::utility::constGetOne<double>());
					rowHadDiagonalEntry = true;
					LOG4CPLUS_WARN(logger, "Warning while parsing " << filename << ": state " << skippedRow << " has no outgoing transitions. A self-loop was inserted.");
				} else if (!isRewardMatrix) {
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
			if (insertDiagonalEntriesIfMissing && !isRewardMatrix) {
				resultMatrix.addNextValue(row, row, storm::utility::constGetZero<double>());
				LOG4CPLUS_DEBUG(logger, "While parsing " << filename << ": state " << row << " has no transition to itself. Inserted a 0-transition. (2)");
			} else if (!isRewardMatrix) {
				LOG4CPLUS_WARN(logger, "Warning while parsing " << filename << ": state " << row << " has no transition to itself.");
			}
		}

		resultMatrix.addNextValue(row, col, val);
		buf = trimWhitespaces(buf);
	}

	if (!rowHadDiagonalEntry) {
		if (insertDiagonalEntriesIfMissing && !isRewardMatrix) {
			resultMatrix.addNextValue(lastRow, lastRow, storm::utility::constGetZero<double>());
			LOG4CPLUS_DEBUG(logger, "While parsing " << filename << ": state " << lastRow << " has no transition to itself. Inserted a 0-transition. (3)");
		} else if (!isRewardMatrix) {
			LOG4CPLUS_WARN(logger, "Warning while parsing " << filename << ": state " << lastRow << " has no transition to itself.");
		}
	}

	if (!fixDeadlocks && hadDeadlocks) throw storm::exceptions::WrongFormatException() << "Some of the nodes had deadlocks. You can use --fixDeadlocks to insert self-loops on the fly.";

	/*
	 *	Finalize Matrix.
	 */	
	resultMatrix.finalize();

	return resultMatrix;
}

}  // namespace parser
}  // namespace storm
