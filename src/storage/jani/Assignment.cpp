#include "src/storage/jani/Assignment.h"

namespace storm  {
    namespace jani {
        
        Assignment::Assignment(storm::expressions::Variable const& variable, storm::expressions::Expression const& expression) : variable(variable), expression(expression) {
            // Intentionally left empty.
        }
        
        storm::expressions::Variable const& Assignment::getExpressionVariable() const {
            return variable;
        }
        
        storm::expressions::Expression const& Assignment::getAssignedExpression() const {
            return expression;
        }
        
        std::ostream& operator<<(std::ostream& stream, Assignment const& assignment) {
            stream << assignment.getExpressionVariable().getName() << " := " << assignment.getAssignedExpression();
            return stream;
        }
        
    }
}