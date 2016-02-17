#include "src/logic/ConditionalFormula.h"

#include "src/logic/FormulaVisitor.h"

namespace storm {
    namespace logic {
        ConditionalFormula::ConditionalFormula(std::shared_ptr<Formula const> const& subformula, std::shared_ptr<Formula const> const& conditionFormula, Context context) : subformula(subformula), conditionFormula(conditionFormula), context(context)  {
            // Intentionally left empty.
        }
        
        Formula const& ConditionalFormula::getSubformula() const {
            return *subformula;
        }
        
        Formula const& ConditionalFormula::getConditionFormula() const {
            return *conditionFormula;
        }
        
        bool ConditionalFormula::isConditionalProbabilityFormula() const {
            return context == Context::Probability;
        }
        
        bool ConditionalFormula::isConditionalRewardFormula() const {
            return context == Context::Reward;
        }
        
        boost::any ConditionalFormula::accept(FormulaVisitor const& visitor, boost::any const& data) const {
            return visitor.visit(*this, data);
        }
        
        std::shared_ptr<Formula> ConditionalFormula::substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) const {
            return std::make_shared<ConditionalFormula>(this->getSubformula().substitute(substitution), this->getConditionFormula().substitute(substitution));
        }
        
        std::ostream& ConditionalFormula::writeToStream(std::ostream& out) const {
            this->getSubformula().writeToStream(out);
            out << " || ";
            this->getConditionFormula().writeToStream(out);
            return out;
        }
    }
}