#include <map>

#include "src/storage/prism/Variable.h"

namespace storm {
    namespace prism {
        Variable::Variable(std::string const& variableName, storm::expressions::Expression const& initialValueExpression) : variableName(variableName), initialValueExpression(initialValueExpression) {
            // Nothing to do here.
        }
        
        Variable::Variable(Variable const& oldVariable, std::string const& newName, std::map<std::string, std::string> const& renaming) : variableName(newName), initialValueExpression(oldVariable.getInitialValueExpression().substitute<std::map>(renaming)) {
            // Intentionally left empty.
        }
        
        std::string const& Variable::getName() const {
            return variableName;
        }
        
        storm::expressions::Expression const& Variable::getInitialValueExpression() const {
            return this->initialValueExpression;
        }
    } // namespace prism
} // namespace storm
