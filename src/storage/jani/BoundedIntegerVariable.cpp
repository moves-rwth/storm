#include "src/storage/jani/BoundedIntegerVariable.h"

namespace storm {
    namespace jani {
        
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
        
    }
}