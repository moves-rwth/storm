#include "src/properties/logic/ConditionalPathFormula.h"

namespace storm {
    namespace logic {
        ConditionalPathFormula::ConditionalPathFormula(std::shared_ptr<Formula> const& leftSubformula, std::shared_ptr<Formula> const& rightSubformula) : BinaryPathFormula(leftSubformula, rightSubformula) {
            // Intentionally left empty.
        }
        
        bool ConditionalPathFormula::isConditionalPathFormula() const {
            return true;
        }
        
        std::ostream& ConditionalPathFormula::writeToStream(std::ostream& out) const {
            this->getLeftSubformula().writeToStream(out);
            out << " | ";
            this->getRightSubformula().writeToStream(out);
            return out;
        }
    }
}