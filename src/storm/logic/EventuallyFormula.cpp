#include "storm/logic/EventuallyFormula.h"
#include "storm/logic/FormulaVisitor.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/InvalidPropertyException.h"

namespace storm {
    namespace logic {
        EventuallyFormula::EventuallyFormula(std::shared_ptr<Formula const> const& subformula, FormulaContext context) : UnaryPathFormula(subformula), context(context) {
            STORM_LOG_THROW(context == FormulaContext::Probability || context == FormulaContext::Reward || context == FormulaContext::Time, storm::exceptions::InvalidPropertyException, "Invalid context for formula.");
        }
        
        FormulaContext const& EventuallyFormula::getContext() const {
            return context;
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
        
        bool EventuallyFormula::isReachabilityTimeFormula() const {
            return context == FormulaContext::Time;
        }
        
        bool EventuallyFormula::isProbabilityPathFormula() const {
            return this->isReachabilityProbabilityFormula();
        }
        
        bool EventuallyFormula::isRewardPathFormula() const {
            return this->isReachabilityRewardFormula();
        }
        
        bool EventuallyFormula::isTimePathFormula() const {
            return this->isReachabilityTimeFormula();
        }
        
        boost::any EventuallyFormula::accept(FormulaVisitor const& visitor, boost::any const& data) const {
            return visitor.visit(*this, data);
        }
                
        std::ostream& EventuallyFormula::writeToStream(std::ostream& out) const {
            out << "F ";
            this->getSubformula().writeToStream(out);
            return out;
        }
    }
}
