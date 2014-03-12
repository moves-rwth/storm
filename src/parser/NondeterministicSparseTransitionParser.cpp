/*!
 *	NondeterministicSparseTransitionParser.cpp
 *
 *	Created on: 20.11.2012
 *		Author: Gereon Kremer
 */

#include "src/parser/NondeterministicSparseTransitionParser.h"

#include <string>

#include "src/parser/MappedFile.h"
#include "src/settings/Settings.h"
#include "src/exceptions/FileIoException.h"
#include "src/exceptions/OutOfRangeException.h"
#include "src/exceptions/WrongFormatException.h"

#include "src/utility/cstring.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
extern log4cplus::Logger logger;

namespace storm {
	namespace parser {

		using namespace storm::utility::cstring;

		storm::storage::SparseMatrix<double> NondeterministicSparseTransitionParser::parseNondeterministicTransitions(std::string const& filename) {

			storm::storage::SparseMatrix<double> emptyMatrix;

			return NondeterministicSparseTransitionParser::parse(filename, false, emptyMatrix);
		}

		storm::storage::SparseMatrix<double> NondeterministicSparseTransitionParser::parseNondeterministicTransitionRewards(std::string const& filename, storm::storage::SparseMatrix<double> const & modelInformation) {

			return NondeterministicSparseTransitionParser::parse(filename, true, modelInformation);
		}

		storm::storage::SparseMatrix<double> NondeterministicSparseTransitionParser::parse(std::string const &filename, bool isRewardFile, storm::storage::SparseMatrix<double> const & modelInformation) {

			// Enforce locale where decimal point is '.'.
			setlocale(LC_NUMERIC, "C");

			if (!MappedFile::fileExistsAndIsReadable(filename.c_str())) {
				LOG4CPLUS_ERROR(logger, "Error while parsing " << filename << ": File does not exist or is not readable.");
				throw storm::exceptions::FileIoException() << "Error while parsing " << filename << ": File does not exist or is not readable.";
			}

			// Open file.
			MappedFile file(filename.c_str());
			char* buf = file.getData();

			// Perform first pass, i.e. obtain number of columns, rows and non-zero elements.
			NondeterministicSparseTransitionParser::FirstPassResult firstPass = NondeterministicSparseTransitionParser::firstPass(file.getData(), isRewardFile, modelInformation);

			// If first pass returned zero, the file format was wrong.
			if (firstPass.numberOfNonzeroEntries == 0) {
				LOG4CPLUS_ERROR(logger, "Error while parsing " << filename << ": erroneous file format.");
				throw storm::exceptions::WrongFormatException() << "Error while parsing " << filename << ": erroneous file format.";
			}

			// Perform second pass.

			// Skip the format hint if it is there.
			buf = trimWhitespaces(buf);
			if(buf[0] < '0' || buf[0] > '9') {
				buf = forwardToLineEnd(buf);
				buf = trimWhitespaces(buf);
			}

			if (isRewardFile) {
				// The reward matrix should match the size of the transition matrix.
				if (firstPass.choices > modelInformation.getRowCount() || (uint_fast64_t)(firstPass.highestStateIndex + 1) > modelInformation.getColumnCount()) {
					LOG4CPLUS_ERROR(logger, "Reward matrix size exceeds transition matrix size.");
					throw storm::exceptions::OutOfRangeException() << "Reward matrix size exceeds transition matrix size.";
				} else if (firstPass.choices != modelInformation.getRowCount()) {
					LOG4CPLUS_ERROR(logger, "Reward matrix row count does not match transition matrix row count.");
					throw storm::exceptions::OutOfRangeException() << "Reward matrix row count does not match transition matrix row count.";
				} else if(firstPass.numberOfNonzeroEntries > modelInformation.getEntryCount()) {
					LOG4CPLUS_ERROR(logger, "The reward matrix has more entries than the transition matrix. There must be a reward for a non existent transition");
					throw storm::exceptions::OutOfRangeException() << "The reward matrix has more entries than the transition matrix.";
				} else {
					firstPass.highestStateIndex = modelInformation.getColumnCount() - 1;
				}
			}

			// Create the matrix builder.
			// The matrix to be build should have as many columns as we have nodes and as many rows as we have choices.
			// Those two values, as well as the number of nonzero elements, was been calculated in the first run.
			LOG4CPLUS_INFO(logger, "Attempting to create matrix of size " << firstPass.choices << " x " << (firstPass.highestStateIndex+1) << " with " << firstPass.numberOfNonzeroEntries << " entries.");
			storm::storage::SparseMatrixBuilder<double> matrixBuilder;
			if(!isRewardFile) {
				matrixBuilder = storm::storage::SparseMatrixBuilder<double>(firstPass.choices, firstPass.highestStateIndex + 1, firstPass.numberOfNonzeroEntries, true, firstPass.highestStateIndex + 1);
			} else {
				matrixBuilder = storm::storage::SparseMatrixBuilder<double>(firstPass.choices, firstPass.highestStateIndex + 1, firstPass.numberOfNonzeroEntries, true, modelInformation.getRowGroupCount());
			}

			// Initialize variables for the parsing run.
			uint_fast64_t source = 0, target = 0, lastSource = 0, choice = 0, lastChoice = 0, curRow = 0;
			double val = 0.0;
			bool fixDeadlocks = storm::settings::Settings::getInstance()->isSet("fixDeadlocks");
			bool hadDeadlocks = false;

			// The first state already starts a new row group of the matrix.
			matrixBuilder.newRowGroup(0);

			// Read all transitions from file.
			while (buf[0] != '\0') {

				// Read source state and choice.
				source = checked_strtol(buf, &buf);
				choice = checked_strtol(buf, &buf);

				if (isRewardFile) {
					// If we have switched the source state, we possibly need to insert the rows of the last
					// source state.
					if (source != lastSource) {
						curRow += ((modelInformation.getRowGroupIndices())[lastSource + 1] - (modelInformation.getRowGroupIndices())[lastSource]) -(lastChoice + 1);
					}

					// If we skipped some states, we need to reserve empty rows for all their nondeterministic
					// choices.
					for (uint_fast64_t i = lastSource + 1; i < source; ++i) {
						curRow += ((modelInformation.getRowGroupIndices())[i + 1] - (modelInformation.getRowGroupIndices())[i]);
					}

					// If we advanced to the next state, but skipped some choices, we have to reserve rows
					// for them
					if (source != lastSource) {
						curRow += choice + 1;
					} else if (choice != lastChoice) {
						curRow += choice - lastChoice;
					}
				} else {
					// Increase line count if we have either finished reading the transitions of a certain state
					// or we have finished reading one nondeterministic choice of a state.
					if ((source != lastSource || choice != lastChoice)) {
						++curRow;
					}

					// Check if we have skipped any source node, i.e. if any node has no
					// outgoing transitions. If so, insert a self-loop.
					// Also begin a new rowGroup for the skipped state.
					for (uint_fast64_t node = lastSource + 1; node < source; node++) {
						hadDeadlocks = true;
						if (fixDeadlocks) {
							matrixBuilder.newRowGroup(curRow);
							matrixBuilder.addNextValue(curRow, node, 1);
							++curRow;
							LOG4CPLUS_WARN(logger, "Warning while parsing " << filename << ": node " << node << " has no outgoing transitions. A self-loop was inserted.");
						} else {
							LOG4CPLUS_ERROR(logger, "Error while parsing " << filename << ": node " << node << " has no outgoing transitions.");
						}
					}
					if (source != lastSource) {
						// Add create a new rowGroup for the source, if this is the first choice we encounter for this state.
						matrixBuilder.newRowGroup(curRow);
					}
				}

				// Read target and value and write it to the matrix.
				target = checked_strtol(buf, &buf);
				val = checked_strtod(buf, &buf);
				matrixBuilder.addNextValue(curRow, target, val);

				lastSource = source;
				lastChoice = choice;

				// Proceed to beginning of next line in file and next row in matrix.
				buf = forwardToLineEnd(buf);

				buf = trimWhitespaces(buf);
			}

			if (!fixDeadlocks && hadDeadlocks && !isRewardFile) throw storm::exceptions::WrongFormatException() << "Some of the nodes had deadlocks. You can use --fixDeadlocks to insert self-loops on the fly.";

			// Since we assume the transition rewards are for the transitions of the model, we copy the rowGroupIndices.
			if(isRewardFile) {
				// We already have rowGroup 0.
				for(uint_fast64_t index = 1; index < modelInformation.getRowGroupIndices().size(); index++) {
					matrixBuilder.newRowGroup(modelInformation.getRowGroupIndices()[index]);
				}
			} else {
				for (uint_fast64_t node = lastSource + 1; node <= firstPass.highestStateIndex; node++) {
					matrixBuilder.newRowGroup(curRow + 1);
				}
			}

			// Finally, build the actual matrix, test and return it.
			storm::storage::SparseMatrix<double> resultMatrix = matrixBuilder.build();

			// Since we cannot do the testing if each transition for which there is a reward in the reward file also exists in the transition matrix during parsing, we have to do it afterwards.
			if(isRewardFile && !resultMatrix.isSubmatrixOf(modelInformation)) {
				LOG4CPLUS_ERROR(logger, "There are rewards for non existent transitions given in the reward file.");
				throw storm::exceptions::WrongFormatException() << "There are rewards for non existent transitions given in the reward file.";
			}

			return resultMatrix;
		}

		NondeterministicSparseTransitionParser::FirstPassResult NondeterministicSparseTransitionParser::firstPass(char* buf, bool isRewardFile, storm::storage::SparseMatrix<double> const & modelInformation) {

			// Check file header and extract number of transitions.

			// Skip the format hint if it is there.
			buf = trimWhitespaces(buf);
			if(buf[0] < '0' || buf[0] > '9') {
				buf = forwardToLineEnd(buf);
				buf = trimWhitespaces(buf);
			}

			// Read all transitions.
			uint_fast64_t source = 0, target = 0, choice = 0, lastChoice = 0, lastSource = 0, lastTarget = -1;
			double val = 0.0;
			NondeterministicSparseTransitionParser::FirstPassResult result;

			// Since the first line is already a new choice but is not covered below, that has to be covered here.
			result.choices = 1;

			while (buf[0] != '\0') {

				// Read source state and choice.
				source = checked_strtol(buf, &buf);

				// Read the name of the nondeterministic choice.
				choice = checked_strtol(buf, &buf);

				if (source < lastSource) {
					LOG4CPLUS_ERROR(logger, "The current source state " << source << " is smaller than the last one " << lastSource << ".");
					throw storm::exceptions::InvalidArgumentException() << "The current source state " << source << " is smaller than the last one " << lastSource << ".";
				}

				// Check if we encountered a state index that is bigger than all previously seen.
				if (source > result.highestStateIndex) {
					result.highestStateIndex = source;
				}

				if (isRewardFile) {
					// If we have switched the source state, we possibly need to insert rows for skipped choices of the last
					// source state.
					if (source != lastSource) {
						// number of choices skipped = number of choices of last state - number of choices read
						result.choices += ((modelInformation.getRowGroupIndices())[lastSource + 1] - (modelInformation.getRowGroupIndices())[lastSource]) - (lastChoice + 1);
					}

					// If we skipped some states, we need to reserve empty rows for all their nondeterministic
					// choices.
					for (uint_fast64_t i = lastSource + 1; i < source; ++i) {
						result.choices += ((modelInformation.getRowGroupIndices())[i + 1] - (modelInformation.getRowGroupIndices())[i]);
					}

					// If we advanced to the next state, but skipped some choices, we have to reserve rows
					// for them.
					if (source != lastSource) {
						result.choices += choice + 1;
					} else if (choice != lastChoice) {
						result.choices += choice - lastChoice;
					}
				} else {

					// If we have skipped some states, we need to reserve the space for the self-loop insertion
					// in the second pass.
					if (source > lastSource + 1) {
						result.numberOfNonzeroEntries += source - lastSource - 1;
						result.choices += source - lastSource - 1;
					}

					if (source != lastSource || choice != lastChoice) {
						// If we have switched the source state or the nondeterministic choice, we need to
						// reserve one row more.
						++result.choices;
					}
				}

				// Read target and check if we encountered a state index that is bigger than all previously seen.
				target = checked_strtol(buf, &buf);

				if (target > result.highestStateIndex) {
					result.highestStateIndex = target;
				}

				// Also, have we already seen this transition?
				if (target == lastTarget && choice == lastChoice && source == lastSource) {
					LOG4CPLUS_ERROR(logger, "The same transition (" << source << ", " << choice << ", " << target << ") is given twice.");
					throw storm::exceptions::InvalidArgumentException() << "The same transition (" << source << ", " << choice << ", " << target << ") is given twice.";
				}

				// Read value and check whether it's positive.
				val = checked_strtod(buf, &buf);
				if (!isRewardFile && (val < 0.0 || val > 1.0 )) {
					LOG4CPLUS_ERROR(logger, "Expected a positive probability but got \"" << std::string(buf, 0, 16) << "\".");
					NondeterministicSparseTransitionParser::FirstPassResult nullResult;
					return nullResult;
				}
				else if (val < 0.0) {
					LOG4CPLUS_ERROR(logger, "Expected a positive reward value but got \"" << std::string(buf, 0, 16) << "\".");
					NondeterministicSparseTransitionParser::FirstPassResult nullResult;
					return nullResult;
				}

				lastChoice = choice;
				lastSource = source;
				lastTarget = target;

				// Increase number of non-zero values.
				result.numberOfNonzeroEntries++;

				// The PRISM output format lists the name of the transition in the fourth column,
				// but omits the fourth column if it is an internal action. In either case we can skip to the end of the line.
				buf = forwardToLineEnd(buf);

				buf = trimWhitespaces(buf);
			}

			if (isRewardFile) {
				// If not all rows were filled for the last state, we need to insert them.
				result.choices += ((modelInformation.getRowGroupIndices())[lastSource + 1] - (modelInformation.getRowGroupIndices())[lastSource] ) - (lastChoice + 1);

				// If we skipped some states, we need to reserve empty rows for all their nondeterministic
				// choices.
				for (uint_fast64_t i = lastSource + 1; i < modelInformation.getRowGroupIndices().size() - 1; ++i) {
					result.choices += ((modelInformation.getRowGroupIndices())[i + 1] - (modelInformation.getRowGroupIndices())[i]);
				}
			}

			return result;
		}

	}  // namespace parser
}  // namespace storm
