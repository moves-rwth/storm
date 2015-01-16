#include "Assignment.h"

namespace storm {
    namespace prism {
        Assignment::Assignment(storm::expressions::Variable const& variable, storm::expressions::Expression const& expression, std::string const& filename, uint_fast64_t lineNumber) : LocatedInformation(filename, lineNumber), variable(variable), expression(expression) {
            // Intentionally left empty.
        }
        
        std::string const& Assignment::getVariableName() const {
            return variable.getName();
        }
        
        storm::expressions::Variable const& Assignment::getVariable() const {
            return variable;
        }
        
        storm::expressions::Expression const& Assignment::getExpression() const {
            return this->expression;
        }
        
        Assignment Assignment::substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) const {
            return Assignment(this->getVariable(), this->getExpression().substitute(substitution).simplify(), this->getFilename(), this->getLineNumber());
        }
        
        std::ostream& operator<<(std::ostream& stream, Assignment const& assignment) {
            stream << "(" << assignment.getVariableName() << "' = " << assignment.getExpression() << ")";
            return stream;
        }

    } // namespace prism
} // namespace storm
