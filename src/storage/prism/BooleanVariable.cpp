#include "src/storage/prism/BooleanVariable.h"

namespace storm {
    namespace prism {
        BooleanVariable::BooleanVariable(std::string const& variableName, std::string const& filename, uint_fast64_t lineNumber) : Variable(variableName, storm::expressions::Expression::createFalse(), true, filename, lineNumber) {
            // Nothing to do here.
        }

        BooleanVariable::BooleanVariable(std::string const& variableName, storm::expressions::Expression const& initialValueExpression, std::string const& filename, uint_fast64_t lineNumber) : Variable(variableName, initialValueExpression, false, filename, lineNumber) {
            // Nothing to do here.
        }
        
        BooleanVariable BooleanVariable::substitute(std::map<std::string, storm::expressions::Expression> const& substitution) const {
            return BooleanVariable(this->getName(), this->getInitialValueExpression().substitute<std::map>(substitution), this->getFilename(), this->getLineNumber());
        }
        
        std::ostream& operator<<(std::ostream& stream, BooleanVariable const& variable) {
            stream << variable.getName() << ": bool " << variable.getInitialValueExpression() << ";";
            return stream;
        }
        
    } // namespace prism
} // namespace storm
