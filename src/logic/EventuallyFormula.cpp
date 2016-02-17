#include "src/logic/EventuallyFormula.h"

#include "src/logic/FormulaVisitor.h"

namespace storm {
    namespace logic {
        EventuallyFormula::EventuallyFormula(std::shared_ptr<Formula const> const& subformula, Context context) : UnaryPathFormula(subformula), context(context) {
            // Intentionally left empty.
        }
        
        bool EventuallyFormula::isEventuallyFormula() const {
            return context == Context::Probability;
        }
        
        bool EventuallyFormula::isReachabilityRewardFormula() const {
            return context == Context::Reward;
        }
        
        bool EventuallyFormula::isReachbilityExpectedTimeFormula() const {
            return context == Context::ExpectedTime;
        }
        
        bool EventuallyFormula::isProbabilityPathFormula() const {
            return this->isEventuallyFormula();
        }
        
        bool EventuallyFormula::isRewardPathFormula() const {
            return this->isReachabilityRewardFormula();
        }
        
        bool EventuallyFormula::isExpectedTimePathFormula() const {
            return this->isReachbilityExpectedTimeFormula();
        }
        
        boost::any EventuallyFormula::accept(FormulaVisitor const& visitor, boost::any const& data) const {
            return visitor.visit(*this, data);
        }
        
        std::shared_ptr<Formula> EventuallyFormula::substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) const {
            return std::make_shared<EventuallyFormula>(this->getSubformula().substitute(substitution));
        }
        
        std::ostream& EventuallyFormula::writeToStream(std::ostream& out) const {
            out << "F ";
            this->getSubformula().writeToStream(out);
            return out;
        }
    }
}