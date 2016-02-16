#include "src/logic/LongRunAverageOperatorFormula.h"

namespace storm {
    namespace logic {
        LongRunAverageOperatorFormula::LongRunAverageOperatorFormula(std::shared_ptr<Formula const> const& subformula) : LongRunAverageOperatorFormula(boost::none, boost::none, subformula) {
            // Intentionally left empty.
        }
        
        LongRunAverageOperatorFormula::LongRunAverageOperatorFormula(Bound<double> const& bound, std::shared_ptr<Formula const> const& subformula) : LongRunAverageOperatorFormula(boost::none, bound, subformula) {
            // Intentionally left empty.
        }
        
        LongRunAverageOperatorFormula::LongRunAverageOperatorFormula(OptimizationDirection optimalityType, Bound<double> const& bound, std::shared_ptr<Formula const> const& subformula) : LongRunAverageOperatorFormula(boost::optional<OptimizationDirection>(optimalityType), bound, subformula) {
            // Intentionally left empty.
        }
        
        LongRunAverageOperatorFormula::LongRunAverageOperatorFormula(OptimizationDirection optimalityType, std::shared_ptr<Formula const> const& subformula) : LongRunAverageOperatorFormula(boost::optional<OptimizationDirection>(optimalityType), boost::none, subformula) {
            // Intentionally left empty.
        }
                
        bool LongRunAverageOperatorFormula::isLongRunAverageOperatorFormula() const {
            return true;
        }
        
        bool LongRunAverageOperatorFormula::isPctlStateFormula() const {
            return this->getSubformula().isPctlStateFormula();
        }
        
        bool LongRunAverageOperatorFormula::isPctlWithConditionalStateFormula() const {
            return this->getSubformula().isPctlWithConditionalStateFormula();
        }
        
        bool LongRunAverageOperatorFormula::containsProbabilityOperator() const {
            return this->getSubformula().containsProbabilityOperator();
        }
        
        bool LongRunAverageOperatorFormula::containsNestedProbabilityOperators() const {
            return this->getSubformula().containsNestedProbabilityOperators();
        }
        
        LongRunAverageOperatorFormula::LongRunAverageOperatorFormula(boost::optional<OptimizationDirection> optimalityType, boost::optional<Bound<double>> bound, std::shared_ptr<Formula const> const& subformula) : OperatorFormula(optimalityType, bound, subformula) {
            // Intentionally left empty.
        }
        
        std::shared_ptr<Formula> LongRunAverageOperatorFormula::substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) const {
            return std::make_shared<LongRunAverageOperatorFormula>(this->optimalityType, this->bound, this->getSubformula().substitute(substitution));
        }
        
        std::ostream& LongRunAverageOperatorFormula::writeToStream(std::ostream& out) const {
            out << "LRA";
            OperatorFormula::writeToStream(out);
            return out;
        }
    }
}