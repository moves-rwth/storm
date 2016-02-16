#include "src/logic/UnaryPathFormula.h"

namespace storm {
    namespace logic {
        UnaryPathFormula::UnaryPathFormula(std::shared_ptr<Formula const> const& subformula) : subformula(subformula) {
            // Intentionally left empty.
        }
        
        bool UnaryPathFormula::isUnaryPathFormula() const {
            return true;
        }
        
        bool UnaryPathFormula::isPctlPathFormula() const {
            return this->getSubformula().isPctlStateFormula();
        }
        
        bool UnaryPathFormula::isPctlWithConditionalPathFormula() const {
            return this->getSubformula().isPctlWithConditionalStateFormula();
        }
        
        bool UnaryPathFormula::isLtlFormula() const {
            return this->getSubformula().isLtlFormula();
        }
        
        bool UnaryPathFormula::containsBoundedUntilFormula() const {
            return this->getSubformula().containsBoundedUntilFormula();
        }
        
        bool UnaryPathFormula::containsNextFormula() const {
            return this->getSubformula().containsNextFormula();
        }
        
        bool UnaryPathFormula::containsProbabilityOperator() const {
            return this->getSubformula().containsProbabilityOperator();
        }
        
        bool UnaryPathFormula::containsNestedProbabilityOperators() const {
            return this->getSubformula().containsNestedProbabilityOperators();
        }
        
        bool UnaryPathFormula::containsRewardOperator() const {
            return this->getSubformula().containsRewardOperator();
        }
        
        bool UnaryPathFormula::containsNestedRewardOperators() const {
            return this->getSubformula().containsNestedRewardOperators();
        }
        
        Formula const& UnaryPathFormula::getSubformula() const {
            return *subformula;
        }
        
        void UnaryPathFormula::gatherAtomicExpressionFormulas(std::vector<std::shared_ptr<AtomicExpressionFormula const>>& atomicExpressionFormulas) const {
            this->getSubformula().gatherAtomicExpressionFormulas(atomicExpressionFormulas);
        }
        
        void UnaryPathFormula::gatherAtomicLabelFormulas(std::vector<std::shared_ptr<AtomicLabelFormula const>>& atomicLabelFormulas) const {
            this->getSubformula().gatherAtomicLabelFormulas(atomicLabelFormulas);
        }
        
        void UnaryPathFormula::gatherReferencedRewardModels(std::set<std::string>& referencedRewardModels) const {
            this->getSubformula().gatherReferencedRewardModels(referencedRewardModels);
        }
    }
}