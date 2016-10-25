#pragma once

#include <memory>

#include "src/builder/jit/StateBehaviour.h"

#include "src/storage/jani/ModelType.h"

namespace storm {
    namespace storage {
        template <typename ValueType>
        class SparseMatrixBuilder;
    }
    
    namespace models {
        namespace sparse {
            template <typename ValueType>
            class StandardRewardModel;
            
            template <typename ValueType, typename RewardModelType>
            class Model;
        }
    }
    
    namespace builder {
        namespace jit {
            
            template <typename IndexType, typename ValueType>
            class ModelComponentsBuilder {
            public:
                ModelComponentsBuilder(storm::jani::ModelType const& modelType);
                
                void addStateBehaviour(StateBehaviour<IndexType, ValueType> const& behaviour);

                storm::models::sparse::Model<ValueType, storm::models::sparse::StandardRewardModel<ValueType>>* build();
                
            private:
                storm::jani::ModelType modelType;
                bool isDeterministicModel;
                bool isDiscreteTimeModel;
                std::unique_ptr<storm::storage::SparseMatrixBuilder<ValueType>> transitionMatrixBuilder;
            };
            
        }
    }
}
