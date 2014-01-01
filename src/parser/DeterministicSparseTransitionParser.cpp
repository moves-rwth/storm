/*!
 *	TraParser.cpp
 *
 *	Created on: 20.11.2012
 *		Author: Gereon Kremer
 */

#include "src/parser/DeterministicSparseTransitionParser.h"

#include <cstdio>
#include <cstring>
#include <cstdint>
#include <clocale>
#include <iostream>
#include <string>

#include "src/utility/constants.h"

#include "src/exceptions/FileIoException.h"
#include "src/exceptions/WrongFormatException.h"
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
DeterministicSparseTransitionParser::FirstPassResult DeterministicSparseTransitionParser::firstPass(char* buf, SupportedLineEndingsEnum lineEndings, bool insertDiagonalEntriesIfMissing) {
	DeterministicSparseTransitionParser::FirstPassResult result;

	// Skip the format hint if it is there.
	buf = trimWhitespaces(buf);
	if(buf[0] != '0') {
		buf = storm::parser::forwardToNextLine(buf, lineEndings);
	}

	 // Check all transitions for non-zero diagonal entries and deadlock states.
	uint_fast64_t row, col, lastRow = 0;
	bool rowHadDiagonalEntry = false;
	while (buf[0] != '\0') {

		// Read the transition..
		row = checked_strtol(buf, &buf);
		col = checked_strtol(buf, &buf);
		// The actual read value is not needed here.
		checked_strtod(buf, &buf);

		// Compensate for missing diagonal entries if desired.
		if (insertDiagonalEntriesIfMissing) {
			if (lastRow != row) {
				if(!rowHadDiagonalEntry) {
					++result.numberOfNonzeroEntries;
				}

				// Compensate for missing rows.
				for (uint_fast64_t skippedRow = lastRow + 1; skippedRow < row; ++skippedRow) {
					++result.numberOfNonzeroEntries;
				}
				lastRow = row;
				rowHadDiagonalEntry = false;
			}

			if (col == row) {
				rowHadDiagonalEntry = true;
			}

			if (col > row && !rowHadDiagonalEntry) {
				rowHadDiagonalEntry = true;
				++result.numberOfNonzeroEntries;
			}
		}

		// Check if a higher state id was found.
		if (row > result.highestStateIndex) result.highestStateIndex = row;
		if (col > result.highestStateIndex) result.highestStateIndex = col;

		++result.numberOfNonzeroEntries;
		buf = trimWhitespaces(buf);
	}

	if(insertDiagonalEntriesIfMissing) {
		if (!rowHadDiagonalEntry) {
			++result.numberOfNonzeroEntries;
		}

		//Compensate for missing rows at the end of the file.
		for (uint_fast64_t skippedRow = (uint_fast64_t)(lastRow + 1); skippedRow <= result.highestStateIndex; ++skippedRow) {
			++result.numberOfNonzeroEntries;
		}
	}

	return result;
}

/*!
 *	Reads a .tra file and produces a sparse matrix representing the described Markov Chain.
 *
 *	Matrices created with this method have to be freed with the delete operator.
 *	@param filename input .tra file's name.
 *	@return a pointer to the created sparse matrix.
 */

storm::storage::SparseMatrix<double> DeterministicSparseTransitionParser::parseDeterministicTransitions(std::string const& filename, bool insertDiagonalEntriesIfMissing) {

	// Enforce locale where decimal point is '.'.
	setlocale(LC_NUMERIC, "C");

	if (!fileExistsAndIsReadable(filename.c_str())) {
		LOG4CPLUS_ERROR(logger, "Error while parsing " << filename << ": File does not exist or is not readable.");
		throw storm::exceptions::FileIoException() << "The supplied Transition input file \"" << filename << "\" does not exist or is not readable by this process.";
	}

	// Find out about the used line endings.
	SupportedLineEndingsEnum lineEndings = findUsedLineEndings(filename, true);

	// Open file.
	MappedFile file(filename.c_str());
	char* buf = file.data;

	// Perform first pass, i.e. count entries that are not zero.

	DeterministicSparseTransitionParser::FirstPassResult firstPass = DeterministicSparseTransitionParser::firstPass(file.data, lineEndings, insertDiagonalEntriesIfMissing);

	LOG4CPLUS_INFO(logger, "First pass on " << filename << " shows " << firstPass.numberOfNonzeroEntries << " NonZeros.");

	// If first pass returned zero, the file format was wrong.
	if (firstPass.numberOfNonzeroEntries == 0) {
		LOG4CPLUS_ERROR(logger, "Error while parsing " << filename << ": empty or erroneous file format.");
		throw storm::exceptions::WrongFormatException();
	}

	// Perform second pass.

	// Skip the format hint if it is there.
	buf = trimWhitespaces(buf);
	if(buf[0] != '0') {
		buf = storm::parser::forwardToNextLine(buf, lineEndings);
	}

	// Creating matrix builder here.
	// The number of non-zero elements is computed by firstPass().
	// The contents are inserted during the readout of the file, below.
	// The actual matrix will be build once all contents are inserted.
	storm::storage::SparseMatrixBuilder<double> resultMatrix(firstPass.highestStateIndex + 1, firstPass.highestStateIndex + 1, firstPass.numberOfNonzeroEntries);

	uint_fast64_t row, col, lastRow = 0;
	double val;
	bool fixDeadlocks = storm::settings::Settings::getInstance()->isSet("fixDeadlocks");
	bool hadDeadlocks = false;
	bool rowHadDiagonalEntry = false;


	// Read all transitions from file. Note that we assume that the
	// transitions are listed in canonical order, otherwise this will not
	// work, i.e. the values in the matrix will be at wrong places.
	while (buf[0] != '\0') {

		// Read next transition.
		row = checked_strtol(buf, &buf);
		col = checked_strtol(buf, &buf);
		val = checked_strtod(buf, &buf);

		// Read probability of this transition.
		// Check, if the value is a probability, i.e. if it is between 0 and 1.
		if ((val < 0.0) || (val > 1.0)) {
			LOG4CPLUS_ERROR(logger, "Expected a positive probability but got \"" << val << "\".");
			throw storm::exceptions::WrongFormatException();
		}

		// Test if we moved to a new row.
		// Handle all incomplete or skipped rows.
		if (lastRow != row) {
			if (!rowHadDiagonalEntry) {
				if (insertDiagonalEntriesIfMissing) {
					resultMatrix.addNextValue(lastRow, lastRow, storm::utility::constantZero<double>());
					LOG4CPLUS_DEBUG(logger, "While parsing " << filename << ": state " << lastRow << " has no transition to itself. Inserted a 0-transition. (1)");
				} else {
					LOG4CPLUS_WARN(logger, "Warning while parsing " << filename << ": state " << lastRow << " has no transition to itself.");
				}
				// No increment for lastRow.
				rowHadDiagonalEntry = true;
			}
			for (uint_fast64_t skippedRow = lastRow + 1; skippedRow < row; ++skippedRow) {
				hadDeadlocks = true;
				if (fixDeadlocks) {
					resultMatrix.addNextValue(skippedRow, skippedRow, storm::utility::constantOne<double>());
					rowHadDiagonalEntry = true;
					LOG4CPLUS_WARN(logger, "Warning while parsing " << filename << ": state " << skippedRow << " has no outgoing transitions. A self-loop was inserted.");
				} else {
					LOG4CPLUS_ERROR(logger, "Error while parsing " << filename << ": state " << skippedRow << " has no outgoing transitions.");
					// Before throwing the appropriate exception we will give notice of all deadlock states.
				}
			}
			lastRow = row;
			rowHadDiagonalEntry = false;
		}

		if (col == row) {
			rowHadDiagonalEntry = true;
		}

		if (col > row && !rowHadDiagonalEntry) {
			if (insertDiagonalEntriesIfMissing) {
				resultMatrix.addNextValue(row, row, storm::utility::constantZero<double>());
				LOG4CPLUS_DEBUG(logger, "While parsing " << filename << ": state " << row << " has no transition to itself. Inserted a 0-transition. (2)");
			} else {
				LOG4CPLUS_WARN(logger, "Warning while parsing " << filename << ": state " << row << " has no transition to itself.");
			}
			rowHadDiagonalEntry = true;
		}

		resultMatrix.addNextValue(row, col, val);
		buf = trimWhitespaces(buf);
	}

	if (!rowHadDiagonalEntry) {
		if (insertDiagonalEntriesIfMissing) {
			resultMatrix.addNextValue(lastRow, lastRow, storm::utility::constantZero<double>());
			LOG4CPLUS_DEBUG(logger, "While parsing " << filename << ": state " << lastRow << " has no transition to itself. Inserted a 0-transition. (3)");
		} else {
			LOG4CPLUS_WARN(logger, "Warning while parsing " << filename << ": state " << lastRow << " has no transition to itself.");
		}
	}

	// If we encountered deadlock and did not fix them, now is the time to throw the exception.
	if (!fixDeadlocks && hadDeadlocks) throw storm::exceptions::WrongFormatException() << "Some of the nodes had deadlocks. You can use --fixDeadlocks to insert self-loops on the fly.";

	return resultMatrix.build();
}

/*!
 *	Reads a .tra file and produces a sparse matrix representing the described Markov Chain.
 *
 *	Matrices created with this method have to be freed with the delete operator.
 *	@param filename input .tra file's name.
 *	@return a pointer to the created sparse matrix.
 */

storm::storage::SparseMatrix<double> DeterministicSparseTransitionParser::parseDeterministicTransitionRewards(std::string const& filename, RewardMatrixInformationStruct const& rewardMatrixInformation) {
	// Enforce locale where decimal point is '.'.
	setlocale(LC_NUMERIC, "C");

	if (!fileExistsAndIsReadable(filename.c_str())) {
		LOG4CPLUS_ERROR(logger, "Error while parsing " << filename << ": File does not exist or is not readable.");
		throw storm::exceptions::FileIoException() << "The supplied Transition input file \"" << filename << "\" does not exist or is not readable by this process.";
	}

	// Find out about the used line endings.
	SupportedLineEndingsEnum lineEndings = findUsedLineEndings(filename, true);

	// Open file.
	MappedFile file(filename.c_str());
	char* buf = file.data;

	// Perform first pass, i.e. count entries that are not zero.

	DeterministicSparseTransitionParser::FirstPassResult firstPass = DeterministicSparseTransitionParser::firstPass(file.data, lineEndings, false);

	LOG4CPLUS_INFO(logger, "First pass on " << filename << " shows " << firstPass.numberOfNonzeroEntries << " NonZeros.");

	// If first pass returned zero, the file format was wrong.
	if (firstPass.numberOfNonzeroEntries == 0) {
		LOG4CPLUS_ERROR(logger, "Error while parsing " << filename << ": empty or erroneous file format.");
		throw storm::exceptions::WrongFormatException();
	}

	// Perform second pass.

	// Skip the format hint if it is there.
	buf = trimWhitespaces(buf);
	if(buf[0] != '0') {
		buf = storm::parser::forwardToNextLine(buf, lineEndings);
	}

	// The reward matrix should match the size of the transition matrix.
	if (firstPass.highestStateIndex + 1 > rewardMatrixInformation.rowCount || firstPass.highestStateIndex + 1 > rewardMatrixInformation.columnCount) {
		LOG4CPLUS_ERROR(logger, "Reward matrix has more rows or columns than transition matrix.");
		throw storm::exceptions::WrongFormatException() << "Reward matrix has more rows or columns than transition matrix.";
	} else {
		// If we found the right number of states or less, we set it to the number of states represented by the transition matrix.
		firstPass.highestStateIndex = rewardMatrixInformation.rowCount - 1;
	}


	// Creating matrix builder here.
	// The number of non-zero elements is computed by firstPass().
	// The contents are inserted during the readout of the file, below.
	// The actual matrix will be build once all contents are inserted.
	storm::storage::SparseMatrixBuilder<double> resultMatrix(firstPass.highestStateIndex + 1, firstPass.highestStateIndex + 1, firstPass.numberOfNonzeroEntries);

	uint_fast64_t row, col;
	double val;

	// Read all transitions from file. Note that we assume that the
	// transitions are listed in canonical order, otherwise this will not
	// work, i.e. the values in the matrix will be at wrong places.
	while (buf[0] != '\0') {

		// Read next transition.
		row = checked_strtol(buf, &buf);
		col = checked_strtol(buf, &buf);
		val = checked_strtod(buf, &buf);

		resultMatrix.addNextValue(row, col, val);
		buf = trimWhitespaces(buf);
	}

	return resultMatrix.build();
}

}  // namespace parser

}  // namespace storm
