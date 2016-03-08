#include "src/logic/EventuallyFormula.h"
#include "src/logic/FormulaVisitor.h"

#include "src/utility/macros.h"
#include "src/exceptions/InvalidPropertyException.h"

namespace storm {
    namespace logic {
        EventuallyFormula::EventuallyFormula(std::shared_ptr<Formula const> const& subformula, FormulaContext context) : UnaryPathFormula(subformula), context(context) {
            STORM_LOG_THROW(context == FormulaContext::Probability || context == FormulaContext::Reward || context == FormulaContext::ExpectedTime, storm::exceptions::InvalidPropertyException, "Invalid context for formula.");
        }
        
        bool EventuallyFormula::isEventuallyFormula() const {
            return true;
        }
        
        bool EventuallyFormula::isReachabilityProbabilityFormula() const {
            return context == FormulaContext::Probability;
        }
        
        bool EventuallyFormula::isReachabilityRewardFormula() const {
            return context == FormulaContext::Reward;
        }
        
        bool EventuallyFormula::isReachabilityExpectedTimeFormula() const {
            return context == FormulaContext::ExpectedTime;
        }
        
        bool EventuallyFormula::isProbabilityPathFormula() const {
            return this->isEventuallyFormula();
        }
        
        bool EventuallyFormula::isRewardPathFormula() const {
            return this->isReachabilityRewardFormula();
        }
        
        bool EventuallyFormula::isExpectedTimePathFormula() const {
            return this->isReachabilityExpectedTimeFormula();
        }
        
        boost::any EventuallyFormula::accept(FormulaVisitor const& visitor, boost::any const& data) const {
            return visitor.visit(*this, data);
        }
        
        std::shared_ptr<Formula> EventuallyFormula::substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) const {
            return std::make_shared<EventuallyFormula>(this->getSubformula().substitute(substitution), context);
        }
        
        std::ostream& EventuallyFormula::writeToStream(std::ostream& out) const {
            out << "F ";
            this->getSubformula().writeToStream(out);
            return out;
        }
    }
}