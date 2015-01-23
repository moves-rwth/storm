#include "src/properties/logic/UntilFormula.h"

namespace storm {
    namespace logic {
        UntilFormula::UntilFormula(std::shared_ptr<Formula> const& leftSubformula, std::shared_ptr<Formula> const& rightSubformula) : BinaryPathFormula(leftSubformula, rightSubformula) {
            // Intentionally left empty.
        }
        
        bool UntilFormula::isUntilFormula() const {
            return true;
        }
        
        std::ostream& UntilFormula::writeToStream(std::ostream& out) const {
            this->getLeftSubformula().writeToStream(out);
            out << " U ";
            this->getRightSubformula().writeToStream(out);
            return out;
        }
    }
}