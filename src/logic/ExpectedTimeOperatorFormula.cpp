#include "src/logic/ExpectedTimeOperatorFormula.h"

#include "src/logic/FormulaVisitor.h"

namespace storm {
    namespace logic {
        ExpectedTimeOperatorFormula::ExpectedTimeOperatorFormula(std::shared_ptr<Formula const> const& subformula) : ExpectedTimeOperatorFormula(boost::none, boost::none, subformula) {
            // Intentionally left empty.
        }
        
        ExpectedTimeOperatorFormula::ExpectedTimeOperatorFormula(Bound<double> const& bound, std::shared_ptr<Formula const> const& subformula) : ExpectedTimeOperatorFormula(boost::none, bound, subformula) {
            // Intentionally left empty.
        }
        
        ExpectedTimeOperatorFormula::ExpectedTimeOperatorFormula(OptimizationDirection optimalityType, Bound<double> const& bound, std::shared_ptr<Formula const> const& subformula) : ExpectedTimeOperatorFormula(boost::optional<OptimizationDirection>(optimalityType), bound, subformula) {
            // Intentionally left empty.
        }
        
        ExpectedTimeOperatorFormula::ExpectedTimeOperatorFormula(OptimizationDirection optimalityType, std::shared_ptr<Formula const> const& subformula) : ExpectedTimeOperatorFormula(boost::optional<OptimizationDirection>(optimalityType), boost::none, subformula) {
            // Intentionally left empty.
        }
        
        bool ExpectedTimeOperatorFormula::isExpectedTimeOperatorFormula() const {
            return true;
        }
        
        boost::any ExpectedTimeOperatorFormula::accept(FormulaVisitor const& visitor, boost::any const& data) const {
            return visitor.visit(*this, data);
        }
        
        ExpectedTimeOperatorFormula::ExpectedTimeOperatorFormula(boost::optional<OptimizationDirection> optimalityType, boost::optional<Bound<double>> bound, std::shared_ptr<Formula const> const& subformula) : OperatorFormula(optimalityType, bound, subformula) {
            // Intentionally left empty.
        }
        
        std::shared_ptr<Formula> ExpectedTimeOperatorFormula::substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) const {
            return std::make_shared<ExpectedTimeOperatorFormula>(this->optimalityType, this->bound, this->getSubformula().substitute(substitution));
        }
        
        std::ostream& ExpectedTimeOperatorFormula::writeToStream(std::ostream& out) const {
            out << "ET";
            OperatorFormula::writeToStream(out);
            return out;
        }
    }
}