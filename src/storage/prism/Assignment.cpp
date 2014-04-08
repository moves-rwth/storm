#include "Assignment.h"

namespace storm {
    namespace prism {
        Assignment::Assignment(std::string const& variableName, storm::expressions::Expression const& expression, std::string const& filename, uint_fast64_t lineNumber) : LocatedInformation(filename, lineNumber), variableName(variableName), expression(expression) {
            // Intentionally left empty.
        }
        
        Assignment::Assignment(Assignment const& oldAssignment, std::map<std::string, std::string> const& renaming, std::string const& filename, uint_fast64_t lineNumber) : LocatedInformation(filename, lineNumber), variableName(oldAssignment.getVariableName()), expression(oldAssignment.getExpression().substitute<std::map>(renaming)) {
            auto renamingPair = renaming.find(oldAssignment.variableName);
            if (renamingPair != renaming.end()) {
                this->variableName = renamingPair->second;
            }
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
