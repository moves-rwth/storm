#include "src/modelchecker/exploration/StateGeneration.h"

namespace storm {
    namespace modelchecker {
        namespace exploration_detail {
            
            template <typename StateType, typename ValueType>
            StateGeneration<StateType, ValueType>::StateGeneration(storm::prism::Program const& program, storm::generator::VariableInformation const& variableInformation, storm::expressions::Expression const& conditionStateExpression, storm::expressions::Expression const& targetStateExpression) : generator(program, variableInformation, false), conditionStateExpression(conditionStateExpression), targetStateExpression(targetStateExpression) {
                // Intentionally left empty.
            }
            
            template <typename StateType, typename ValueType>
            void StateGeneration<StateType, ValueType>::setStateToIdCallback(std::function<StateType (storm::generator::CompressedState const&)> const& stateToIdCallback) {
                this->stateToIdCallback = stateToIdCallback;
            }
            
            template <typename StateType, typename ValueType>
            void StateGeneration<StateType, ValueType>::load(storm::generator::CompressedState const& state) {
                generator.load(state);
            }
            
            template <typename StateType, typename ValueType>
            std::vector<StateType> StateGeneration<StateType, ValueType>::getInitialStates() {
                return generator.getInitialStates(stateToIdCallback);
            }
            
            template <typename StateType, typename ValueType>
            storm::generator::StateBehavior<ValueType, StateType> StateGeneration<StateType, ValueType>::expand() {
                return generator.expand(stateToIdCallback);
            }
            
            template <typename StateType, typename ValueType>
            bool StateGeneration<StateType, ValueType>::isConditionState() const {
                return generator.satisfies(conditionStateExpression);
            }
            
            template <typename StateType, typename ValueType>
            bool StateGeneration<StateType, ValueType>::isTargetState() const {
                return generator.satisfies(targetStateExpression);
            }
         
            template class StateGeneration<uint32_t, double>;
        }
    }
}