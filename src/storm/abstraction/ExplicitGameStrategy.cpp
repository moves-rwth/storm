#include "storm/abstraction/ExplicitGameStrategy.h"

namespace storm {
    namespace abstraction {
        
        const uint64_t ExplicitGameStrategy::UNDEFINED = std::numeric_limits<uint64_t>::max();
        
        ExplicitGameStrategy::ExplicitGameStrategy(uint64_t numberOfStates) : choices(numberOfStates, UNDEFINED) {
            // Intentionally left empty.
        }
        
        ExplicitGameStrategy::ExplicitGameStrategy(std::vector<uint64_t>&& choices) : choices(std::move(choices)) {
            // Intentionally left empty.
        }
        
        uint64_t ExplicitGameStrategy::getNumberOfStates() const {
            return choices.size();
        }
        
        uint64_t ExplicitGameStrategy::getChoice(uint64_t state) const {
            return choices[state];
        }
        
        void ExplicitGameStrategy::setChoice(uint64_t state, uint64_t choice) {
            choices[state] = choice;
        }
        
        bool ExplicitGameStrategy::hasDefinedChoice(uint64_t state) const {
            return choices[state] != UNDEFINED;
        }
        
        void ExplicitGameStrategy::undefineAll() {
            for (auto& e : choices) {
                e = UNDEFINED;
            }
        }
        
    }
}
