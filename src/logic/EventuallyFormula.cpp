#include "src/logic/EventuallyFormula.h"

namespace storm {
    namespace logic {
        EventuallyFormula::EventuallyFormula(std::shared_ptr<Formula const> const& subformula) : UnaryPathFormula(subformula) {
            // Intentionally left empty.
        }
        
        bool EventuallyFormula::isEventuallyFormula() const {
            return true;
        }
        
        bool EventuallyFormula::isValidProbabilityPathFormula() const {
            return true;
        }
        
        bool EventuallyFormula::isValidRewardPathFormula() const {
            return true;
        }
        
        std::shared_ptr<Formula> EventuallyFormula::substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) const {
            return std::make_shared<EventuallyFormula>(this->getSubformula().substitute(substitution));
        }
        
        std::ostream& EventuallyFormula::writeToStream(std::ostream& out) const {
            out << "F ";
            this->getSubformula().writeToStream(out);
            return out;
        }
    }
}