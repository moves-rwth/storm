#include "src/logic/BinaryPathFormula.h"

namespace storm {
    namespace logic {
        BinaryPathFormula::BinaryPathFormula(std::shared_ptr<Formula const> const& leftSubformula, std::shared_ptr<Formula const> const& rightSubformula) : leftSubformula(leftSubformula), rightSubformula(rightSubformula) {
            // Intentionally left empty.
        }

        bool BinaryPathFormula::isBinaryPathFormula() const {
            return true;
        }
        
        bool BinaryPathFormula::isPctlPathFormula() const {
            return this->getLeftSubformula().isPctlStateFormula() && this->getRightSubformula().isPctlStateFormula();
        }
        
        bool BinaryPathFormula::isLtlFormula() const {
            return this->getLeftSubformula().isLtlFormula() && this->getRightSubformula().isLtlFormula();
        }
        
        bool BinaryPathFormula::hasProbabilityOperator() const {
            return this->getLeftSubformula().hasProbabilityOperator() || this->getRightSubformula().hasProbabilityOperator();
        }
        
        bool BinaryPathFormula::hasNestedProbabilityOperators() const {
            return this->getLeftSubformula().Formula::hasNestedProbabilityOperators() || this->getRightSubformula().hasNestedProbabilityOperators();
        }
        
        Formula const& BinaryPathFormula::getLeftSubformula() const {
            return *leftSubformula;
        }

        Formula const& BinaryPathFormula::getRightSubformula() const {
            return *rightSubformula;
        }
    }
}