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
        
        bool UnaryStateFormula::isPctlStateFormula() const {
            return this->getSubformula().isPctlStateFormula();
        }
        
        bool UnaryStateFormula::isLtlFormula() const {
            return this->getSubformula().isLtlFormula();
        }
        
        bool UnaryStateFormula::containsProbabilityOperator() const {
            return getSubformula().containsProbabilityOperator();
        }
        
        bool UnaryStateFormula::containsNestedProbabilityOperators() const {
            return getSubformula().containsNestedProbabilityOperators();
        }
        
        bool UnaryStateFormula::containsRewardOperator() const {
            return this->getSubformula().containsRewardOperator();
        }
        
        bool UnaryStateFormula::containsNestedRewardOperators() const {
            return this->getSubformula().containsNestedRewardOperators();
        }
        
        Formula const& UnaryStateFormula::getSubformula() const {
            return *subformula;
        }
        
        void UnaryStateFormula::gatherAtomicExpressionFormulas(std::vector<std::shared_ptr<AtomicExpressionFormula const>>& atomicExpressionFormulas) const {
            this->getSubformula().gatherAtomicExpressionFormulas(atomicExpressionFormulas);
        }
        
        void UnaryStateFormula::gatherAtomicLabelFormulas(std::vector<std::shared_ptr<AtomicLabelFormula const>>& atomicLabelFormulas) const {
            this->getSubformula().gatherAtomicLabelFormulas(atomicLabelFormulas);
        }

    }
}