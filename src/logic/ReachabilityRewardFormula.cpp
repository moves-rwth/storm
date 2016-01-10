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
        
        void ReachabilityRewardFormula::gatherAtomicExpressionFormulas(std::vector<std::shared_ptr<AtomicExpressionFormula const>>& atomicExpressionFormulas) const {
            this->getSubformula().gatherAtomicExpressionFormulas(atomicExpressionFormulas);
        }
        
        void ReachabilityRewardFormula::gatherAtomicLabelFormulas(std::vector<std::shared_ptr<AtomicLabelFormula const>>& atomicLabelFormulas) const {
            this->getSubformula().gatherAtomicLabelFormulas(atomicLabelFormulas);
        }
        
        std::shared_ptr<Formula> ReachabilityRewardFormula::substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) const {
            return std::make_shared<ReachabilityRewardFormula>(this->getSubformula().substitute(substitution));
        }
        
        std::ostream& ReachabilityRewardFormula::writeToStream(std::ostream& out) const {
            out << "F ";
            this->getSubformula().writeToStream(out);
            return out;
        }
    }
}