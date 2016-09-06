#include "src/storage/jani/BoundedIntegerVariable.h"
#include "src/exceptions/NotImplementedException.h"
#include "src/utility/macros.h"

namespace storm {
    namespace jani {
        
        BoundedIntegerVariable::BoundedIntegerVariable(std::string const& name, storm::expressions::Variable const& variable, storm::expressions::Expression const& initValue, bool transient, storm::expressions::Expression const& lowerBound, storm::expressions::Expression const& upperBound) : Variable(name, variable, initValue, transient), lowerBound(lowerBound), upperBound(upperBound) {
            // Intentionally left empty.
        }
        
        BoundedIntegerVariable::BoundedIntegerVariable(std::string const& name, storm::expressions::Variable const& variable, storm::expressions::Expression const& initValue, storm::expressions::Expression const& lowerBound, storm::expressions::Expression const& upperBound) : Variable(name, variable, initValue), lowerBound(lowerBound), upperBound(upperBound) {
            // Intentionally left empty.
        }
 
        BoundedIntegerVariable::BoundedIntegerVariable(std::string const& name, storm::expressions::Variable const& variable, bool transient, storm::expressions::Expression const& lowerBound, storm::expressions::Expression const& upperBound) : Variable(name, variable, transient), lowerBound(lowerBound), upperBound(upperBound) {
            // Intentionally left empty.
        }

        BoundedIntegerVariable::BoundedIntegerVariable(std::string const& name, storm::expressions::Variable const& variable, storm::expressions::Expression const& lowerBound, storm::expressions::Expression const& upperBound) : Variable(name, variable), lowerBound(lowerBound), upperBound(upperBound) {
            // Intentionally left empty.
        }


        storm::expressions::Expression const& BoundedIntegerVariable::getLowerBound() const {
            return lowerBound;
        }
        
        void BoundedIntegerVariable::setLowerBound(storm::expressions::Expression const& expression) {
            this->lowerBound = expression;
        }
        
        storm::expressions::Expression const& BoundedIntegerVariable::getUpperBound() const {
            return upperBound;
        }
        
        void BoundedIntegerVariable::setUpperBound(storm::expressions::Expression const& expression) {
            this->upperBound = expression;
        }
        
        storm::expressions::Expression BoundedIntegerVariable::getRangeExpression() const {
            return this->getLowerBound() <= this->getExpressionVariable() && this->getExpressionVariable() <= this->getUpperBound();
        }

        bool BoundedIntegerVariable::isBoundedIntegerVariable() const {
            return true;
        }
        
        std::shared_ptr<BoundedIntegerVariable> makeBoundedIntegerVariable(std::string const& name, storm::expressions::Variable const& variable, boost::optional<storm::expressions::Expression> initValue, bool transient, boost::optional<storm::expressions::Expression> lowerBound, boost::optional<storm::expressions::Expression> upperBound) {
            STORM_LOG_THROW(lowerBound && upperBound, storm::exceptions::NotImplementedException, "Jani Bounded Integer variables (for now) have to be bounded from both sides");
            if (initValue) {
                return std::make_shared<BoundedIntegerVariable>(name, variable, initValue.get(), transient, lowerBound.get(), upperBound.get());
            } else {
                return std::make_shared<BoundedIntegerVariable>(name, variable, transient, lowerBound.get(), upperBound.get());
            }
        }
    }
}