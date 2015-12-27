#include "src/logic/GloballyFormula.h"

namespace storm {
    namespace logic {
        GloballyFormula::GloballyFormula(std::shared_ptr<Formula const> const& subformula) : UnaryPathFormula(subformula) {
            // Intentionally left empty.
        }
        
        bool GloballyFormula::isGloballyFormula() const {
            return true;
        }
        
        std::shared_ptr<Formula> GloballyFormula::substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) const {
            return std::make_shared<GloballyFormula>(this->getSubformula().substitute(substitution));
        }
        
        std::ostream& GloballyFormula::writeToStream(std::ostream& out) const {
            out << "G ";
            this->getSubformula().writeToStream(out);
            return out;
        }
    }
}