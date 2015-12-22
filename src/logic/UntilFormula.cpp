#include "src/logic/UntilFormula.h"

namespace storm {
    namespace logic {
        UntilFormula::UntilFormula(std::shared_ptr<Formula const> const& leftSubformula, std::shared_ptr<Formula const> const& rightSubformula) : BinaryPathFormula(leftSubformula, rightSubformula) {
            // Intentionally left empty.
        }
        
        bool UntilFormula::isUntilFormula() const {
            return true;
        }
        
        std::shared_ptr<Formula> UntilFormula::substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) const {
            return std::make_shared<UntilFormula>(this->getLeftSubformula().substitute(substitution), this->getRightSubformula().substitute(substitution));
        }
        
        std::ostream& UntilFormula::writeToStream(std::ostream& out) const {
            this->getLeftSubformula().writeToStream(out);
            out << " U ";
            this->getRightSubformula().writeToStream(out);
            return out;
        }
    }
}