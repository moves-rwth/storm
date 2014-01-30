#include "MarkovAutomatonSparseTransitionParser.h"

#include "src/settings/Settings.h"
#include "src/exceptions/WrongFormatException.h"
#include "src/parser/MappedFile.h"
#include "src/utility/cstring.h"


namespace storm {

namespace parser {

using namespace storm::utility::cstring;

MarkovAutomatonSparseTransitionParser::FirstPassResult MarkovAutomatonSparseTransitionParser::firstPass(char* buf) {
	MarkovAutomatonSparseTransitionParser::FirstPassResult result;

	bool fixDeadlocks = storm::settings::Settings::getInstance()->isSet("fixDeadlocks");

	// Skip the format hint if it is there.
	buf = trimWhitespaces(buf);
	if(buf[0] < '0' || buf[0] > '9') {
		buf = forwardToLineEnd(buf);
		buf = trimWhitespaces(buf);
	}

	// Now read the transitions.
	uint_fast64_t source, target = 0;
	uint_fast64_t lastsource = 0;
	bool encounteredEOF = false;
	bool stateHasMarkovianChoice = false;
	bool stateHasProbabilisticChoice = false;
	while (buf[0] != '\0' && !encounteredEOF) {
		// At the current point, the next thing to read is the source state of the next choice to come.
		source = checked_strtol(buf, &buf);

		// Check if we encountered a state index that is bigger than all previously seen ones and record it if necessary.
		if (source > result.highestStateIndex) {
			result.highestStateIndex = source;
		}

		// If we have skipped some states, we need to reserve the space for the self-loop insertion in the second pass.
		if (source > lastsource + 1) {
			if (fixDeadlocks) {
				result.numberOfNonzeroEntries += source - lastsource - 1;
				result.numberOfChoices += source - lastsource - 1;
			} else {
				LOG4CPLUS_ERROR(logger, "Found deadlock states (e.g. " << lastsource + 1 << ") during parsing. Please fix them or set the appropriate flag.");
				throw storm::exceptions::WrongFormatException() << "Found deadlock states (e.g. " << lastsource + 1 << ") during parsing. Please fix them or set the appropriate flag.";
			}
		} else if (source < lastsource) {
			LOG4CPLUS_ERROR(logger, "Illegal state choice order. A choice of state " << source << " appears at an illegal position.");
			throw storm::exceptions::WrongFormatException() << "Illegal state choice order. A choice of state " << source << " appears at an illegal position.";
		}

		++result.numberOfChoices;

		// If we have moved to the next state, we need to clear the flag that stores whether or not the source has a Markovian or probabilistic choice.
		if (source != lastsource) {
			stateHasMarkovianChoice = false;
			stateHasProbabilisticChoice = false;
		}

		// Record that the current source was the last source.
		lastsource = source;

		buf = trimWhitespaces(buf);

		// Depending on the action name, the choice is either a probabilitic one or a markovian one.
		bool isMarkovianChoice = false;
		if (buf[0] == '!' &&  skipWord(buf) - buf == 1) {
			isMarkovianChoice = true;
		}else {
			isMarkovianChoice = false;
		}
		buf = skipWord(buf);

		if (isMarkovianChoice) {
			if (stateHasMarkovianChoice) {
				LOG4CPLUS_ERROR(logger, "The state " << source << " has multiple Markovian choices.");
				throw storm::exceptions::WrongFormatException() << "The state " << source << " has multiple Markovian choices.";
			}
			if (stateHasProbabilisticChoice) {
				LOG4CPLUS_ERROR(logger, "The state " << source << " has a probabilistic choice preceding a Markovian choice. The Markovian choice must be the first choice listed.");
				throw storm::exceptions::WrongFormatException() << "The state " << source << " has a probabilistic choice preceding a Markovian choice. The Markovian choice must be the first choice listed.";
			}
			stateHasMarkovianChoice = true;
		} else {
			stateHasProbabilisticChoice = true;
		}

		// Go to the next line where the transitions start.
		buf = forwardToNextLine(buf);

		// Now that we have the source state and the information whether or not the current choice is probabilistic or Markovian, we need to read the list of successors and the probabilities/rates.
		bool hasSuccessorState = false;
		bool encounteredNewDistribution = false;
		uint_fast64_t lastSuccessorState = 0;

		// At this point, we need to check whether there is an additional successor or we have reached the next choice for the same or a different state.
		do {
			buf = trimWhitespaces(buf);
			// If the end of the file was reached, we need to abort and check whether we are in a legal state.
			if (buf[0] == '\0') {
				if (!hasSuccessorState) {
					LOG4CPLUS_ERROR(logger, "Premature end-of-file. Expected at least one successor state for state " << source << ".");
					throw storm::exceptions::WrongFormatException() << "Premature end-of-file. Expected at least one successor state for state " << source << ".";
				} else {
					// If there was at least one successor for the current choice, this is legal and we need to move on.
					encounteredEOF = true;
				}
			} else if (buf[0] == '*') {
				// As we have encountered a "*", we know that there is an additional successor state for the current choice.
				buf= skipWord(buf);

				// Now we need to read the successor state and check if we already saw a higher state index.
				target = checked_strtol(buf, &buf);
				if (target > result.highestStateIndex) {
					result.highestStateIndex = target;
				}
				if (hasSuccessorState && target <= lastSuccessorState) {
					LOG4CPLUS_ERROR(logger, "Illegal transition order for source state " << source << ".");
					throw storm::exceptions::WrongFormatException() << "Illegal transition order for source state " << source << ".";
				}

				// And the corresponding probability/rate.
				double val = checked_strtod(buf, &buf);
				if (val <= 0.0) {
					LOG4CPLUS_ERROR(logger, "Illegal probability/rate value for transition from " << source << " to " << target << ": " << val << ".");
					throw storm::exceptions::WrongFormatException() << "Illegal probability/rate value for transition from " << source << " to " << target << ": " << val << ".";
				}

				// We need to record that we found at least one successor state for the current choice.
				hasSuccessorState = true;
				lastSuccessorState = target;

				// As we found a new successor, we need to increase the number of nonzero entries.
				++result.numberOfNonzeroEntries;

				buf = forwardToNextLine(buf);
			} else {
				// If it was not a "*", we have to assume that we encountered the beginning of a new choice definition. In this case, we don't move the pointer
				// to the buffer, because we still need to read the new source state.
				encounteredNewDistribution = true;
			}
		} while (!encounteredEOF && !encounteredNewDistribution);
	}

	return result;
}

MarkovAutomatonSparseTransitionParser::ResultType MarkovAutomatonSparseTransitionParser::secondPass(char* buf, FirstPassResult const& firstPassResult) {
	ResultType result(firstPassResult);

	bool fixDeadlocks = storm::settings::Settings::getInstance()->isSet("fixDeadlocks");

	// Skip the format hint if it is there.
	buf = trimWhitespaces(buf);
	if(buf[0] < '0' || buf[0] > '9') {
		buf = forwardToLineEnd(buf);
		buf = trimWhitespaces(buf);
	}

	// Now read the transitions.
	uint_fast64_t source, target = 0;
	uint_fast64_t lastsource = 0;
	bool encounteredEOF = false;
	uint_fast64_t currentChoice = 0;
	while (buf[0] != '\0' && !encounteredEOF) {
		// At the current point, the next thing to read is the source state of the next choice to come.
		source = checked_strtol(buf, &buf);

		// If we have skipped some states, we need to insert self-loops if requested.
		if (source > lastsource + 1) {
			if (fixDeadlocks) {
				for (uint_fast64_t index = lastsource + 1; index < source; ++index) {
					result.nondeterministicChoiceIndices[index] = currentChoice;
					result.transitionMatrixBuilder.addNextValue(currentChoice, index, 1);
					++currentChoice;
				}
			} else {
				LOG4CPLUS_ERROR(logger, "Found deadlock states (e.g. " << lastsource + 1 << ") during parsing. Please fix them or set the appropriate flag.");
				throw storm::exceptions::WrongFormatException() << "Found deadlock states (e.g. " << lastsource + 1 << ") during parsing. Please fix them or set the appropriate flag.";
			}
		}

		if (source != lastsource) {
			// If we skipped to a new state we need to record the beginning of the choices in the nondeterministic choice indices.
			result.nondeterministicChoiceIndices[source] = currentChoice;
		}

		// Record that the current source was the last source.
		lastsource = source;

		buf = trimWhitespaces(buf);

		// Depending on the action name, the choice is either a probabilitic one or a markovian one.
		bool isMarkovianChoice = false;
		if (buf[0] == '!' && skipWord(buf) - buf == 1) {
			isMarkovianChoice = true;

			// Mark the current state as a Markovian one.
			result.markovianStates.set(source, true);
		} else {
			isMarkovianChoice = false;
		}

		// Go to the next line where the transitions start.
		buf = forwardToNextLine(buf);

		// Now that we have the source state and the information whether or not the current choice is probabilistic or Markovian, we need to read the list of successors and the probabilities/rates.
		bool encounteredNewDistribution = false;

		// At this point, we need to check whether there is an additional successor or we have reached the next choice for the same or a different state.
		do {
			buf = trimWhitespaces(buf);

			// If the end of the file was reached, we need to abort and check whether we are in a legal state.
			if (buf[0] == '\0') {
				// Under the assumption that the currently open choice has at least one successor (which is given after the first run)
				// we may legally stop reading here.
				encounteredEOF = true;
			} else if (buf[0] == '*') {

				// As we have encountered a "*", we know that there is an additional successor state for the current choice.
				buf = skipWord(buf);

				// Now we need to read the successor state and check if we already saw a higher state index.
				target = checked_strtol(buf, &buf);

				// And the corresponding probability/rate.
				double val = checked_strtod(buf, &buf);

				// Record the value as well as the exit rate in case of a Markovian choice.
				result.transitionMatrixBuilder.addNextValue(currentChoice, target, val);
				if (isMarkovianChoice) {
					result.exitRates[source] += val;
				}

				buf = forwardToNextLine(buf);
			} else {
				// If it was not a "*", we have to assume that we encountered the beginning of a new choice definition. In this case, we don't move the pointer
				// to the buffer, because we still need to read the new source state.
				encounteredNewDistribution = true;
			}
		} while (!encounteredEOF && !encounteredNewDistribution);

		++currentChoice;
	}

	// Put a sentinel element at the end.
	result.nondeterministicChoiceIndices[firstPassResult.highestStateIndex + 1] = currentChoice;

	return result;
}

MarkovAutomatonSparseTransitionParser::ResultType MarkovAutomatonSparseTransitionParser::parseMarkovAutomatonTransitions(std::string const& filename) {
	// Set the locale to correctly recognize floating point numbers.
	setlocale(LC_NUMERIC, "C");

	if (!MappedFile::fileExistsAndIsReadable(filename.c_str())) {
		LOG4CPLUS_ERROR(logger, "Error while parsing " << filename << ": File does not exist or is not readable.");
		throw storm::exceptions::WrongFormatException() << "Error while parsing " << filename << ": File does not exist or is not readable.";
	}

	// Open file and prepare pointer to buffer.
	MappedFile file(filename.c_str());
	char* buf = file.data;

	return secondPass(buf, firstPass(buf));
}

} // namespace parser
} // namespace storm
