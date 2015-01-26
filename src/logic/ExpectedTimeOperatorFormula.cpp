#include "src/logic/ExpectedTimeOperatorFormula.h"

namespace storm {
    namespace logic {
        ExpectedTimeOperatorFormula::ExpectedTimeOperatorFormula(std::shared_ptr<Formula const> const& subformula) : ExpectedTimeOperatorFormula(boost::optional<OptimalityType>(), boost::optional<ComparisonType>(), boost::optional<double>(), subformula) {
            // Intentionally left empty.
        }
        
        ExpectedTimeOperatorFormula::ExpectedTimeOperatorFormula(ComparisonType comparisonType, double bound, std::shared_ptr<Formula const> const& subformula) : ExpectedTimeOperatorFormula(boost::optional<OptimalityType>(), boost::optional<ComparisonType>(comparisonType), boost::optional<double>(bound), subformula) {
            // Intentionally left empty.
        }
        
        ExpectedTimeOperatorFormula::ExpectedTimeOperatorFormula(OptimalityType optimalityType, ComparisonType comparisonType, double bound, std::shared_ptr<Formula const> const& subformula) : ExpectedTimeOperatorFormula(boost::optional<OptimalityType>(optimalityType), boost::optional<ComparisonType>(comparisonType), boost::optional<double>(bound), subformula) {
            // Intentionally left empty.
        }
        
        ExpectedTimeOperatorFormula::ExpectedTimeOperatorFormula(OptimalityType optimalityType, std::shared_ptr<Formula const> const& subformula) : ExpectedTimeOperatorFormula(boost::optional<OptimalityType>(optimalityType), boost::optional<ComparisonType>(), boost::optional<double>(), subformula) {
            // Intentionally left empty.
        }
        
        bool ExpectedTimeOperatorFormula::isExpectedTimeOperatorFormula() const {
            return true;
        }
        
        bool ExpectedTimeOperatorFormula::isPctlStateFormula() const {
            return this->getSubformula().isPctlStateFormula();
        }
        
        bool ExpectedTimeOperatorFormula::hasProbabilityOperator() const {
            return this->getSubformula().hasProbabilityOperator();
        }
        
        bool ExpectedTimeOperatorFormula::hasNestedProbabilityOperators() const {
            return this->getSubformula().hasNestedProbabilityOperators();
        }
        
        ExpectedTimeOperatorFormula::ExpectedTimeOperatorFormula(boost::optional<OptimalityType> optimalityType, boost::optional<ComparisonType> comparisonType, boost::optional<double> bound, std::shared_ptr<Formula const> const& subformula) : OperatorFormula(optimalityType, comparisonType, bound, subformula) {
            // Intentionally left empty.
        }
        
        std::ostream& ExpectedTimeOperatorFormula::writeToStream(std::ostream& out) const {
            out << "ET";
            OperatorFormula::writeToStream(out);
            return out;
        }
    }
}