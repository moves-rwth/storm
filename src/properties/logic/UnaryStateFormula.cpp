#include "src/properties/logic/UnaryStateFormula.h"

namespace storm {
    namespace logic {
        UnaryStateFormula::UnaryStateFormula(std::shared_ptr<Formula> subformula) : subformula(subformula) {
            // Intentionally left empty.
        }
        
        bool UnaryStateFormula::isUnaryStateFormula() const {
            return true;
        }
        
        Formula& UnaryStateFormula::getSubformula() {
            return *subformula;
        }
        
        Formula const& UnaryStateFormula::getSubformula() const {
            return *subformula;
        }
    }
}