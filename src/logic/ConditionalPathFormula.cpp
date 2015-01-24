#include "src/logic/ConditionalPathFormula.h"

namespace storm {
    namespace logic {
        ConditionalPathFormula::ConditionalPathFormula(std::shared_ptr<Formula const> const& leftSubformula, std::shared_ptr<Formula const> const& rightSubformula) : BinaryPathFormula(leftSubformula, rightSubformula) {
            // Intentionally left empty.
        }
        
        bool ConditionalPathFormula::isConditionalPathFormula() const {
            return true;
        }
        
        std::ostream& ConditionalPathFormula::writeToStream(std::ostream& out) const {
            this->getLeftSubformula().writeToStream(out);
            out << " || ";
            this->getRightSubformula().writeToStream(out);
            return out;
        }
    }
}