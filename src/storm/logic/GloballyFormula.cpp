#include "storm/logic/GloballyFormula.h"

#include "storm/logic/FormulaVisitor.h"

namespace storm {
    namespace logic {
        GloballyFormula::GloballyFormula(std::shared_ptr<Formula const> const& subformula) : UnaryPathFormula(subformula) {
            // Intentionally left empty.
        }
        
        bool GloballyFormula::isGloballyFormula() const {
            return true;
        }

        bool GloballyFormula::isProbabilityPathFormula() const {
            return true;
        }
        
        boost::any GloballyFormula::accept(FormulaVisitor const& visitor, boost::any const& data) const {
            return visitor.visit(*this, data);
        }
        
        std::ostream& GloballyFormula::writeToStream(std::ostream& out) const {
            out << "G ";
            this->getSubformula().writeToStream(out);
            return out;
        }
    }
}
