#ifndef STORM_GENERATOR_NEXTSTATEGENERATOR_H_
#define STORM_GENERATOR_NEXTSTATEGENERATOR_H_

#include <vector>
#include <cstdint>

#include "src/storage/sparse/StateType.h"
#include "src/storage/BitVector.h"

#include "src/generator/Choice.h"

namespace storm {
    namespace generator {
        typedef storm::storage::BitVector CompressedState;

        template<typename ValueType, typename StateType = uint32_t>
        class NextStateGenerator {
        public:
            typedef StateType (*StateToIdCallback)(CompressedState const&);

            virtual std::vector<StateType> getInitialStates(StateToIdCallback stateToIdCallback) = 0;
            virtual std::vector<Choice<ValueType>> expand(CompressedState const& state, StateToIdCallback stateToIdCallback) = 0;
        };
    }
}

#endif