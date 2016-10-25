#include "src/builder/jit/ModelComponentBuilder.h"

#include "src/storage/SparseMatrix.h"

namespace storm {
    namespace builder {
        namespace jit {
            
            template <typename IndexType, typename ValueType>
            ModelComponentsBuilder<IndexType, ValueType>::ModelComponentsBuilder(storm::jani::ModelType const& modelType) : modelType(modelType), isDeterministicModel(storm::jani::isDeterministicModel(modelType)), isDiscreteTimeModel(storm::jani::isDiscreteTimeModel(modelType)), transitionMatrixBuilder(std::make_unique<storm::storage::SparseMatrixBuilder<ValueType>>(0, 0, 0, true, !isDeterministicModel)) {
                // Intentionally left empty.
            }
            
            template <typename IndexType, typename ValueType>
            void ModelComponentsBuilder<IndexType, ValueType>::addStateBehaviour(StateBehaviour<IndexType, ValueType> const& behaviour) {
                // If there is more than one choice in a deterministic model, we need to combine the choices into one
                // global choice.
                bool choiceIsGuaranteedDistribution = false;
                if (model_is_deterministic() && choices.size() > 1) {
                    uint64_t choiceCount = choices.size();
                    
                    // We do this by adding the entries of choices after the first one to the first one and then making
                    // the distribution unique again.
                    for (auto it = ++choices.begin(), ite = choices.end(); it != ite; ++it) {
                        choices.front().add(std::move(*it));
                    }
                    choices.resize(1);
                    choices.front().makeUniqueAndShrink();
                    
                    // If we are dealing with a discrete-time model, we need to scale the probabilities such that they
                    // form a probability distribution.
                    if (model_is_discrete_time()) {
                        for (auto& element : choices.front().getDistribution()) {
                            element.scaleValue(choiceCount);
                        }
                    }
                    choiceIsGuaranteedDistribution = true;
                }
                
                for (auto& choice : choices) {
                    if (!choiceIsGuaranteedDistribution) {
                        // Create a proper distribution without duplicate entries.
                        choice.makeUniqueAndShrink();
                    }
                    
                    // Add the elements to the transition matrix.
                    for (auto const& element : choice.getDistribution()) {
                        modelComponents.transitionMatrixBuilder.addNextValue(currentRow, element.getColumn(), element.getValue());
                    }
                    
                    // Proceed to next row.
                    ++currentRow;
                }
            }
            
            template <typename IndexType, typename ValueType>
            storm::models::sparse::Model<ValueType, storm::models::sparse::StandardRewardModel<ValueType>>* ModelComponentsBuilder<IndexType, ValueType>::build() {
                // FIXME
                return nullptr;
            }
            
        }
    }
}
