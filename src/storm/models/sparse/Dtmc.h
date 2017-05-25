#ifndef STORM_MODELS_SPARSE_DTMC_H_
#define STORM_MODELS_SPARSE_DTMC_H_

#include "storm/models/sparse/DeterministicModel.h"

namespace storm {
    namespace models {
        namespace sparse {
            
            /*!
             * This class represents a discrete-time Markov chain.
             */
            template<class ValueType, typename RewardModelType = StandardRewardModel<ValueType>>
            class Dtmc : public DeterministicModel<ValueType, RewardModelType> {
            public:
                /*!
                 * Constructs a model from the given data.
                 *
                 * @param transitionMatrix The matrix representing the transitions in the model.
                 * @param stateLabeling The labeling of the states.
                 * @param rewardModels A mapping of reward model names to reward models.
                 * @param optionalChoiceLabeling A vector that represents the labels associated with the choices of each state.
                 */
                Dtmc(storm::storage::SparseMatrix<ValueType> const& probabilityMatrix,
                     storm::models::sparse::StateLabeling const& stateLabeling,
                     std::unordered_map<std::string, RewardModelType> const& rewardModels = std::unordered_map<std::string, RewardModelType>(),
                     boost::optional<std::vector<LabelSet>> const& optionalChoiceLabeling = boost::optional<std::vector<LabelSet>>());
                
                /*!
                 * Constructs a model by moving the given data.
                 *
                 * @param transitionMatrix The matrix representing the transitions in the model.
                 * @param stateLabeling The labeling of the states.
                 * @param rewardModels A mapping of reward model names to reward models.
                 * @param optionalChoiceLabeling A vector that represents the labels associated with the choices of each state.
                 */
                Dtmc(storm::storage::SparseMatrix<ValueType>&& probabilityMatrix, storm::models::sparse::StateLabeling&& stateLabeling,
                     std::unordered_map<std::string, RewardModelType>&& rewardModels = std::unordered_map<std::string, RewardModelType>(),
                     boost::optional<std::vector<LabelSet>>&& optionalChoiceLabeling = boost::optional<std::vector<LabelSet>>());
                
                Dtmc(Dtmc<ValueType, RewardModelType> const& dtmc) = default;
                Dtmc& operator=(Dtmc<ValueType, RewardModelType> const& dtmc) = default;
                
                Dtmc(Dtmc<ValueType, RewardModelType>&& dtmc) = default;
                Dtmc& operator=(Dtmc<ValueType, RewardModelType>&& dtmc) = default;


            };
            
        } // namespace sparse
    } // namespace models
} // namespace storm

#endif /* STORM_MODELS_SPARSE_DTMC_H_ */
