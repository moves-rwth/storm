#ifndef STORM_GENERATOR_PRISM_STATEBEHAVIOR_H_
#define STORM_GENERATOR_PRISM_STATEBEHAVIOR_H_

#include <cstdint>

#include "src/generator/Choice.h"

namespace storm {
    namespace generator {
        
        template<typename ValueType, typename StateType = uint32_t>
        class StateBehavior {
        public:
            /*!
             * Adds the given choice to the behavior of the state.
             */
            void addChoice(Choice<ValueType, StateType>&& choice);
            
            /*!
             * Adds the given state reward to the behavior of the state.
             */
            void addStateReward(ValueType const& stateReward);
            
        private:
            // The choices available in the state.
            std::vector<Choice<ValueType, StateType>> choices;
            
            // The state rewards (under the different, selected reward models) of the state.
            std::vector<ValueType> stateRewards;
        };
        
    }
}

#endif /* STORM_GENERATOR_PRISM_STATEBEHAVIOR_H_ */
