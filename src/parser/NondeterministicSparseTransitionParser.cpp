/*!
 *	NondeterministicSparseTransitionParser.cpp
 *
 *	Created on: 20.11.2012
 *		Author: Gereon Kremer
 */

#include "src/parser/NondeterministicSparseTransitionParser.h"

#include <string>

#include "src/settings/Settings.h"
#include "src/exceptions/FileIoException.h"
#include "src/exceptions/WrongFormatException.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
extern log4cplus::Logger logger;

namespace storm {
	namespace parser {

		NondeterministicSparseTransitionParser::Result NondeterministicSparseTransitionParser::parseNondeterministicTransitions(std::string const& filename) {

			RewardMatrixInformationStruct nullInformation;

			return NondeterministicSparseTransitionParser::parse(filename, false, nullInformation);
		}

		NondeterministicSparseTransitionParser::Result NondeterministicSparseTransitionParser::parseNondeterministicTransitionRewards(std::string const& filename, RewardMatrixInformationStruct const& rewardMatrixInformation) {

			return NondeterministicSparseTransitionParser::parse(filename, true, rewardMatrixInformation);
		}

		NondeterministicSparseTransitionParser::FirstPassResult NondeterministicSparseTransitionParser::firstPass(char* buf, SupportedLineEndingsEnum lineEndings, bool isRewardFile, RewardMatrixInformationStruct const& rewardMatrixInformation) {

			// Check file header and extract number of transitions.

			// Skip the format hint if it is there.
			buf = trimWhitespaces(buf);
			if(buf[0] < '0' || buf[0] > '9') {
				buf = storm::parser::forwardToNextLine(buf, lineEndings);
			}

			// Read all transitions.
			uint_fast64_t source = 0, target = 0, choice = 0, lastchoice = 0, lastsource = 0;
			double val = 0.0;
			NondeterministicSparseTransitionParser::FirstPassResult result;

			// Since the first line is already a new choice but is not covered below, that has to be covered here.
			result.choices = 1;

			while (buf[0] != '\0') {

				// Read source state and choice.
				source = checked_strtol(buf, &buf);

				// Read the name of the nondeterministic choice.
				choice = checked_strtol(buf, &buf);

				// Check if we encountered a state index that is bigger than all previously seen.
				if (source > result.highestStateIndex) {
					result.highestStateIndex = source;
				}

				if (isRewardFile) {
					// If we have switched the source state, we possibly need to insert rows for skipped choices of the last
					// source state.
					if (source != lastsource) {
						// number of choices skipped = number of choices of last state - number of choices read
						result.choices += ((*rewardMatrixInformation.nondeterministicChoiceIndices)[lastsource + 1] - (*rewardMatrixInformation.nondeterministicChoiceIndices)[lastsource]) - (lastchoice + 1);
					}

					// If we skipped some states, we need to reserve empty rows for all their nondeterministic
					// choices.
					for (uint_fast64_t i = lastsource + 1; i < source; ++i) {
						result.choices += ((*rewardMatrixInformation.nondeterministicChoiceIndices)[i + 1] - (*rewardMatrixInformation.nondeterministicChoiceIndices)[i]);
					}

					// If we advanced to the next state, but skipped some choices, we have to reserve rows
					// for them.
					if (source != lastsource) {
						result.choices += choice + 1;
					} else if (choice != lastchoice) {
						result.choices += choice - lastchoice;
					}
				} else {

					// If we have skipped some states, we need to reserve the space for the self-loop insertion
					// in the second pass.
					if (source > lastsource + 1) {
						result.numberOfNonzeroEntries += source - lastsource - 1;
						result.choices += source - lastsource - 1;
					} else if (source != lastsource || choice != lastchoice) {
						// If we have switched the source state or the nondeterministic choice, we need to
						// reserve one row more.
						++result.choices;
					}
				}

				// Read target and check if we encountered a state index that is bigger than all previously
				// seen.
				target = checked_strtol(buf, &buf);
				if (target > result.highestStateIndex) {
					result.highestStateIndex = target;
				}

				// Read value and check whether it's positive.
				val = checked_strtod(buf, &buf);
				if ((val < 0.0) || (val > 1.0)) {
					LOG4CPLUS_ERROR(logger, "Expected a positive probability but got \"" << std::string(buf, 0, 16) << "\".");
					NondeterministicSparseTransitionParser::FirstPassResult nullResult;
					return nullResult;
				}

				lastchoice = choice;
				lastsource = source;

				// Increase number of non-zero values.
				result.numberOfNonzeroEntries++;

				// The PRISM output format lists the name of the transition in the fourth column,
				// but omits the fourth column if it is an internal action. In either case we can skip to the end of the line.
				buf = forwardToLineEnd(buf, lineEndings);

				buf = trimWhitespaces(buf);
			}

			if (isRewardFile) {
				// If not all rows were filled for the last state, we need to insert them.
				result.choices += ((*rewardMatrixInformation.nondeterministicChoiceIndices)[lastsource + 1] - (*rewardMatrixInformation.nondeterministicChoiceIndices)[lastsource] ) - (lastchoice + 1);

				// If we skipped some states, we need to reserve empty rows for all their nondeterministic
				// choices.
				for (uint_fast64_t i = lastsource + 1; i < rewardMatrixInformation.nondeterministicChoiceIndices->size() - 1; ++i) {
					result.choices += ((*rewardMatrixInformation.nondeterministicChoiceIndices)[i + 1] - (*rewardMatrixInformation.nondeterministicChoiceIndices)[i]);
				}
			}

			return result;
		}

		NondeterministicSparseTransitionParser::Result NondeterministicSparseTransitionParser::parse(std::string const &filename, bool isRewardFile, RewardMatrixInformationStruct const& rewardMatrixInformation) {

			// Enforce locale where decimal point is '.'.
			setlocale(LC_NUMERIC, "C");

			if (!fileExistsAndIsReadable(filename.c_str())) {
				LOG4CPLUS_ERROR(logger, "Error while parsing " << filename << ": File does not exist or is not readable.");
				throw storm::exceptions::WrongFormatException();
			}

			// Find out about the used line endings.
			SupportedLineEndingsEnum lineEndings = findUsedLineEndings(filename, true);

			// Open file.
			MappedFile file(filename.c_str());
			char* buf = file.data;

			// Perform first pass, i.e. obtain number of columns, rows and non-zero elements.
			NondeterministicSparseTransitionParser::FirstPassResult firstPass = NondeterministicSparseTransitionParser::firstPass(file.data, lineEndings, isRewardFile, rewardMatrixInformation);

			// If first pass returned zero, the file format was wrong.
			if (firstPass.numberOfNonzeroEntries == 0) {
				LOG4CPLUS_ERROR(logger, "Error while parsing " << filename << ": erroneous file format.");
				throw storm::exceptions::WrongFormatException();
			}

			// Perform second pass.

			// Skip the format hint if it is there.
			buf = trimWhitespaces(buf);
			if(buf[0] < '0' || buf[0] > '9') {
				buf = storm::parser::forwardToNextLine(buf, lineEndings);
			}

			if (isRewardFile) {
				// The reward matrix should match the size of the transition matrix.
				if (firstPass.choices > rewardMatrixInformation.rowCount || (uint_fast64_t)(firstPass.highestStateIndex + 1) > rewardMatrixInformation.columnCount) {
					LOG4CPLUS_ERROR(logger, "Reward matrix size exceeds transition matrix size.");
					throw storm::exceptions::WrongFormatException() << "Reward matrix size exceeds transition matrix size.";
				} else if (firstPass.choices != rewardMatrixInformation.rowCount) {
					LOG4CPLUS_ERROR(logger, "Reward matrix row count does not match transition matrix row count.");
					throw storm::exceptions::WrongFormatException() << "Reward matrix row count does not match transition matrix row count.";
				} else {
					firstPass.highestStateIndex = rewardMatrixInformation.columnCount - 1;
				}
			}


			// Create the matrix builder.
			// The matrix to be build should have as many columns as we have nodes and as many rows as we have choices.
			// Those two values, as well as the number of nonzero elements, was been calculated in the first run.
			LOG4CPLUS_INFO(logger, "Attempting to create matrix of size " << firstPass.choices << " x " << (firstPass.highestStateIndex+1) << " with " << firstPass.numberOfNonzeroEntries << " entries.");
			storm::storage::SparseMatrixBuilder<double> matrixBuilder(firstPass.choices, firstPass.highestStateIndex + 1, firstPass.numberOfNonzeroEntries);

			// Create row mapping.
			std::vector<uint_fast64_t> rowMapping(firstPass.highestStateIndex + 2, 0);

			// Initialize variables for the parsing run.
			uint_fast64_t source = 0, target = 0, lastsource = 0, choice = 0, lastchoice = 0, curRow = 0;
			double val = 0.0;
			bool fixDeadlocks = storm::settings::Settings::getInstance()->isSet("fixDeadlocks");
			bool hadDeadlocks = false;

			// Read all transitions from file.
			while (buf[0] != '\0') {

				// Read source state and choice.
				source = checked_strtol(buf, &buf);
				choice = checked_strtol(buf, &buf);

				if (isRewardFile) {
					// If we have switched the source state, we possibly need to insert the rows of the last
					// source state.
					if (source != lastsource) {
						curRow += ((*rewardMatrixInformation.nondeterministicChoiceIndices)[lastsource + 1] - (*rewardMatrixInformation.nondeterministicChoiceIndices)[lastsource]) -(lastchoice + 1);
					}

					// If we skipped some states, we need to reserve empty rows for all their nondeterministic
					// choices.
					for (uint_fast64_t i = lastsource + 1; i < source; ++i) {
						curRow += ((*rewardMatrixInformation.nondeterministicChoiceIndices)[i + 1] - (*rewardMatrixInformation.nondeterministicChoiceIndices)[i]);
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
					// Check if we have skipped any source node, i.e. if any node has no
					// outgoing transitions. If so, insert a self-loop.
					// Also add self-loops to rowMapping.
					for (uint_fast64_t node = lastsource + 1; node < source; node++) {
						hadDeadlocks = true;
						if (fixDeadlocks) {
							rowMapping.at(node) = curRow;
							matrixBuilder.addNextValue(curRow, node, 1);
							++curRow;
							LOG4CPLUS_WARN(logger, "Warning while parsing " << filename << ": node " << node << " has no outgoing transitions. A self-loop was inserted.");
						} else {
							LOG4CPLUS_ERROR(logger, "Error while parsing " << filename << ": node " << node << " has no outgoing transitions.");
						}
					}
					if (source != lastsource) {
						// Add this source to rowMapping, if this is the first choice we encounter for this state.
						rowMapping.at(source) = curRow;
					}
				}

				// Read target and value and write it to the matrix.
				target = checked_strtol(buf, &buf);
				val = checked_strtod(buf, &buf);
				matrixBuilder.addNextValue(curRow, target, val);

				lastsource = source;
				lastchoice = choice;

				// Proceed to beginning of next line in file and next row in matrix.
				buf = forwardToLineEnd(buf, lineEndings);

				buf = trimWhitespaces(buf);
			}

			for (uint_fast64_t node = lastsource + 1; node <= firstPass.highestStateIndex + 1; node++) {
				rowMapping.at(node) = curRow + 1;
			}

			if (!fixDeadlocks && hadDeadlocks && !isRewardFile) throw storm::exceptions::WrongFormatException() << "Some of the nodes had deadlocks. You can use --fixDeadlocks to insert self-loops on the fly.";

			return NondeterministicSparseTransitionParser::Result(matrixBuilder.build(), rowMapping);
		}

	}  // namespace parser
}  // namespace storm
