#include "src/logic/ExpectedTimeOperatorFormula.h"

namespace storm {
    namespace logic {
        ExpectedTimeOperatorFormula::ExpectedTimeOperatorFormula(std::shared_ptr<Formula const> const& subformula) : ExpectedTimeOperatorFormula(boost::optional<OptimizationDirection>(), boost::optional<ComparisonType>(), boost::optional<double>(), subformula) {
            // Intentionally left empty.
        }
        
        ExpectedTimeOperatorFormula::ExpectedTimeOperatorFormula(ComparisonType comparisonType, double bound, std::shared_ptr<Formula const> const& subformula) : ExpectedTimeOperatorFormula(boost::optional<OptimizationDirection>(), boost::optional<ComparisonType>(comparisonType), boost::optional<double>(bound), subformula) {
            // Intentionally left empty.
        }
        
        ExpectedTimeOperatorFormula::ExpectedTimeOperatorFormula(OptimizationDirection optimalityType, ComparisonType comparisonType, double bound, std::shared_ptr<Formula const> const& subformula) : ExpectedTimeOperatorFormula(boost::optional<OptimizationDirection>(optimalityType), boost::optional<ComparisonType>(comparisonType), boost::optional<double>(bound), subformula) {
            // Intentionally left empty.
        }
        
        ExpectedTimeOperatorFormula::ExpectedTimeOperatorFormula(OptimizationDirection optimalityType, std::shared_ptr<Formula const> const& subformula) : ExpectedTimeOperatorFormula(boost::optional<OptimizationDirection>(optimalityType), boost::optional<ComparisonType>(), boost::optional<double>(), subformula) {
            // Intentionally left empty.
        }
        
        bool ExpectedTimeOperatorFormula::isExpectedTimeOperatorFormula() const {
            return true;
        }
        
        bool ExpectedTimeOperatorFormula::isPctlStateFormula() const {
            return this->getSubformula().isPctlStateFormula();
        }
        
        bool ExpectedTimeOperatorFormula::containsProbabilityOperator() const {
            return this->getSubformula().containsProbabilityOperator();
        }
        
        bool ExpectedTimeOperatorFormula::containsNestedProbabilityOperators() const {
            return this->getSubformula().containsNestedProbabilityOperators();
        }
        
        ExpectedTimeOperatorFormula::ExpectedTimeOperatorFormula(boost::optional<OptimizationDirection> optimalityType, boost::optional<ComparisonType> comparisonType, boost::optional<double> bound, std::shared_ptr<Formula const> const& subformula) : OperatorFormula(optimalityType, comparisonType, bound, subformula) {
            // Intentionally left empty.
        }
        
        std::ostream& ExpectedTimeOperatorFormula::writeToStream(std::ostream& out) const {
            out << "ET";
            OperatorFormula::writeToStream(out);
            return out;
        }
    }
}