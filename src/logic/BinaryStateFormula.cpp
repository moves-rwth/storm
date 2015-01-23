#include "src/logic/BinaryStateFormula.h"

namespace storm {
    namespace logic {
        BinaryStateFormula::BinaryStateFormula(std::shared_ptr<Formula> const& leftSubformula, std::shared_ptr<Formula> const& rightSubformula) : leftSubformula(leftSubformula), rightSubformula(rightSubformula) {
            // Intentionally left empty.
        }
        
        bool BinaryStateFormula::isBinaryStateFormula() const {
            return true;
        }
        
        Formula& BinaryStateFormula::getLeftSubformula() {
            return *leftSubformula;
        }
        
        Formula const& BinaryStateFormula::getLeftSubformula() const {
            return *leftSubformula;
        }
        
        Formula& BinaryStateFormula::getRightSubformula() {
            return *rightSubformula;
        }
        
        Formula const& BinaryStateFormula::getRightSubformula() const {
            return *rightSubformula;
        }
    }
}