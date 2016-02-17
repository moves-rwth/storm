#include "src/logic/NextFormula.h"

#include "src/logic/FormulaVisitor.h"

namespace storm {
    namespace logic {
        NextFormula::NextFormula(std::shared_ptr<Formula const> const& subformula) : UnaryPathFormula(subformula) {
            // Intentionally left empty.
        }
        
        bool NextFormula::isNextFormula() const {
            return true;
        }
        
        bool NextFormula::isProbabilityPathFormula() const {
            return true;
        }
        
        boost::any NextFormula::accept(FormulaVisitor const& visitor, boost::any const& data) const {
            return visitor.visit(*this, data);
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