#include "src/storage/prism/Formula.h"

namespace storm {
    namespace prism {
        Formula::Formula(std::string const& formulaName, storm::expressions::Expression const& expression, std::string const& filename, uint_fast64_t lineNumber) : LocatedInformation(filename, lineNumber), formulaName(formulaName), expression(expression) {
            // Intentionally left empty.
        }
        
        std::string const& Formula::getFormulaName() const {
            return this->formulaName;
        }
        
        storm::expressions::Expression const& Formula::getExpression() const {
            return this->expression;
        }
        
        storm::expressions::ExpressionReturnType Formula::getReturnType() const {
            return this->getExpression().getReturnType();
        }
        
        Formula Formula::substitute(std::map<std::string, storm::expressions::Expression> const& substitution) const {
            return Formula(this->getFormulaName(), this->getExpression().substitute<std::map>(substitution), this->getFilename(), this->getLineNumber());
        }
        
        std::ostream& operator<<(std::ostream& stream, Formula const& formula) {
            stream << "formula " << formula.getFormulaName() << " = " << formula.getExpression() << ";";
            return stream;
        }
    }
}