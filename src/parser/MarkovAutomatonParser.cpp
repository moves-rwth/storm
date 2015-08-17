#include "MarkovAutomatonParser.h"
#include "AtomicPropositionLabelingParser.h"
#include "SparseStateRewardParser.h"
#include "src/exceptions/WrongFormatException.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
extern log4cplus::Logger logger;

namespace storm {
	namespace parser {

        storm::models::sparse::MarkovAutomaton<double> MarkovAutomatonParser::parseMarkovAutomaton(std::string const& transitionsFilename, std::string const& labelingFilename, std::string const& stateRewardFilename, std::string const& transitionRewardFilename) {

			// Parse the transitions of the Markov Automaton.
			storm::parser::MarkovAutomatonSparseTransitionParser::Result transitionResult(storm::parser::MarkovAutomatonSparseTransitionParser::parseMarkovAutomatonTransitions(transitionsFilename));

			// Build the actual transition matrix using the MatrixBuilder provided by the transitionResult.
			storm::storage::SparseMatrix<double> transitionMatrix(transitionResult.transitionMatrixBuilder.build());

			// Parse the state labeling.
			storm::models::sparse::StateLabeling resultLabeling(storm::parser::AtomicPropositionLabelingParser::parseAtomicPropositionLabeling(transitionMatrix.getColumnCount(), labelingFilename));

			// If given, parse the state rewards file.
            boost::optional<std::vector<double>> stateRewards;
			if (stateRewardFilename != "") {
				stateRewards.reset(storm::parser::SparseStateRewardParser::parseSparseStateReward(transitionMatrix.getColumnCount(), stateRewardFilename));
			}
            std::map<std::string, storm::models::sparse::StandardRewardModel<double>> rewardModels;
            rewardModels.insert(std::make_pair("", storm::models::sparse::StandardRewardModel<double>(stateRewards, boost::optional<std::vector<double>>(), boost::optional<storm::storage::SparseMatrix<double>>())));

			// Since Markov Automata do not support transition rewards no path should be given here.
			if (transitionRewardFilename != "") {
				LOG4CPLUS_ERROR(logger, "Transition rewards are unsupported for Markov automata.");
				throw storm::exceptions::WrongFormatException() << "Transition rewards are unsupported for Markov automata.";
			}

			// Put the pieces together to generate the Markov Automaton.
			storm::models::sparse::MarkovAutomaton<double> resultingAutomaton(std::move(transitionMatrix), std::move(resultLabeling), std::move(transitionResult.markovianStates), std::move(transitionResult.exitRates), rewardModels, boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>>());

			return resultingAutomaton;
		}

	} // namespace parser
} // namespace storm
