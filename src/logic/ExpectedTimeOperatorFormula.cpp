#include "src/logic/ExpectedTimeOperatorFormula.h"

#include "src/logic/FormulaVisitor.h"

#include "src/utility/macros.h"
#include "src/exceptions/InvalidPropertyException.h"

namespace storm {
    namespace logic {
        ExpectedTimeOperatorFormula::ExpectedTimeOperatorFormula(std::shared_ptr<Formula const> const& subformula, OperatorInformation const& operatorInformation) : OperatorFormula(subformula, operatorInformation) {
            STORM_LOG_THROW(this->getMeasureType() == MeasureType::Expectation || this->getMeasureType() == MeasureType::Variance, storm::exceptions::InvalidPropertyException, "Invalid measure type in ET-operator.");
        }
        
        bool ExpectedTimeOperatorFormula::isExpectedTimeOperatorFormula() const {
            return true;
        }
        
        boost::any ExpectedTimeOperatorFormula::accept(FormulaVisitor const& visitor, boost::any const& data) const {
            return visitor.visit(*this, data);
        }
        
        std::shared_ptr<Formula> ExpectedTimeOperatorFormula::substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) const {
            return std::make_shared<ExpectedTimeOperatorFormula>(this->getSubformula().substitute(substitution), this->operatorInformation);
        }
        
        std::ostream& ExpectedTimeOperatorFormula::writeToStream(std::ostream& out) const {
            out << "ET";
            OperatorFormula::writeToStream(out);
            return out;
        }
    }
}