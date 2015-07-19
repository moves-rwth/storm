#ifndef STORM_MODELS_SPARSE_NONDETERMINISTICMODEL_H_
#define STORM_MODELS_SPARSE_NONDETERMINISTICMODEL_H_

#include "src/models/sparse/Model.h"
#include "src/utility/OsDetection.h"

namespace storm {
    namespace models {
        namespace sparse {
            
            /*!
             * The base class of sparse nondeterministic models.
             */
            template<class ValueType>
            class NondeterministicModel: public Model<ValueType> {
            public:
                /*!
                 * Constructs a model from the given data.
                 *
                 * @param modelType The type of the model.
                 * @param transitionMatrix The matrix representing the transitions in the model.
                 * @param stateLabeling The labeling of the states.
                 * @param optionalStateRewardVector The reward values associated with the states.
                 * @param optionalTransitionRewardMatrix The reward values associated with the transitions of the model.
                 * @param optionalChoiceLabeling A vector that represents the labels associated with the choices of each state.
                 */
                NondeterministicModel(storm::models::ModelType const& modelType,
                                      storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                      storm::models::sparse::StateLabeling const& stateLabeling,
                                      boost::optional<std::vector<ValueType>> const& optionalStateRewardVector = boost::optional<std::vector<ValueType>>(),
                                      boost::optional<storm::storage::SparseMatrix<ValueType>> const& optionalTransitionRewardMatrix = boost::optional<storm::storage::SparseMatrix<ValueType>>(),
                                      boost::optional<std::vector<LabelSet>> const& optionalChoiceLabeling = boost::optional<std::vector<LabelSet>>());
                
                /*!
                 * Constructs a model by moving the given data.
                 *
                 * @param modelType The type of the model.
                 * @param transitionMatrix The matrix representing the transitions in the model.
                 * @param stateLabeling The labeling of the states.
                 * @param optionalStateRewardVector The reward values associated with the states.
                 * @param optionalTransitionRewardMatrix The reward values associated with the transitions of the model.
                 * @param optionalChoiceLabeling A vector that represents the labels associated with the choices of each state.
                 */
                NondeterministicModel(storm::models::ModelType const& modelType,
                                      storm::storage::SparseMatrix<ValueType>&& transitionMatrix,
                                      storm::models::sparse::StateLabeling&& stateLabeling,
                                      boost::optional<std::vector<ValueType>>&& optionalStateRewardVector = boost::optional<std::vector<ValueType>>(),
                                      boost::optional<storm::storage::SparseMatrix<ValueType>>&& optionalTransitionRewardMatrix = boost::optional<storm::storage::SparseMatrix<ValueType>>(),
                                      boost::optional<std::vector<LabelSet>>&& optionalChoiceLabeling = boost::optional<std::vector<LabelSet>>());
                
                NondeterministicModel(NondeterministicModel const& other) = default;
                NondeterministicModel& operator=(NondeterministicModel const& other) = default;
                
#ifndef WINDOWS
                NondeterministicModel(NondeterministicModel&& other) = default;
                NondeterministicModel& operator=(NondeterministicModel&& other) = default;
#endif
                
                /*!
                 * Retrieves the number of (nondeterministic) choices in the model.
                 *
                 * @return The number of (nondeterministic) choices in the model.
                 */
                uint_fast64_t getNumberOfChoices() const;
                
                /*!
                 * Retrieves the vector indicating which matrix rows represent non-deterministic choices of a certain state.
                 *
                 * @param The vector indicating which matrix rows represent non-deterministic choices of a certain state.
                 */
                std::vector<uint_fast64_t> const& getNondeterministicChoiceIndices() const;
                
                virtual void printModelInformationToStream(std::ostream& out) const;
                
                virtual void writeDotToStream(std::ostream& outStream, bool includeLabeling = true, storm::storage::BitVector const* subsystem = nullptr, std::vector<ValueType> const* firstValue = nullptr, std::vector<ValueType> const* secondValue = nullptr, std::vector<uint_fast64_t> const* stateColoring = nullptr, std::vector<std::string> const* colors = nullptr, std::vector<uint_fast64_t>* scheduler = nullptr, bool finalizeOutput = true) const;
            };
            
        } // namespace sparse
    } // namespace models
} // namespace storm

#endif /* STORM_MODELS_SPARSE_NONDETERMINISTICMODEL_H_ */
