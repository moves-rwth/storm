#include "src/logic/ConditionalFormula.h"
#include "src/logic/FormulaVisitor.h"

#include "src/utility/macros.h"
#include "src/exceptions/InvalidPropertyException.h"

namespace storm {
    namespace logic {
        ConditionalFormula::ConditionalFormula(std::shared_ptr<Formula const> const& subformula, std::shared_ptr<Formula const> const& conditionFormula, FormulaContext context) : subformula(subformula), conditionFormula(conditionFormula), context(context)  {
            STORM_LOG_THROW(context == FormulaContext::Probability || context == FormulaContext::Reward, storm::exceptions::InvalidPropertyException, "Invalid context for formula.");
        }
        
        Formula const& ConditionalFormula::getSubformula() const {
            return *subformula;
        }
        
        Formula const& ConditionalFormula::getConditionFormula() const {
            return *conditionFormula;
        }
        
        FormulaContext const& ConditionalFormula::getContext() const {
            return context;
        }
        
        bool ConditionalFormula::isConditionalProbabilityFormula() const {
            return context == FormulaContext::Probability;
        }
        
        bool ConditionalFormula::isConditionalRewardFormula() const {
            return context == FormulaContext::Reward;
        }
        
        boost::any ConditionalFormula::accept(FormulaVisitor const& visitor, boost::any const& data) const {
            return visitor.visit(*this, data);
        }
        
        std::shared_ptr<Formula> ConditionalFormula::substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) const {
            return std::make_shared<ConditionalFormula>(this->getSubformula().substitute(substitution), this->getConditionFormula().substitute(substitution), context);
        }
        
        void ConditionalFormula::gatherAtomicExpressionFormulas(std::vector<std::shared_ptr<AtomicExpressionFormula const>>& atomicExpressionFormulas) const {
            this->getSubformula().gatherAtomicExpressionFormulas(atomicExpressionFormulas);
            this->getConditionFormula().gatherAtomicExpressionFormulas(atomicExpressionFormulas);
        }
        
        void ConditionalFormula::gatherAtomicLabelFormulas(std::vector<std::shared_ptr<AtomicLabelFormula const>>& atomicLabelFormulas) const {
            this->getSubformula().gatherAtomicLabelFormulas(atomicLabelFormulas);
            this->getConditionFormula().gatherAtomicLabelFormulas(atomicLabelFormulas);
        }
        
        void ConditionalFormula::gatherReferencedRewardModels(std::set<std::string>& referencedRewardModels) const {
            this->getSubformula().gatherReferencedRewardModels(referencedRewardModels);
            this->getConditionFormula().gatherReferencedRewardModels(referencedRewardModels);
        }
        
        bool ConditionalFormula::hasQualitativeResult() const {
            return false;
        }
        
        bool ConditionalFormula::hasQuantitativeResult() const {
            return true;
        }
        
        std::ostream& ConditionalFormula::writeToStream(std::ostream& out) const {
            this->getSubformula().writeToStream(out);
            out << " || ";
            this->getConditionFormula().writeToStream(out);
            return out;
        }
    }
}