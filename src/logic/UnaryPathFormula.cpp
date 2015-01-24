#include "src/logic/UnaryPathFormula.h"

namespace storm {
    namespace logic {
        UnaryPathFormula::UnaryPathFormula(std::shared_ptr<Formula const> const& subformula) : subformula(subformula) {
            // Intentionally left empty.
        }
        
        bool UnaryPathFormula::isUnaryPathFormula() const {
            return true;
        }
                
        Formula const& UnaryPathFormula::getSubformula() const {
            return *subformula;
        }
    }
}