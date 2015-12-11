#include "src/parser/NondeterministicModelParser.h"

#include <string>
#include <vector>

#include "src/models/sparse/StandardRewardModel.h"

#include "src/parser/NondeterministicSparseTransitionParser.h"
#include "src/parser/AtomicPropositionLabelingParser.h"
#include "src/parser/SparseStateRewardParser.h"
#include "src/parser/SparseChoiceLabelingParser.h"

#include "src/adapters/CarlAdapter.h"

namespace storm {
    namespace parser {

        template<typename ValueType, typename RewardValueType>
        typename NondeterministicModelParser<ValueType, RewardValueType>::Result NondeterministicModelParser<ValueType, RewardValueType>::parseNondeterministicModel(std::string const& transitionsFilename, std::string const& labelingFilename, std::string const& stateRewardFilename, std::string const& transitionRewardFilename, std::string const& choiceLabelingFilename) {

            // Parse the transitions.
            storm::storage::SparseMatrix<ValueType> transitions(std::move(storm::parser::NondeterministicSparseTransitionParser<ValueType>::parseNondeterministicTransitions(transitionsFilename)));

            uint_fast64_t stateCount = transitions.getColumnCount();

            // Parse the state labeling.
            storm::models::sparse::StateLabeling labeling(std::move(storm::parser::AtomicPropositionLabelingParser::parseAtomicPropositionLabeling(stateCount, labelingFilename)));

            // Only parse state rewards if a file is given.
            boost::optional<std::vector<RewardValueType>> stateRewards;
            if (!stateRewardFilename.empty()) {
                stateRewards = std::move(storm::parser::SparseStateRewardParser<RewardValueType>::parseSparseStateReward(stateCount, stateRewardFilename));
            }

            // Only parse transition rewards if a file is given.
            boost::optional<storm::storage::SparseMatrix<RewardValueType>> transitionRewards;
            if (!transitionRewardFilename.empty()) {
                transitionRewards = std::move(storm::parser::NondeterministicSparseTransitionParser<RewardValueType>::parseNondeterministicTransitionRewards(transitionRewardFilename, transitions));
            }

            // Only parse choice labeling if a file is given.
            boost::optional<std::vector < storm::models::sparse::LabelSet>> choiceLabeling;
            if (!choiceLabelingFilename.empty()) {
                choiceLabeling = std::move(storm::parser::SparseChoiceLabelingParser::parseChoiceLabeling(transitions.getRowGroupIndices(), choiceLabelingFilename));
            }

            // Construct the result.
            Result result(std::move(transitions), std::move(labeling));
            result.stateRewards = std::move(stateRewards);
            result.transitionRewards = std::move(transitionRewards);
            result.choiceLabeling = std::move(choiceLabeling);

            return result;
        }

        template<typename ValueType, typename RewardValueType>
        storm::models::sparse::Mdp<ValueType, storm::models::sparse::StandardRewardModel<RewardValueType>> NondeterministicModelParser<ValueType, RewardValueType>::parseMdp(std::string const& transitionsFilename, std::string const& labelingFilename, std::string const& stateRewardFilename, std::string const& transitionRewardFilename, std::string const& choiceLabelingFilename) {
            Result parserResult = parseNondeterministicModel(transitionsFilename, labelingFilename, stateRewardFilename, transitionRewardFilename, choiceLabelingFilename);

            std::unordered_map<std::string, storm::models::sparse::StandardRewardModel<RewardValueType>> rewardModels;
            rewardModels.insert(std::make_pair("", storm::models::sparse::StandardRewardModel<RewardValueType>(parserResult.stateRewards, boost::optional<std::vector<RewardValueType>>(), parserResult.transitionRewards)));
            return storm::models::sparse::Mdp<ValueType, storm::models::sparse::StandardRewardModel<RewardValueType>>(std::move(parserResult.transitionSystem), std::move(parserResult.labeling), std::move(rewardModels), std::move(parserResult.choiceLabeling));
        }

        template class NondeterministicModelParser<double, double>;

#ifdef STORM_HAVE_CARL
        template class NondeterministicModelParser<double, storm::Interval>;
#endif

    } /* namespace parser */
} /* namespace storm */
