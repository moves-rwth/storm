#include <map>

#include "src/storage/prism/Variable.h"

namespace storm {
    namespace prism {
        Variable::Variable(std::string const& variableName, storm::expressions::Expression const& initialValueExpression, bool defaultInitialValue, std::string const& filename, uint_fast64_t lineNumber) : LocatedInformation(filename, lineNumber), variableName(variableName), initialValueExpression(initialValueExpression), defaultInitialValue(defaultInitialValue) {
            // Nothing to do here.
        }
        
        Variable::Variable(Variable const& oldVariable, std::string const& newName, std::map<std::string, std::string> const& renaming, std::string const& filename, uint_fast64_t lineNumber) : LocatedInformation(filename, lineNumber), variableName(newName), initialValueExpression(oldVariable.getInitialValueExpression().substitute<std::map>(renaming)), defaultInitialValue(oldVariable.hasDefaultInitialValue()) {
            // Intentionally left empty.
        }
        
        std::string const& Variable::getName() const {
            return variableName;
        }
        
        bool Variable::hasDefaultInitialValue() const {
            return this->defaultInitialValue;
        }

        storm::expressions::Expression const& Variable::getInitialValueExpression() const {
            return this->initialValueExpression;
        }
    } // namespace prism
} // namespace storm
