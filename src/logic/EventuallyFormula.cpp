#include "src/logic/EventuallyFormula.h"

namespace storm {
    namespace logic {
        EventuallyFormula::EventuallyFormula(std::shared_ptr<Formula const> const& subformula) : UnaryPathFormula(subformula) {
            // Intentionally left empty.
        }
        
        bool EventuallyFormula::isEventuallyFormula() const {
            return true;
        }
        
        std::ostream& EventuallyFormula::writeToStream(std::ostream& out) const {
            out << "F ";
            this->getSubformula().writeToStream(out);
            return out;
        }
    }
}