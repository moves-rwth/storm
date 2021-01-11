#ifndef STORM_MODELS_SPARSE_SMG_H_
#define STORM_MODELS_SPARSE_SMG_H_

#include "storm/models/sparse/NondeterministicModel.h"

namespace storm {
    namespace models {
        namespace sparse {

            /*!
             * This class represents a stochastic multiplayer game.
             */
            template<class ValueType, typename RewardModelType = StandardRewardModel<ValueType>>
            class Smg : public NondeterministicModel<ValueType, RewardModelType> {
            public:
                /*!
                 * Constructs a model from the given data.
                 *
                 * @param transitionMatrix The matrix representing the transitions in the model.
                 * @param stateLabeling The labeling of the states.
                 * @param rewardModels A mapping of reward model names to reward models.
                 */
                Smg(storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                    storm::models::sparse::StateLabeling const& stateLabeling,
                    std::unordered_map<std::string, RewardModelType> const& rewardModels = std::unordered_map<std::string, RewardModelType>(), ModelType type = ModelType::Smg);

                /*!
                 * Constructs a model by moving the given data.
                 *
                 * @param transitionMatrix The matrix representing the transitions in the model.
                 * @param stateLabeling The labeling of the states.
                 * @param rewardModels A mapping of reward model names to reward models.
                 */
                Smg(storm::storage::SparseMatrix<ValueType>&& transitionMatrix,
                    storm::models::sparse::StateLabeling&& stateLabeling,
                    std::unordered_map<std::string, RewardModelType>&& rewardModels = std::unordered_map<std::string, RewardModelType>(), ModelType type = ModelType::Smg);

                /*!
                 * Constructs a model from the given data.
                 *
                 * @param components The components for this model.
                 */
                Smg(storm::storage::sparse::ModelComponents<ValueType, RewardModelType> const& components, ModelType type = ModelType::Smg);
                Smg(storm::storage::sparse::ModelComponents<ValueType, RewardModelType>&& components, ModelType type = ModelType::Smg);

                Smg(Smg<ValueType, RewardModelType> const& other) = default;
                Smg& operator=(Smg<ValueType, RewardModelType> const& other) = default;

                Smg(Smg<ValueType, RewardModelType>&& other) = default;
                Smg& operator=(Smg<ValueType, RewardModelType>&& other) = default;
            };

        } // namespace sparse
    } // namespace models
} // namespace storm

#endif /* STORM_MODELS_SPARSE_SMG_H_ */
