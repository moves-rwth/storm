#ifndef STORM_MODELS_SPARSE_DETERMINISTICMODEL_H_
#define STORM_MODELS_SPARSE_DETERMINISTICMODEL_H_

#include "src/models/sparse/Model.h"
#include "src/utility/OsDetection.h"

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
                                   std::unordered_map<std::string, RewardModelType> const& rewardModels = std::map<std::string, RewardModelType>(),
                                   boost::optional<std::vector<LabelSet>> const& optionalChoiceLabeling = boost::optional<std::vector<LabelSet>>());
                
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
                                   std::unordered_map<std::string, RewardModelType>&& rewardModels = std::map<std::string, RewardModelType>(),
                                   boost::optional<std::vector<LabelSet>>&& optionalChoiceLabeling = boost::optional<std::vector<LabelSet>>());
                
                DeterministicModel(DeterministicModel const& other) = default;
                DeterministicModel& operator=(DeterministicModel const& other) = default;
                
#ifndef WINDOWS
                DeterministicModel(DeterministicModel&& other) = default;
                DeterministicModel<ValueType>& operator=(DeterministicModel<ValueType>&& model) = default;
#endif
                
                virtual void reduceToStateBasedRewards() override;
                
                virtual void writeDotToStream(std::ostream& outStream, bool includeLabeling = true, storm::storage::BitVector const* subsystem = nullptr, std::vector<ValueType> const* firstValue = nullptr, std::vector<ValueType> const* secondValue = nullptr, std::vector<uint_fast64_t> const* stateColoring = nullptr, std::vector<std::string> const* colors = nullptr, std::vector<uint_fast64_t>* scheduler = nullptr, bool finalizeOutput = true) const;
            };
            
        } // namespace sparse
    } // namespace models
} // namespace storm

#endif /* STORM_MODELS_SPARSE_DETERMINISTICMODEL_H_ */
