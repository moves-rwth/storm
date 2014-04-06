#include "src/storage/prism/BooleanVariable.h"

namespace storm {
    namespace prism {
        BooleanVariable::BooleanVariable(std::string const& variableName) : BooleanVariable(variableName, storm::expressions::Expression::createFalse()) {
            // Nothing to do here.
        }

        BooleanVariable::BooleanVariable(std::string const& variableName, storm::expressions::Expression const& initialValueExpression) : Variable(variableName, initialValueExpression) {
            // Nothing to do here.
        }
        
        BooleanVariable::BooleanVariable(BooleanVariable const& oldVariable, std::string const& newName, std::map<std::string, std::string> const& renaming) : Variable(oldVariable, newName, renaming) {
            // Nothing to do here.
        }
        
        std::ostream& operator<<(std::ostream& stream, BooleanVariable const& variable) {
            stream << variable.getName() << ": bool " << variable.getInitialValueExpression() << ";";
            return stream;
        }
        
    } // namespace prism
} // namespace storm
