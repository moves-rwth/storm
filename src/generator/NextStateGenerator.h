#ifndef STORM_GENERATOR_NEXTSTATEGENERATOR_H_
#define STORM_GENERATOR_NEXTSTATEGENERATOR_H_

#include <vector>
#include <cstdint>

#include "src/storage/expressions/Expression.h"

#include "src/generator/CompressedState.h"
#include "src/generator/StateBehavior.h"

namespace storm {
    namespace generator {
        template<typename ValueType, typename StateType = uint32_t>
        class NextStateGenerator {
        public:
            typedef std::function<StateType (CompressedState const&)> StateToIdCallback;

            virtual bool isDeterministicModel() const = 0;
            virtual std::vector<StateType> getInitialStates(StateToIdCallback const& stateToIdCallback) = 0;
            
            virtual void load(CompressedState const& state) = 0;
            virtual StateBehavior<ValueType, StateType> expand(StateToIdCallback const& stateToIdCallback) = 0;
            virtual bool satisfies(storm::expressions::Expression const& expression) const = 0;
        };
    }
}

#endif /* STORM_GENERATOR_NEXTSTATEGENERATOR_H_ */