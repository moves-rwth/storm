#include "src/logic/UnaryStateFormula.h"

#include "src/logic/FormulaVisitor.h"

namespace storm {
    namespace logic {
        UnaryStateFormula::UnaryStateFormula(std::shared_ptr<Formula const> subformula) : subformula(subformula) {
            // Intentionally left empty.
        }
        
        bool UnaryStateFormula::isUnaryStateFormula() const {
            return true;
        }
        
        boost::any UnaryStateFormula::accept(FormulaVisitor const& visitor, boost::any const& data) const {
            return visitor.visit(*this, data);
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
        
        void UnaryStateFormula::gatherReferencedRewardModels(std::set<std::string>& referencedRewardModels) const {
            this->getSubformula().gatherReferencedRewardModels(referencedRewardModels);
        }

    }
}