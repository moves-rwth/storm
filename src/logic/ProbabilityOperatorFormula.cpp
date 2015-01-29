#include "src/logic/ProbabilityOperatorFormula.h"

namespace storm {
    namespace logic {
        ProbabilityOperatorFormula::ProbabilityOperatorFormula(std::shared_ptr<Formula const> const& subformula) : ProbabilityOperatorFormula(boost::optional<OptimalityType>(), boost::optional<ComparisonType>(), boost::optional<double>(), subformula) {
            // Intentionally left empty.
        }
        
        ProbabilityOperatorFormula::ProbabilityOperatorFormula(ComparisonType comparisonType, double bound, std::shared_ptr<Formula const> const& subformula) : ProbabilityOperatorFormula(boost::optional<OptimalityType>(), boost::optional<ComparisonType>(comparisonType), boost::optional<double>(bound), subformula) {
            // Intentionally left empty.
        }
        
        ProbabilityOperatorFormula::ProbabilityOperatorFormula(OptimalityType optimalityType, ComparisonType comparisonType, double bound, std::shared_ptr<Formula const> const& subformula) : ProbabilityOperatorFormula(boost::optional<OptimalityType>(optimalityType), boost::optional<ComparisonType>(comparisonType), boost::optional<double>(bound), subformula) {
            // Intentionally left empty.
        }
        
        ProbabilityOperatorFormula::ProbabilityOperatorFormula(OptimalityType optimalityType, std::shared_ptr<Formula const> const& subformula) : ProbabilityOperatorFormula(boost::optional<OptimalityType>(optimalityType), boost::optional<ComparisonType>(), boost::optional<double>(), subformula) {
            // Intentionally left empty.
        }
        
        bool ProbabilityOperatorFormula::isProbabilityOperatorFormula() const {
            return true;
        }
        
        bool ProbabilityOperatorFormula::isPctlStateFormula() const {
            return this->getSubformula().isPctlPathFormula();
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
        
        ProbabilityOperatorFormula::ProbabilityOperatorFormula(boost::optional<OptimalityType> optimalityType, boost::optional<ComparisonType> comparisonType, boost::optional<double> bound, std::shared_ptr<Formula const> const& subformula) : OperatorFormula(optimalityType, comparisonType, bound, subformula) {
            // Intentionally left empty.
        }
        
        std::ostream& ProbabilityOperatorFormula::writeToStream(std::ostream& out) const {
            out << "P";
            OperatorFormula::writeToStream(out);
            return out;
        }
    }
}