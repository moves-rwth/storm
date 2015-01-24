#include "src/logic/ReachabilityRewardFormula.h"

namespace storm {
    namespace logic {
        ReachabilityRewardFormula::ReachabilityRewardFormula(std::shared_ptr<Formula const> const& subformula) : subformula(subformula) {
            // Intentionally left empty.
        }
        
        bool ReachabilityRewardFormula::isReachabilityRewardFormula() const {
            return true;
        }
        
        Formula const& ReachabilityRewardFormula::getSubformula() const {
            return *subformula;
        }
        
        std::ostream& ReachabilityRewardFormula::writeToStream(std::ostream& out) const {
            out << "F ";
            this->getSubformula().writeToStream(out);
            return out;
        }
    }
}