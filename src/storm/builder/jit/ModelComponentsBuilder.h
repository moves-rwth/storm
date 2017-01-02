#pragma once

#include <memory>

#include "storm/builder/jit/StateBehaviour.h"

#include "storm/storage/jani/ModelType.h"

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
        class RewardModelInformation;
        
        template<typename ValueType>
        class RewardModelBuilder;
        
        namespace jit {
            
            template <typename IndexType, typename ValueType>
            class ModelComponentsBuilder {
            public:
                ModelComponentsBuilder(storm::jani::ModelType const& modelType);
                ~ModelComponentsBuilder();
                
                void addStateBehaviour(IndexType const& stateIndex, StateBehaviour<IndexType, ValueType>& behaviour);

                storm::models::sparse::Model<ValueType, storm::models::sparse::StandardRewardModel<ValueType>>* build(IndexType const& stateCount);
                
                void registerRewardModel(RewardModelInformation const& rewardModelInformation);
                
                void registerLabel(std::string const& name, IndexType const& stateCount);
                void addLabel(IndexType const& stateId, IndexType const& labelIndex);
                
            private:
                storm::jani::ModelType modelType;
                bool isDeterministicModel;
                bool isDiscreteTimeModel;
                bool dontFixDeadlocks;
                
                IndexType currentRowGroup;
                IndexType currentRow;
                std::unique_ptr<storm::storage::BitVector> markovianStates;
                std::unique_ptr<storm::storage::SparseMatrixBuilder<ValueType>> transitionMatrixBuilder;
                std::vector<storm::builder::RewardModelBuilder<ValueType>> rewardModelBuilders;
                std::vector<std::pair<std::string, storm::storage::BitVector>> labels;
            };
            
        }
    }
}
