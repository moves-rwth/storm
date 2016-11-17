#include "src/storm/storage/sparse/StateValuations.h"

namespace storm {
    namespace storage {
        namespace sparse {
            
            StateValuations::StateValuations(state_type const& numberOfStates) : valuations(numberOfStates) {
                // Intentionally left empty.
            }
                        
            std::string StateValuations::stateInfo(state_type const& state) const {
                return valuations[state].toString();
            }
            
        }
    }
}
