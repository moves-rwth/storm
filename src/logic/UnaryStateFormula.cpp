#include "src/logic/UnaryStateFormula.h"

namespace storm {
    namespace logic {
        UnaryStateFormula::UnaryStateFormula(std::shared_ptr<Formula const> subformula) : subformula(subformula) {
            // Intentionally left empty.
        }
        
        bool UnaryStateFormula::isUnaryStateFormula() const {
            return true;
        }
        
        bool UnaryStateFormula::isPropositionalFormula() const {
            return this->getSubformula().isPropositionalFormula();
        }
        
        Formula const& UnaryStateFormula::getSubformula() const {
            return *subformula;
        }
    }
}