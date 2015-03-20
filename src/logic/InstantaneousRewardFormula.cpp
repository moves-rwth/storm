#include "src/logic/InstantaneousRewardFormula.h"

namespace storm {
    namespace logic {
        InstantaneousRewardFormula::InstantaneousRewardFormula(uint_fast64_t timeBound) : timeBound(timeBound) {
            // Intentionally left empty.
        }
        
        InstantaneousRewardFormula::InstantaneousRewardFormula(double timeBound) : timeBound(timeBound) {
            // Intentionally left empty.            
        }
        
        bool InstantaneousRewardFormula::isInstantaneousRewardFormula() const {
            return true;
        }
        
        bool InstantaneousRewardFormula::hasDiscreteTimeBound() const {
            return timeBound.which() == 0;
        }
        
        uint_fast64_t InstantaneousRewardFormula::getDiscreteTimeBound() const {
            return boost::get<uint_fast64_t>(timeBound);
        }
        
        bool InstantaneousRewardFormula::hasContinuousTimeBound() const {
            return timeBound.which() == 1;
        }
        
        double InstantaneousRewardFormula::getContinuousTimeBound() const {
            if (this->hasDiscreteTimeBound()) {
                return this->getDiscreteTimeBound();
            } else {
                return boost::get<double>(timeBound);
            }
        }
        
        std::ostream& InstantaneousRewardFormula::writeToStream(std::ostream& out) const {
            if (this->hasDiscreteTimeBound()) {
                out << "I=" << this->getDiscreteTimeBound();
            } else {
                out << "I=" << this->getContinuousTimeBound();
            }
            return out;
        }
    }
}