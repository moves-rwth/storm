#include "src/storm/storage/prism/Formula.h"

namespace storm {
    namespace prism {
        Formula::Formula(std::string const& name, storm::expressions::Expression const& expression, std::string const& filename, uint_fast64_t lineNumber) : LocatedInformation(filename, lineNumber), name(name), expression(expression) {
            // Intentionally left empty.
        }
        
        std::string const& Formula::getName() const {
            return this->name;
        }
        
        storm::expressions::Expression const& Formula::getExpression() const {
            return this->expression;
        }
        
        storm::expressions::Type const& Formula::getType() const {
            return this->getExpression().getType();
        }
        
        Formula Formula::substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) const {
            return Formula(this->getName(), this->getExpression().substitute(substitution), this->getFilename(), this->getLineNumber());
        }
        
        std::ostream& operator<<(std::ostream& stream, Formula const& formula) {
            stream << "formula " << formula.getName() << " = " << formula.getExpression() << ";";
            return stream;
        }
    } // namespace prism
} // namespace storm
