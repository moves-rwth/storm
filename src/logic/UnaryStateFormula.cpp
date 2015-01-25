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
        
        bool UnaryStateFormula::hasProbabilityOperator() const {
            return getSubformula().hasProbabilityOperator();
        }
        
        bool UnaryStateFormula::hasNestedProbabilityOperators() const {
            return getSubformula().hasNestedProbabilityOperators();
        }
        
        Formula const& UnaryStateFormula::getSubformula() const {
            return *subformula;
        }
    }
}