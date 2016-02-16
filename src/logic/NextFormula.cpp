#include "src/logic/NextFormula.h"

namespace storm {
    namespace logic {
        NextFormula::NextFormula(std::shared_ptr<Formula const> const& subformula) : UnaryPathFormula(subformula) {
            // Intentionally left empty.
        }
        
        bool NextFormula::isNextFormula() const {
            return true;
        }
        
        bool NextFormula::isValidProbabilityPathFormula() const {
            return true;
        }
        
        bool NextFormula::containsNextFormula() const {
            return true;
        }
        
        std::shared_ptr<Formula> NextFormula::substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) const {
            return std::make_shared<NextFormula>(this->getSubformula().substitute(substitution));
        }
        
        std::ostream& NextFormula::writeToStream(std::ostream& out) const {
            out << "X ";
            this->getSubformula().writeToStream(out);
            return out;
        }
    }
}