#include "src/logic/UntilFormula.h"

#include "src/logic/FormulaVisitor.h"

namespace storm {
    namespace logic {
        UntilFormula::UntilFormula(std::shared_ptr<Formula const> const& leftSubformula, std::shared_ptr<Formula const> const& rightSubformula) : BinaryPathFormula(leftSubformula, rightSubformula) {
            // Intentionally left empty.
        }
        
        bool UntilFormula::isUntilFormula() const {
            return true;
        }
        
        bool UntilFormula::isProbabilityPathFormula() const {
            return true;
        }
        
        boost::any UntilFormula::accept(FormulaVisitor const& visitor, boost::any const& data) const {
            return visitor.visit(*this, data);
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