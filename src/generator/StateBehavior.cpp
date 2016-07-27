#include "src/generator/StateBehavior.h"

#include "src/adapters/CarlAdapter.h"

namespace storm {
    namespace generator {

        template<typename ValueType, typename StateType>
        StateBehavior<ValueType, StateType>::StateBehavior() : expanded(false) {
            // Intentionally left empty.
        }
        
        template<typename ValueType, typename StateType>
        void StateBehavior<ValueType, StateType>::addChoice(Choice<ValueType, StateType>&& choice) {
            choices.push_back(std::move(choice));
        }
        
        template<typename ValueType, typename StateType>
        void StateBehavior<ValueType, StateType>::addStateReward(ValueType const& stateReward) {
            stateRewards.push_back(stateReward);
        }
        
        template<typename ValueType, typename StateType>
        void StateBehavior<ValueType, StateType>::setExpanded(bool newValue) {
            this->expanded = newValue;
        }
        
        template<typename ValueType, typename StateType>
        bool StateBehavior<ValueType, StateType>::wasExpanded() const {
            return expanded;
        }
        
        template<typename ValueType, typename StateType>
        bool StateBehavior<ValueType, StateType>::empty() const {
            return choices.empty();
        }
        
        template<typename ValueType, typename StateType>
        typename std::vector<Choice<ValueType, StateType>>::const_iterator StateBehavior<ValueType, StateType>::begin() const {
            return choices.begin();
        }
        
        template<typename ValueType, typename StateType>
        typename std::vector<Choice<ValueType, StateType>>::const_iterator StateBehavior<ValueType, StateType>::end() const {
            return choices.end();
        }
        
        template<typename ValueType, typename StateType>
        std::vector<ValueType> const& StateBehavior<ValueType, StateType>::getStateRewards() const {
            return stateRewards;
        }
        
        template<typename ValueType, typename StateType>
        std::size_t StateBehavior<ValueType, StateType>::getNumberOfChoices() const {
            return choices.size();
        }
        
        template class StateBehavior<double>;

#ifdef STORM_HAVE_CARL
        template class StateBehavior<storm::RationalNumber>;
        template class StateBehavior<storm::RationalFunction>;
#endif
    }
}
