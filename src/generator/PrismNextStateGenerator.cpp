#include "src/generator/PrismNextStateGenerator.h"

namespace storm {
    namespace generator {
        
        template<typename ValueType, typename StateType>
        PrismNextStateGenerator<ValueType, StateType>::PrismNextStateGenerator(storm::prism::Program const& program) : program(program) {
            // Intentionally left empty.
        }
        
        template<typename ValueType, typename StateType>
        std::vector<Choice<ValueType>> PrismNextStateGenerator<ValueType, StateType>::expand(StateType const& state, typename NextStateGenerator<ValueType, StateType>::StateToIdCallback stateToIdCallback) {
            // TODO
        }
        
        template<typename ValueType, typename StateType>
        ValueType PrismNextStateGenerator<ValueType, StateType>::getStateReward(StateType const& state) {
            // TODO
        }
        
    }
}