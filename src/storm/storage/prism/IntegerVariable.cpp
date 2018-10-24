#include "storm/storage/prism/IntegerVariable.h"

namespace storm {
    namespace prism {
        IntegerVariable::IntegerVariable(storm::expressions::Variable const& variable, storm::expressions::Expression const& lowerBoundExpression, storm::expressions::Expression const& upperBoundExpression, storm::expressions::Expression const& initialValueExpression, bool observable, std::string const& filename, uint_fast64_t lineNumber) : Variable(variable, initialValueExpression, observable, filename, lineNumber), lowerBoundExpression(lowerBoundExpression), upperBoundExpression(upperBoundExpression) {
            // Intentionally left empty.
        }
        
        storm::expressions::Expression const& IntegerVariable::getLowerBoundExpression() const {
            return this->lowerBoundExpression;
        }
        
        storm::expressions::Expression const& IntegerVariable::getUpperBoundExpression() const {
            return this->upperBoundExpression;
        }
        
        storm::expressions::Expression IntegerVariable::getRangeExpression() const {
            return this->getLowerBoundExpression() <= this->getExpressionVariable() && this->getExpressionVariable() <= this->getUpperBoundExpression();
        }
        
        IntegerVariable IntegerVariable::substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) const {
            return IntegerVariable(this->getExpressionVariable(), this->getLowerBoundExpression().substitute(substitution), this->getUpperBoundExpression().substitute(substitution), this->getInitialValueExpression().isInitialized() ? this->getInitialValueExpression().substitute(substitution) : this->getInitialValueExpression(), this->isObservable(), this->getFilename(), this->getLineNumber());
        }
        
        void IntegerVariable::createMissingInitialValue() {
            if (!this->hasInitialValue()) {
                this->setInitialValueExpression(this->getLowerBoundExpression());
            }
        }
        
        std::ostream& operator<<(std::ostream& stream, IntegerVariable const& variable) {
            stream << variable.getName() << ": [" << variable.getLowerBoundExpression() << ".." << variable.getUpperBoundExpression() << "]";
            if (variable.hasInitialValue()) {
                stream << " init " << variable.getInitialValueExpression();
            }
            stream << ";";
            return stream;
        }
    } // namespace prism
} // namespace storm
