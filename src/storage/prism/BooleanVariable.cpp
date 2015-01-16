#include "src/storage/prism/BooleanVariable.h"

namespace storm {
    namespace prism {
        BooleanVariable::BooleanVariable(storm::expressions::Variable const& variable, storm::expressions::Expression const& initialValueExpression, std::string const& filename, uint_fast64_t lineNumber) : Variable(variable, initialValueExpression, false, filename, lineNumber) {
            // Nothing to do here.
        }
        
        BooleanVariable BooleanVariable::substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) const {
            return BooleanVariable(this->getExpressionVariable(), this->getInitialValueExpression().substitute(substitution), this->getFilename(), this->getLineNumber());
        }
        
        std::ostream& operator<<(std::ostream& stream, BooleanVariable const& variable) {
            stream << variable.getName() << ": bool " << variable.getInitialValueExpression() << ";";
            return stream;
        }
        
    } // namespace prism
} // namespace storm
