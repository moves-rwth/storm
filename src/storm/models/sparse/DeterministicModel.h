#ifndef STORM_MODELS_SPARSE_DETERMINISTICMODEL_H_
#define STORM_MODELS_SPARSE_DETERMINISTICMODEL_H_

#include "storm/models/sparse/Model.h"
#include "storm/utility/OsDetection.h"

namespace storm {
    namespace models {
        namespace sparse {
            
            /*!
             * The base class of all sparse deterministic models.
             */
            template<class ValueType, typename RewardModelType = StandardRewardModel<ValueType>>
            class DeterministicModel: public Model<ValueType, RewardModelType> {
            public:
                /*!
                 * Constructs a model from the given data.
                 *
                 * @param modelType The type of the model.
                 * @param transitionMatrix The matrix representing the transitions in the model.
                 * @param stateLabeling The labeling of the states.
                 * @param rewardModels A mapping of reward model names to reward models.
                 * @param optionalChoiceLabeling A vector that represents the labels associated with the choices of each state.
                 */
                DeterministicModel(storm::models::ModelType const& modelType,
                                   storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                   storm::models::sparse::StateLabeling const& stateLabeling,
                                   std::unordered_map<std::string, RewardModelType> const& rewardModels = std::unordered_map<std::string, RewardModelType>(),
                                   boost::optional<storm::models::sparse::ChoiceLabeling> const& optionalChoiceLabeling = boost::none);
                
                /*!
                 * Constructs a model by moving the given data.
                 *
                 * @param modelType The type of the model.
                 * @param transitionMatrix The matrix representing the transitions in the model.
                 * @param stateLabeling The labeling of the states.
                 * @param rewardModels A mapping of reward model names to reward models.
                 * @param optionalChoiceLabeling A vector that represents the labels associated with the choices of each state.
                 */
                DeterministicModel(storm::models::ModelType const& modelType,
                                   storm::storage::SparseMatrix<ValueType>&& transitionMatrix,
                                   storm::models::sparse::StateLabeling&& stateLabeling,
                                   std::unordered_map<std::string, RewardModelType>&& rewardModels = std::unordered_map<std::string, RewardModelType>(),
                                   boost::optional<storm::models::sparse::ChoiceLabeling>&& optionalChoiceLabeling = boost::none);
                
                DeterministicModel(DeterministicModel<ValueType, RewardModelType> const& other) = default;
                DeterministicModel& operator=(DeterministicModel<ValueType, RewardModelType> const& other) = default;
                

                DeterministicModel(DeterministicModel<ValueType, RewardModelType>&& other) = default;
                DeterministicModel<ValueType, RewardModelType>& operator=(DeterministicModel<ValueType, RewardModelType>&& model) = default;

                virtual void reduceToStateBasedRewards() override;
                
                virtual void writeDotToStream(std::ostream& outStream, bool includeLabeling = true, storm::storage::BitVector const* subsystem = nullptr, std::vector<ValueType> const* firstValue = nullptr, std::vector<ValueType> const* secondValue = nullptr, std::vector<uint_fast64_t> const* stateColoring = nullptr, std::vector<std::string> const* colors = nullptr, std::vector<uint_fast64_t>* scheduler = nullptr, bool finalizeOutput = true) const override;
            };
            
        } // namespace sparse
    } // namespace models
} // namespace storm

#endif /* STORM_MODELS_SPARSE_DETERMINISTICMODEL_H_ */
