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
        
        bool BinaryPathFormula::containsProbabilityOperator() const {
            return this->getLeftSubformula().containsProbabilityOperator() || this->getRightSubformula().containsProbabilityOperator();
        }
        
        bool BinaryPathFormula::containsNestedProbabilityOperators() const {
            return this->getLeftSubformula().containsNestedProbabilityOperators() || this->getRightSubformula().containsNestedProbabilityOperators();
        }
        
        bool BinaryPathFormula::containsRewardOperator() const {
            return this->getLeftSubformula().containsRewardOperator() || this->getRightSubformula().containsRewardOperator();
        }
        
        bool BinaryPathFormula::containsNestedRewardOperators() const {
            return this->getLeftSubformula().containsNestedRewardOperators() || this->getRightSubformula().containsNestedRewardOperators();
        }
        
        Formula const& BinaryPathFormula::getLeftSubformula() const {
            return *leftSubformula;
        }

        Formula const& BinaryPathFormula::getRightSubformula() const {
            return *rightSubformula;
        }
        
        void BinaryPathFormula::gatherAtomicExpressionFormulas(std::vector<std::shared_ptr<AtomicExpressionFormula const>>& atomicExpressionFormulas) const {
            this->getLeftSubformula().gatherAtomicExpressionFormulas(atomicExpressionFormulas);
            this->getLeftSubformula().gatherAtomicExpressionFormulas(atomicExpressionFormulas);
        }
        
        void BinaryPathFormula::gatherAtomicLabelFormulas(std::vector<std::shared_ptr<AtomicLabelFormula const>>& atomicLabelFormulas) const {
            this->getLeftSubformula().gatherAtomicLabelFormulas(atomicLabelFormulas);
            this->getRightSubformula().gatherAtomicLabelFormulas(atomicLabelFormulas);
        }
    }
}