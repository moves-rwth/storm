#include "src/logic/BinaryPathFormula.h"

namespace storm {
    namespace logic {
        BinaryPathFormula::BinaryPathFormula(std::shared_ptr<Formula> const& leftSubformula, std::shared_ptr<Formula> const& rightSubformula) : leftSubformula(leftSubformula), rightSubformula(rightSubformula) {
            // Intentionally left empty.
        }

        bool BinaryPathFormula::isBinaryPathFormula() const {
            return true;
        }
        
        Formula& BinaryPathFormula::getLeftSubformula() {
            return *leftSubformula;
        }
        
        Formula const& BinaryPathFormula::getLeftSubformula() const {
            return *leftSubformula;
        }
        
        Formula& BinaryPathFormula::getRightSubformula() {
            return *rightSubformula;
        }
        
        Formula const& BinaryPathFormula::getRightSubformula() const {
            return *rightSubformula;
        }
    }
}