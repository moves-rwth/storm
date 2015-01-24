#include "src/logic/InstantaneousRewardFormula.h"

namespace storm {
    namespace logic {
        InstantaneousRewardFormula::InstantaneousRewardFormula(uint_fast64_t stepCount) : stepCount(stepCount) {
            // Intentionally left empty.
        }
        
        bool InstantaneousRewardFormula::isInstantaneousRewardFormula() const {
            return true;
        }
        
        uint_fast64_t InstantaneousRewardFormula::getStepCount() const {
            return stepCount;
        }
        
        std::ostream& InstantaneousRewardFormula::writeToStream(std::ostream& out) const {
            out << "I=" << stepCount;
            return out;
        }
    }
}