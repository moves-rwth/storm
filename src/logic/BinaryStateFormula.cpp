#include "src/logic/BinaryStateFormula.h"

namespace storm {
    namespace logic {
        BinaryStateFormula::BinaryStateFormula(std::shared_ptr<Formula const> const& leftSubformula, std::shared_ptr<Formula const> const& rightSubformula) : leftSubformula(leftSubformula), rightSubformula(rightSubformula) {
            // Intentionally left empty.
        }
        
        bool BinaryStateFormula::isBinaryStateFormula() const {
            return true;
        }
        
        bool BinaryStateFormula::isPctlStateFormula() const {
            return this->getLeftSubformula().isPctlStateFormula() && this->getRightSubformula().isPctlStateFormula();
        }
        
        bool BinaryStateFormula::isLtlFormula() const {
            return this->getLeftSubformula().isLtlFormula() && this->getRightSubformula().isLtlFormula();
        }
        
        bool BinaryStateFormula::hasProbabilityOperator() const {
            return this->getLeftSubformula().hasProbabilityOperator() || this->getRightSubformula().hasProbabilityOperator();
        }
        
        bool BinaryStateFormula::hasNestedProbabilityOperators() const {
            return this->getLeftSubformula().hasNestedProbabilityOperators() || this->getRightSubformula().hasNestedProbabilityOperators();
        }
        
        Formula const& BinaryStateFormula::getLeftSubformula() const {
            return *leftSubformula;
        }
        
        Formula const& BinaryStateFormula::getRightSubformula() const {
            return *rightSubformula;
        }
        
        void BinaryStateFormula::gatherAtomicExpressionFormulas(std::vector<std::shared_ptr<AtomicExpressionFormula const>>& atomicExpressionFormulas) const {
            this->getLeftSubformula().gatherAtomicExpressionFormulas(atomicExpressionFormulas);
        }
        
        void BinaryStateFormula::gatherAtomicLabelFormulas(std::vector<std::shared_ptr<AtomicLabelFormula const>>& atomicLabelFormulas) const {
            this->getLeftSubformula().gatherAtomicLabelFormulas(atomicLabelFormulas);
        }
    }
}