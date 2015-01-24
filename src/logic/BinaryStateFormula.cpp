#include "src/logic/BinaryStateFormula.h"

namespace storm {
    namespace logic {
        BinaryStateFormula::BinaryStateFormula(std::shared_ptr<Formula const> const& leftSubformula, std::shared_ptr<Formula const> const& rightSubformula) : leftSubformula(leftSubformula), rightSubformula(rightSubformula) {
            // Intentionally left empty.
        }
        
        bool BinaryStateFormula::isBinaryStateFormula() const {
            return true;
        }
        Formula const& BinaryStateFormula::getLeftSubformula() const {
            return *leftSubformula;
        }
        
        Formula const& BinaryStateFormula::getRightSubformula() const {
            return *rightSubformula;
        }
    }
}