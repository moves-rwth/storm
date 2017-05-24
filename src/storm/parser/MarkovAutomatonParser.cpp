#include "MarkovAutomatonParser.h"
#include "SparseItemLabelingParser.h"
#include "SparseStateRewardParser.h"
#include "NondeterministicSparseTransitionParser.h"

#include "storm/models/sparse/StandardRewardModel.h"

#include "storm/exceptions/WrongFormatException.h"

#include "storm/adapters/CarlAdapter.h"

namespace storm {
    namespace parser {

        template<typename ValueType, typename RewardValueType>
        storm::models::sparse::MarkovAutomaton<ValueType, storm::models::sparse::StandardRewardModel<RewardValueType>> MarkovAutomatonParser<ValueType, RewardValueType>::parseMarkovAutomaton(std::string const& transitionsFilename, std::string const& labelingFilename, std::string const& stateRewardFilename, std::string const& transitionRewardFilename, std::string const& choiceLabelingFilename) {

            // Parse the transitions of the Markov Automaton.
            typename storm::parser::MarkovAutomatonSparseTransitionParser<ValueType>::Result transitionResult(storm::parser::MarkovAutomatonSparseTransitionParser<ValueType>::parseMarkovAutomatonTransitions(transitionsFilename));

            // Build the actual transition matrix using the MatrixBuilder provided by the transitionResult.
            storm::storage::SparseMatrix<ValueType> transitionMatrix(transitionResult.transitionMatrixBuilder.build());

            // Parse the state labeling.
            storm::models::sparse::StateLabeling resultLabeling(storm::parser::SparseItemLabelingParser::parseAtomicPropositionLabeling(transitionMatrix.getColumnCount(), labelingFilename));

            // If given, parse the state rewards file.
            boost::optional<std::vector<RewardValueType>> stateRewards;
            if (!stateRewardFilename.empty()) {
                stateRewards.reset(storm::parser::SparseStateRewardParser<RewardValueType>::parseSparseStateReward(transitionMatrix.getColumnCount(), stateRewardFilename));
            }

           // Only parse transition rewards if a file is given.
            boost::optional<storm::storage::SparseMatrix<RewardValueType>> transitionRewards;
            if (!transitionRewardFilename.empty()) {
                transitionRewards = std::move(storm::parser::NondeterministicSparseTransitionParser<RewardValueType>::parseNondeterministicTransitionRewards(transitionRewardFilename, transitionMatrix));
            }
            
            std::unordered_map<std::string, storm::models::sparse::StandardRewardModel<RewardValueType>> rewardModels;
            rewardModels.insert(std::make_pair("", storm::models::sparse::StandardRewardModel<RewardValueType>(std::move(stateRewards), boost::none, std::move(transitionRewards))));

            // Only parse choice labeling if a file is given.
            boost::optional<storm::models::sparse::ChoiceLabeling> choiceLabeling;
            if (!choiceLabelingFilename.empty()) {
                choiceLabeling = storm::parser::SparseItemLabelingParser::parseChoiceLabeling(transitionMatrix.getRowCount(), choiceLabelingFilename, transitionMatrix.getRowGroupIndices());
            }

            // Put the pieces together to generate the Markov Automaton.
            storm::models::sparse::MarkovAutomaton<ValueType, storm::models::sparse::StandardRewardModel<RewardValueType>> resultingAutomaton(std::move(transitionMatrix), std::move(resultLabeling), std::move(transitionResult.markovianStates), std::move(transitionResult.exitRates), std::move(rewardModels), std::move(choiceLabeling));

            return resultingAutomaton;
        }

        template class MarkovAutomatonParser<double, double>;

#ifdef STORM_HAVE_CARL
        template class MarkovAutomatonParser<double, storm::Interval>;
#endif

    } // namespace parser
} // namespace storm
