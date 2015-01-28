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
        
        bool UnaryPathFormula::isLtlFormula() const {
            return this->getSubformula().isLtlFormula();
        }
        
        bool UnaryPathFormula::hasProbabilityOperator() const {
            return this->getSubformula().hasProbabilityOperator();
        }
        
        bool UnaryPathFormula::hasNestedProbabilityOperators() const {
            return this->getSubformula().hasNestedProbabilityOperators();
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
    }
}