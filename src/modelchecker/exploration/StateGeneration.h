#ifndef STORM_MODELCHECKER_EXPLORATION_EXPLORATION_DETAIL_STATEGENERATION_H_
#define STORM_MODELCHECKER_EXPLORATION_EXPLORATION_DETAIL_STATEGENERATION_H_

#include "src/generator/CompressedState.h"
#include "src/generator/PrismNextStateGenerator.h"

namespace storm {
    namespace generator {
        template<typename ValueType, typename StateType>
        class PrismNextStateGenerator;
    }
    
    namespace modelchecker {
        namespace exploration_detail {
            
            template <typename StateType, typename ValueType>
            class StateGeneration {
            public:
                StateGeneration(storm::prism::Program const& program, storm::generator::VariableInformation const& variableInformation, storm::expressions::Expression const& conditionStateExpression, storm::expressions::Expression const& targetStateExpression);
                
                void setStateToIdCallback(std::function<StateType (storm::generator::CompressedState const&)> const& stateToIdCallback);
                
                void load(storm::generator::CompressedState const& state);
                
                std::vector<StateType> getInitialStates();
                
                storm::generator::StateBehavior<ValueType, StateType> expand();
                
                bool isConditionState() const;
                
                bool isTargetState() const;
                
            private:
                storm::generator::PrismNextStateGenerator<ValueType, StateType> generator;
                std::function<StateType (storm::generator::CompressedState const&)> stateToIdCallback;
                storm::expressions::Expression conditionStateExpression;
                storm::expressions::Expression targetStateExpression;
            };
            
        }
    }
}

#endif /* STORM_MODELCHECKER_EXPLORATION_EXPLORATION_DETAIL_STATEGENERATION_H_ */