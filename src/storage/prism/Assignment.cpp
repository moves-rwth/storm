#include "Assignment.h"

namespace storm {
    namespace prism {
        Assignment::Assignment(std::string const& variableName, storm::expressions::Expression const& expression, std::string const& filename, uint_fast64_t lineNumber) : LocatedInformation(filename, lineNumber), variableName(variableName), expression(expression) {
            // Intentionally left empty.
        }
        
        std::string const& Assignment::getVariableName() const {
            return variableName;
        }
        
        storm::expressions::Expression const& Assignment::getExpression() const {
            return this->expression;
        }
        
        std::ostream& operator<<(std::ostream& stream, Assignment const& assignment) {
            stream << "(" << assignment.getVariableName() << "' = " << assignment.getExpression() << ")";
            return stream;
        }

    } // namespace prism
} // namespace storm
