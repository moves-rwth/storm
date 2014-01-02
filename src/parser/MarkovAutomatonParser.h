#ifndef STORM_PARSER_MARKOVAUTOMATONPARSER_H_
#define STORM_PARSER_MARKOVAUTOMATONPARSER_H_

#include "src/models/MarkovAutomaton.h"
#include "src/parser/MarkovAutomatonSparseTransitionParser.h"

namespace storm {

namespace parser {

/*!
 * A class providing the functionality to parse a labeled Markov automaton.
 */
class MarkovAutomatonParser {
public:

	/*!
	 * Parses the given Markov automaton and returns an object representing the automaton.
	 *
	 * @param transitionsFilename The name of the file containing the transitions of the Markov automaton.
	 * @param labelingFilename The name of the file containing the labels for the states of the Markov automaton.
	 * @param stateRewardFilename The name of the file that contains the state reward of the Markov automaton.
	 * @param transitionRewardFilename The name of the file that contains the transition rewards of the Markov automaton. This should be empty as transition rewards are not supported by Markov Automata.
	 */
	static storm::models::MarkovAutomaton<double> parseMarkovAutomaton(std::string const& transitionsFilename, std::string const& labelingFilename, std::string const& stateRewardFilename, std::string const& transitionRewardFilename);
};

} // namespace parser

} // namespace storm

#endif /* STORM_PARSER_MARKOVAUTOMATONPARSER_H_ */
