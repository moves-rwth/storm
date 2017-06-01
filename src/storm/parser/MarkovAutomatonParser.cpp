#include "MarkovAutomatonParser.h"
#include "AtomicPropositionLabelingParser.h"
#include "SparseStateRewardParser.h"

#include "storm/models/sparse/StandardRewardModel.h"

#include "storm/exceptions/WrongFormatException.h"

#include "storm/adapters/RationalFunctionAdapter.h"

namespace storm {
    namespace parser {

        template<typename ValueType, typename RewardValueType>
        storm::models::sparse::MarkovAutomaton<ValueType, storm::models::sparse::StandardRewardModel<RewardValueType>> MarkovAutomatonParser<ValueType, RewardValueType>::parseMarkovAutomaton(std::string const& transitionsFilename, std::string const& labelingFilename, std::string const& stateRewardFilename, std::string const& transitionRewardFilename) {

            // Parse the transitions of the Markov Automaton.
            typename storm::parser::MarkovAutomatonSparseTransitionParser<ValueType>::Result transitionResult(storm::parser::MarkovAutomatonSparseTransitionParser<ValueType>::parseMarkovAutomatonTransitions(transitionsFilename));

            // Build the actual transition matrix using the MatrixBuilder provided by the transitionResult.
            storm::storage::SparseMatrix<ValueType> transitionMatrix(transitionResult.transitionMatrixBuilder.build());

            // Parse the state labeling.
            storm::models::sparse::StateLabeling resultLabeling(storm::parser::AtomicPropositionLabelingParser::parseAtomicPropositionLabeling(transitionMatrix.getColumnCount(), labelingFilename));

            // If given, parse the state rewards file.
            boost::optional<std::vector<RewardValueType>> stateRewards;
            if (stateRewardFilename != "") {
                stateRewards.reset(storm::parser::SparseStateRewardParser<RewardValueType>::parseSparseStateReward(transitionMatrix.getColumnCount(), stateRewardFilename));
            }
            std::unordered_map<std::string, storm::models::sparse::StandardRewardModel<RewardValueType>> rewardModels;
            rewardModels.insert(std::make_pair("", storm::models::sparse::StandardRewardModel<RewardValueType>(stateRewards, boost::optional<std::vector<RewardValueType>>(), boost::optional<storm::storage::SparseMatrix<RewardValueType>>())));

            // Since Markov Automata do not support transition rewards no path should be given here.
            if (transitionRewardFilename != "") {
                STORM_LOG_ERROR("Transition rewards are unsupported for Markov automata.");
                throw storm::exceptions::WrongFormatException() << "Transition rewards are unsupported for Markov automata.";
            }

            // Put the pieces together to generate the Markov Automaton.
            storm::models::sparse::MarkovAutomaton<ValueType, storm::models::sparse::StandardRewardModel<RewardValueType>> resultingAutomaton(std::move(transitionMatrix), std::move(resultLabeling), std::move(transitionResult.markovianStates), std::move(transitionResult.exitRates), rewardModels);

            return resultingAutomaton;
        }

        template class MarkovAutomatonParser<double, double>;

#ifdef STORM_HAVE_CARL
        template class MarkovAutomatonParser<double, storm::Interval>;
#endif

    } // namespace parser
} // namespace storm
