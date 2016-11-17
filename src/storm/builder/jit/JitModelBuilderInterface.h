#pragma once

#include <memory>

#include "storm/builder/jit/ModelComponentsBuilder.h"

namespace storm {
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
            class JitModelBuilderInterface {
            public:
                JitModelBuilderInterface(ModelComponentsBuilder<IndexType, ValueType>& modelComponentsBuilder);
                
                virtual ~JitModelBuilderInterface();
                
                virtual storm::models::sparse::Model<ValueType, storm::models::sparse::StandardRewardModel<ValueType>>* build() = 0;
                
                void addStateBehaviour(IndexType const& stateId, StateBehaviour<IndexType, ValueType>& behaviour);
                
            protected:
                ModelComponentsBuilder<IndexType, ValueType>& modelComponentsBuilder;
            };
            
            
        }
    }
}
