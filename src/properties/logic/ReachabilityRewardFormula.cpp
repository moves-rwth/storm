#include "src/properties/logic/ReachabilityRewardFormula.h"

namespace storm {
    namespace logic {
        ReachabilityRewardFormula::ReachabilityRewardFormula(std::shared_ptr<StateFormula> const& subformula) : subformula(subformula) {
            // Intentionally left empty.
        }
        
        bool ReachabilityRewardFormula::isReachabilityRewardFormula() const {
            return true;
        }
        
        StateFormula& ReachabilityRewardFormula::getSubformula() {
            return *subformula;
        }
        
        StateFormula const& ReachabilityRewardFormula::getSubformula() const {
            return *subformula;
        }
        
        std::ostream& ReachabilityRewardFormula::writeToStream(std::ostream& out) const {
            out << "F ";
            this->getSubformula().writeToStream(out);
            return out;
        }
    }
}