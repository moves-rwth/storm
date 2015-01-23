#include "src/logic/NextFormula.h"

namespace storm {
    namespace logic {
        NextFormula::NextFormula(std::shared_ptr<Formula> const& subformula) : UnaryPathFormula(subformula) {
            // Intentionally left empty.
        }
        
        bool NextFormula::isNextFormula() const {
            return true;
        }
        
        std::ostream& NextFormula::writeToStream(std::ostream& out) const {
            out << "X ";
            this->getSubformula().writeToStream(out);
            return out;
        }
    }
}