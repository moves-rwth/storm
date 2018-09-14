#include "storm/storage/prism/Formula.h"

namespace storm {
    namespace prism {
        Formula::Formula(storm::expressions::Variable const& variable, storm::expressions::Expression const& expression, std::string const& filename, uint_fast64_t lineNumber) : LocatedInformation(filename, lineNumber), variable(variable), expression(expression) {
            // Intentionally left empty.
        }
        
        std::string const& Formula::getName() const {
            return this->variable.getName();
        }
        
        storm::expressions::Variable const& Formula::getExpressionVariable() const {
            return this->variable;
        }
        
        storm::expressions::Expression const& Formula::getExpression() const {
            return this->expression;
        }
        
        storm::expressions::Type const& Formula::getType() const {
            assert(this->getExpressionVariable().getType() ==  this->getExpression().getType());
            return this->getExpressionVariable().getType();
        }
        
        Formula Formula::substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) const {
            return Formula(this->getExpressionVariable(), this->getExpression().substitute(substitution), this->getFilename(), this->getLineNumber());
        }
        
        std::ostream& operator<<(std::ostream& stream, Formula const& formula) {
            stream << "formula " << formula.getName() << " = " << formula.getExpression() << ";";
            return stream;
        }
    } // namespace prism
} // namespace storm
