#include "src/logic/GloballyFormula.h"

namespace storm {
    namespace logic {
        GloballyFormula::GloballyFormula(std::shared_ptr<Formula> const& subformula) : UnaryPathFormula(subformula) {
            // Intentionally left empty.
        }
        
        bool GloballyFormula::isGloballyFormula() const {
            return true;
        }
        
        std::ostream& GloballyFormula::writeToStream(std::ostream& out) const {
            out << "G ";
            this->getSubformula().writeToStream(out);
            return out;
        }
    }
}