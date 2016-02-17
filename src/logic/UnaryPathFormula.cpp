#include "src/logic/UnaryPathFormula.h"

namespace storm {
    namespace logic {
        UnaryPathFormula::UnaryPathFormula(std::shared_ptr<Formula const> const& subformula) : subformula(subformula) {
            // Intentionally left empty.
        }
        
        bool UnaryPathFormula::isUnaryPathFormula() const {
            return true;
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