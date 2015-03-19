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

        bool BinaryStateFormula::isCslStateFormula() const {
            return this->getLeftSubformula().isCslStateFormula() && this->getRightSubformula().isCslStateFormula();
        }
        
        bool BinaryStateFormula::isLtlFormula() const {
            return this->getLeftSubformula().isLtlFormula() && this->getRightSubformula().isLtlFormula();
        }
        
        bool BinaryStateFormula::containsBoundedUntilFormula() const {
            return this->getLeftSubformula().containsBoundedUntilFormula() || this->getRightSubformula().containsBoundedUntilFormula();
        }
        
        bool BinaryStateFormula::containsNextFormula() const {
            return this->getLeftSubformula().containsNextFormula() || this->getRightSubformula().containsNextFormula();
        }
        
        bool BinaryStateFormula::containsProbabilityOperator() const {
            return this->getLeftSubformula().containsProbabilityOperator() || this->getRightSubformula().containsProbabilityOperator();
        }
        
        bool BinaryStateFormula::containsNestedProbabilityOperators() const {
            return this->getLeftSubformula().containsNestedProbabilityOperators() || this->getRightSubformula().containsNestedProbabilityOperators();
        }
        
        bool BinaryStateFormula::containsRewardOperator() const {
            return this->getLeftSubformula().containsRewardOperator() || this->getRightSubformula().containsRewardOperator();
        }
        
        bool BinaryStateFormula::containsNestedRewardOperators() const {
            return this->containsNestedRewardOperators() || this->getRightSubformula().containsNestedRewardOperators();
        }
        
        Formula const& BinaryStateFormula::getLeftSubformula() const {
            return *leftSubformula;
        }
        
        Formula const& BinaryStateFormula::getRightSubformula() const {
            return *rightSubformula;
        }
        
        void BinaryStateFormula::gatherAtomicExpressionFormulas(std::vector<std::shared_ptr<AtomicExpressionFormula const>>& atomicExpressionFormulas) const {
            this->getLeftSubformula().gatherAtomicExpressionFormulas(atomicExpressionFormulas);
            this->getRightSubformula().gatherAtomicExpressionFormulas(atomicExpressionFormulas);
        }
        
        void BinaryStateFormula::gatherAtomicLabelFormulas(std::vector<std::shared_ptr<AtomicLabelFormula const>>& atomicLabelFormulas) const {
            this->getLeftSubformula().gatherAtomicLabelFormulas(atomicLabelFormulas);
            this->getRightSubformula().gatherAtomicLabelFormulas(atomicLabelFormulas);
        }
    }
}