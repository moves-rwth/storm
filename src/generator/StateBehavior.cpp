#include "src/generator/StateBehavior.h"

namespace storm {
    namespace generator {

        template<typename ValueType, typename StateType>
        void StateBehavior<ValueType, StateType>::addChoice(Choice<ValueType, StateType>&& choice) {
            choices.push_back(std::move(choice));
        }
        
        template<typename ValueType, typename StateType>
        void StateBehavior<ValueType, StateType>::addStateReward(ValueType const& stateReward) {
            stateRewards.push_back(stateReward);
        }
        
    }
}