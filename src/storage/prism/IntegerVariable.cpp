#include "src/storage/prism/IntegerVariable.h"

namespace storm {
    namespace prism {
        IntegerVariable::IntegerVariable(std::string const& variableName, storm::expressions::Expression const& lowerBoundExpression, storm::expressions::Expression const& upperBoundExpression) : IntegerVariable(variableName, lowerBoundExpression, upperBoundExpression, lowerBoundExpression) {
            // Intentionally left empty.
        }

        IntegerVariable::IntegerVariable(std::string const& variableName, storm::expressions::Expression const& lowerBoundExpression, storm::expressions::Expression const& upperBoundExpression, storm::expressions::Expression const& initialValueExpression) : Variable(variableName, initialValueExpression), lowerBoundExpression(lowerBoundExpression), upperBoundExpression(upperBoundExpression) {
            // Intentionally left empty.
        }
        
        IntegerVariable::IntegerVariable(IntegerVariable const& oldVariable, std::string const& newName, std::map<std::string, std::string> const& renaming) : Variable(oldVariable, newName, renaming), lowerBoundExpression(oldVariable.getLowerBoundExpression().substitute<std::map>(renaming)), upperBoundExpression(oldVariable.getUpperBoundExpression().substitute<std::map>(renaming)) {
            // Intentionally left empty.
        }
        
        storm::expressions::Expression const& IntegerVariable::getLowerBoundExpression() const {
            return this->lowerBoundExpression;
        }
        
        storm::expressions::Expression const& IntegerVariable::getUpperBoundExpression() const {
            return this->upperBoundExpression;
        }
        
        std::ostream& operator<<(std::ostream& stream, IntegerVariable const& variable) {
            stream << this->getName() << ": [" << variable.getLowerBoundExpression() << ".." << variable.getUpperBoundExpression() << "]" << variable.getInitialValueExpression() << ";";
            return stream;
        }
    } // namespace prism
} // namespace storm
