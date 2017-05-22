#include "storm/parser/DeterministicModelParser.h"

#include <string>
#include <vector>

#include "storm/models/sparse/StandardRewardModel.h"

#include "storm/parser/DeterministicSparseTransitionParser.h"
#include "storm/parser/AtomicPropositionLabelingParser.h"
#include "storm/parser/SparseStateRewardParser.h"

#include "storm/adapters/CarlAdapter.h"

namespace storm {
    namespace parser {

        template<typename ValueType, typename RewardValueType>
        typename DeterministicModelParser<ValueType, RewardValueType>::Result DeterministicModelParser<ValueType, RewardValueType>::parseDeterministicModel(std::string const& transitionsFilename, std::string const& labelingFilename, std::string const& stateRewardFilename, std::string const& transitionRewardFilename) {

            // Parse the transitions.
            storm::storage::SparseMatrix<ValueType> transitions(std::move(storm::parser::DeterministicSparseTransitionParser<ValueType>::parseDeterministicTransitions(transitionsFilename)));

            uint_fast64_t stateCount = transitions.getColumnCount();

            // Parse the state labeling.
            storm::models::sparse::StateLabeling labeling(storm::parser::AtomicPropositionLabelingParser::parseAtomicPropositionLabeling(stateCount, labelingFilename));

            // Construct the result.
            DeterministicModelParser<ValueType, RewardValueType>::Result result(std::move(transitions), std::move(labeling));

            // Only parse state rewards if a file is given.
            if (stateRewardFilename != "") {
                result.stateRewards = storm::parser::SparseStateRewardParser<RewardValueType>::parseSparseStateReward(stateCount, stateRewardFilename);
            }

            // Only parse transition rewards if a file is given.
            if (transitionRewardFilename != "") {
                result.transitionRewards = storm::parser::DeterministicSparseTransitionParser<RewardValueType>::parseDeterministicTransitionRewards(transitionRewardFilename, result.transitionSystem);
            }

            return result;
        }

        template<typename ValueType, typename RewardValueType>
        storm::models::sparse::Dtmc<ValueType, storm::models::sparse::StandardRewardModel<RewardValueType>> DeterministicModelParser<ValueType, RewardValueType>::parseDtmc(std::string const & transitionsFilename, std::string const & labelingFilename, std::string const& stateRewardFilename, std::string const& transitionRewardFilename) {
            typename DeterministicModelParser<ValueType, RewardValueType>::Result parserResult(std::move(parseDeterministicModel(transitionsFilename, labelingFilename, stateRewardFilename, transitionRewardFilename)));

            std::unordered_map<std::string, storm::models::sparse::StandardRewardModel<RewardValueType>> rewardModels;
            if (!stateRewardFilename.empty() || !transitionRewardFilename.empty()) {
                rewardModels.insert(std::make_pair("", storm::models::sparse::StandardRewardModel<RewardValueType>(parserResult.stateRewards, boost::optional<std::vector<RewardValueType>>(), parserResult.transitionRewards)));
            }
            return storm::models::sparse::Dtmc<ValueType, storm::models::sparse::StandardRewardModel<RewardValueType>>(std::move(parserResult.transitionSystem), std::move(parserResult.labeling), std::move(rewardModels));
        }

        template<typename ValueType, typename RewardValueType>
        storm::models::sparse::Ctmc<ValueType, storm::models::sparse::StandardRewardModel<RewardValueType>> DeterministicModelParser<ValueType, RewardValueType>::parseCtmc(std::string const& transitionsFilename, std::string const& labelingFilename, std::string const& stateRewardFilename, std::string const& transitionRewardFilename) {
            typename DeterministicModelParser<ValueType, RewardValueType>::Result parserResult(std::move(parseDeterministicModel(transitionsFilename, labelingFilename, stateRewardFilename, transitionRewardFilename)));

            std::unordered_map<std::string, storm::models::sparse::StandardRewardModel<RewardValueType>> rewardModels;
            if (!stateRewardFilename.empty() || !transitionRewardFilename.empty()) {
                rewardModels.insert(std::make_pair("", storm::models::sparse::StandardRewardModel<RewardValueType>(parserResult.stateRewards, boost::optional<std::vector<RewardValueType>>(), parserResult.transitionRewards)));
            }
            return storm::models::sparse::Ctmc<ValueType, storm::models::sparse::StandardRewardModel<RewardValueType>>(std::move(parserResult.transitionSystem), std::move(parserResult.labeling), std::move(rewardModels), boost::none);
        }

        template class DeterministicModelParser<double, double>;

#ifdef STORM_HAVE_CARL
        template class DeterministicModelParser<double, storm::Interval>;
#endif
        
    } /* namespace parser */
} /* namespace storm */
