#include "src/properties/logic/ProbabilityOperatorFormula.h"

namespace storm {
    namespace logic {
        ProbabilityOperatorFormula::ProbabilityOperatorFormula(std::shared_ptr<Formula> const& subformula) : ProbabilityOperatorFormula(boost::optional<ComparisonType>(), boost::optional<double>(), boost::optional<OptimalityType>(), subformula) {
            // Intentionally left empty.
        }
        
        ProbabilityOperatorFormula::ProbabilityOperatorFormula(ComparisonType comparisonType, double bound, std::shared_ptr<Formula> const& subformula) : ProbabilityOperatorFormula(boost::optional<ComparisonType>(comparisonType), boost::optional<double>(bound), boost::optional<OptimalityType>(), subformula) {
            // Intentionally left empty.
        }
        
        ProbabilityOperatorFormula::ProbabilityOperatorFormula(ComparisonType comparisonType, double bound, OptimalityType optimalityType, std::shared_ptr<Formula> const& subformula) : ProbabilityOperatorFormula(boost::optional<ComparisonType>(comparisonType), boost::optional<double>(bound), boost::optional<OptimalityType>(optimalityType), subformula) {
            // Intentionally left empty.
        }
        
        ProbabilityOperatorFormula::ProbabilityOperatorFormula(OptimalityType optimalityType, std::shared_ptr<Formula> const& subformula) : ProbabilityOperatorFormula(boost::optional<ComparisonType>(), boost::optional<double>(), boost::optional<OptimalityType>(optimalityType), subformula) {
            // Intentionally left empty.
        }
        
        bool ProbabilityOperatorFormula::isProbabilityOperator() const {
            return true;
        }
        
        ProbabilityOperatorFormula::ProbabilityOperatorFormula(boost::optional<ComparisonType> comparisonType, boost::optional<double> bound, boost::optional<OptimalityType> optimalityType, std::shared_ptr<Formula> const& subformula) : OperatorFormula(comparisonType, bound, optimalityType, subformula) {
            // Intentionally left empty.
        }
        
        std::ostream& ProbabilityOperatorFormula::writeToStream(std::ostream& out) const {
            out << "P";
            OperatorFormula::writeToStream(out);
            return out;
        }
    }
}