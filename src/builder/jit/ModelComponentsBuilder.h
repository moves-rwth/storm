#pragma once

#include <memory>

#include "src/builder/jit/StateBehaviour.h"

#include "src/storage/jani/ModelType.h"

namespace storm {
    namespace storage {
        template <typename ValueType>
        class SparseMatrixBuilder;
        
        class BitVector;
    }
    
    namespace models {
        namespace sparse {
            template <typename ValueType>
            class StandardRewardModel;
            
            template <typename ValueType, typename RewardModelType>
            class Model;
            
            class StateLabeling;
        }
    }
    
    namespace builder {
        namespace jit {
            
            template <typename IndexType, typename ValueType>
            class ModelComponentsBuilder {
            public:
                ModelComponentsBuilder(storm::jani::ModelType const& modelType);
                ~ModelComponentsBuilder();
                
                void addStateBehaviour(IndexType const& stateIndex, StateBehaviour<IndexType, ValueType>& behaviour);

                storm::models::sparse::Model<ValueType, storm::models::sparse::StandardRewardModel<ValueType>>* build(IndexType const& stateCount);
                
                void registerLabel(std::string const& name, IndexType const& stateCount);
                void addLabel(IndexType const& stateId, IndexType const& labelIndex);
                
            private:
                storm::jani::ModelType modelType;
                bool isDeterministicModel;
                bool isDiscreteTimeModel;
                bool dontFixDeadlocks;
                
                IndexType currentRowGroup;
                IndexType currentRow;
                std::unique_ptr<storm::storage::SparseMatrixBuilder<ValueType>> transitionMatrixBuilder;
                std::vector<std::pair<std::string, storm::storage::BitVector>> labels;
            };
            
        }
    }
}
