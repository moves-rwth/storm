#ifndef STORM_GENERATOR_NEXTSTATEGENERATOR_H_
#define STORM_GENERATOR_NEXTSTATEGENERATOR_H_

#include <vector>
#include <cstdint>

#include "src/storage/sparse/StateType.h"
#include "src/storage/BitVector.h"

#include "src/generator/Choice.h"

namespace storm {
    namespace generator {
        template<typename ValueType, typename StateType = uint32_t>
        class NextStateGenerator {
        public:
            typedef storm::storage::BitVector InternalStateType;
            typedef StateType (*StateToIdCallback)(InternalStateType const&);
            
            virtual std::vector<StateType> getInitialStates(StateToIdCallback stateToIdCallback) = 0;
            virtual std::vector<Choice<ValueType>> expand(StateType const& state, StateToIdCallback stateToIdCallback) = 0;
            virtual ValueType getStateReward(StateType const& state) = 0;
        };
    }
}

#endif