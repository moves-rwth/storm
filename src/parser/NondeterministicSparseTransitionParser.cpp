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
uint_fast64_t NondeterministicSparseTransitionParser::firstPass(char* buf, uint_fast64_t& choices, int_fast64_t& maxnode, RewardMatrixInformationStruct* rewardMatrixInformation) {
	bool isRewardFile = rewardMatrixInformation != nullptr;

	/*
	 *	Check file header and extract number of transitions.
	 */
	if (!isRewardFile) {
		buf = strchr(buf, '\n') + 1;  // skip format hint
	}

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

		if (isRewardFile) {
			// If we have switched the source state, we possibly need to insert the rows of the last
			// last source state.
			if (source != lastsource && lastsource != -1) {
				choices += lastchoice - ((*rewardMatrixInformation->nondeterministicChoiceIndices)[lastsource + 1] - (*rewardMatrixInformation->nondeterministicChoiceIndices)[lastsource] - 1);
			}

			// If we skipped some states, we need to reserve empty rows for all their nondeterministic
			// choices.
			for (int_fast64_t i = lastsource + 1; i < source; ++i) {
				choices += ((*rewardMatrixInformation->nondeterministicChoiceIndices)[i + 1] - (*rewardMatrixInformation->nondeterministicChoiceIndices)[i]);
			}

			// If we advanced to the next state, but skipped some choices, we have to reserve rows
			// for them
			if (source != lastsource) {
				choices += choice + 1;
			} else if (choice != lastchoice) {
				choices += choice - lastchoice;
			}
		} else {
			// If we have skipped some states, we need to reserve the space for the self-loop insertion
			// in the second pass.
			if (source > lastsource + 1) {
				nonzero += source - lastsource - 1;
				choices += source - lastsource - 1;
			} else if (source != lastsource || choice != lastchoice) {
				// If we have switched the source state or the nondeterministic choice, we need to
				// reserve one row more.
				++choices;
			}
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

		// The PRISM output format lists the name of the transition in the fourth column,
		// but omits the fourth column if it is an internal action. In either case, however, the third column
		// is followed by a space. We need to skip over that space first (instead of trimming whitespaces),
		// before we can skip to the line end, because trimming the white spaces will proceed to the next line
		// in case there is no action label in the fourth column.
		if (buf[0] == ' ') {
			++buf;
		}

		/*
		 *	Proceed to beginning of next line.
		 */
		buf += strcspn(buf, " \t\n\r");
		buf = trimWhitespaces(buf);
	}

	if (isRewardFile) {
		// If not all rows were filled for the last state, we need to insert them.
		choices += lastchoice - ((*rewardMatrixInformation->nondeterministicChoiceIndices)[lastsource + 1] - (*rewardMatrixInformation->nondeterministicChoiceIndices)[lastsource] - 1);

		// If we skipped some states, we need to reserve empty rows for all their nondeterministic
		// choices.
		for (uint_fast64_t i = lastsource + 1; i < rewardMatrixInformation->nondeterministicChoiceIndices->size() - 1; ++i) {
			choices += ((*rewardMatrixInformation->nondeterministicChoiceIndices)[i + 1] - (*rewardMatrixInformation->nondeterministicChoiceIndices)[i]);
		}
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

NondeterministicSparseTransitionParser::NondeterministicSparseTransitionParser(std::string const &filename, RewardMatrixInformationStruct* rewardMatrixInformation)
	: matrix(nullptr) {
	/*
	 *	Enforce locale where decimal point is '.'.
	 */
	setlocale(LC_NUMERIC, "C");

	bool isRewardFile = rewardMatrixInformation != nullptr;

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
	uint_fast64_t nonzero = this->firstPass(file.data, choices, maxnode, rewardMatrixInformation);

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
	 *	Skip file header.
	 */
	if (!isRewardFile) {
		buf = strchr(buf, '\n') + 1;  // skip format hint
	}

	if (isRewardFile) {
		if (choices > rewardMatrixInformation->rowCount || (uint_fast64_t)(maxnode + 1) > rewardMatrixInformation->columnCount) {
			LOG4CPLUS_ERROR(logger, "Reward matrix size exceeds transition matrix size.");
			throw storm::exceptions::WrongFileFormatException() << "Reward matrix size exceeds transition matrix size.";
		} else if (choices != rewardMatrixInformation->rowCount) {
			LOG4CPLUS_ERROR(logger, "Reward matrix row count does not match transition matrix row count.");
			throw storm::exceptions::WrongFileFormatException() << "Reward matrix row count does not match transition matrix row count.";
		} else {
			maxnode = rewardMatrixInformation->columnCount - 1;
		}
	}

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

		if (isRewardFile) {
			// If we have switched the source state, we possibly need to insert the rows of the last
			// last source state.
			if (source != lastsource && lastsource != -1) {
				curRow += lastchoice - ((*rewardMatrixInformation->nondeterministicChoiceIndices)[lastsource + 1] - (*rewardMatrixInformation->nondeterministicChoiceIndices)[lastsource] - 1);
			}

			// If we skipped some states, we need to reserve empty rows for all their nondeterministic
			// choices.
			for (int_fast64_t i = lastsource + 1; i < source; ++i) {
				curRow += ((*rewardMatrixInformation->nondeterministicChoiceIndices)[i + 1] - (*rewardMatrixInformation->nondeterministicChoiceIndices)[i]);
			}

			// If we advanced to the next state, but skipped some choices, we have to reserve rows
			// for them
			if (source != lastsource) {
				curRow += choice + 1;
			} else if (choice != lastchoice) {
				curRow += choice - lastchoice;
			}
		} else {
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
		if (buf[0] == ' ') {
			++buf;
		}
		buf += strcspn(buf, " \t\n\r");
		buf = trimWhitespaces(buf);
	}

	for (int_fast64_t node = lastsource + 1; node <= maxnode + 1; node++) {
		this->rowMapping->at(node) = curRow + 1;
	}

	if (!fixDeadlocks && hadDeadlocks && !isRewardFile) throw storm::exceptions::WrongFileFormatException() << "Some of the nodes had deadlocks. You can use --fix-deadlocks to insert self-loops on the fly.";

	/*
	 * Finalize matrix.
	 */	
	this->matrix->finalize();
}

}  // namespace parser
}  // namespace storm
