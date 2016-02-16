#include "src/logic/ProbabilityOperatorFormula.h"

namespace storm {
    namespace logic {
        ProbabilityOperatorFormula::ProbabilityOperatorFormula(std::shared_ptr<Formula const> const& subformula) : ProbabilityOperatorFormula(boost::none, boost::none, subformula) {
            // Intentionally left empty.
        }
        
        ProbabilityOperatorFormula::ProbabilityOperatorFormula(Bound<double> const& bound, std::shared_ptr<Formula const> const& subformula) : ProbabilityOperatorFormula(boost::none, bound, subformula) {
            // Intentionally left empty.
        }
        
        ProbabilityOperatorFormula::ProbabilityOperatorFormula(OptimizationDirection optimalityType, Bound<double> const& bound, std::shared_ptr<Formula const> const& subformula) : ProbabilityOperatorFormula(boost::optional<OptimizationDirection>(optimalityType), bound, subformula) {
            // Intentionally left empty.
        }
        
        ProbabilityOperatorFormula::ProbabilityOperatorFormula(OptimizationDirection optimalityType, std::shared_ptr<Formula const> const& subformula) : ProbabilityOperatorFormula(boost::optional<OptimizationDirection>(optimalityType), boost::none, subformula) {
            // Intentionally left empty.
        }
        
        bool ProbabilityOperatorFormula::isProbabilityOperatorFormula() const {
            return true;
        }
        
        bool ProbabilityOperatorFormula::isPctlStateFormula() const {
            return this->getSubformula().isPctlPathFormula();
        }
        
        bool ProbabilityOperatorFormula::isCslStateFormula() const {
            return this->getSubformula().isCslPathFormula();
        }
        
        bool ProbabilityOperatorFormula::isPltlFormula() const {
            return this->getSubformula().isLtlFormula();
        }
        
        bool ProbabilityOperatorFormula::containsProbabilityOperator() const {
            return true;
        }
        
        bool ProbabilityOperatorFormula::containsNestedProbabilityOperators() const {
            return this->getSubformula().containsProbabilityOperator();
        }
        
        ProbabilityOperatorFormula::ProbabilityOperatorFormula(boost::optional<OptimizationDirection> optimalityType, boost::optional<Bound<double>> bound, std::shared_ptr<Formula const> const& subformula) : OperatorFormula(optimalityType, bound, subformula) {
            // Intentionally left empty.
        }
        
        std::shared_ptr<Formula> ProbabilityOperatorFormula::substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) const {
            return std::make_shared<ProbabilityOperatorFormula>(this->optimalityType, this->bound, this->getSubformula().substitute(substitution));
        }
        
        std::ostream& ProbabilityOperatorFormula::writeToStream(std::ostream& out) const {
            out << "P";
            OperatorFormula::writeToStream(out);
            return out;
        }
    }
}