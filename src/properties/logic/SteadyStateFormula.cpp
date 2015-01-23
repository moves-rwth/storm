#include "src/properties/logic/SteadyStateFormula.h"

namespace storm {
    namespace logic {
        SteadyStateFormula::SteadyStateFormula(std::shared_ptr<Formula> const& subformula) : SteadyStateFormula(boost::optional<ComparisonType>(), boost::optional<double>(), boost::optional<OptimalityType>(), subformula) {
            // Intentionally left empty.
        }
        
        SteadyStateFormula::SteadyStateFormula(ComparisonType comparisonType, double bound, std::shared_ptr<Formula> const& subformula) : SteadyStateFormula(boost::optional<ComparisonType>(comparisonType), boost::optional<double>(bound), boost::optional<OptimalityType>(), subformula) {
            // Intentionally left empty.
        }
        
        SteadyStateFormula::SteadyStateFormula(ComparisonType comparisonType, double bound, OptimalityType optimalityType, std::shared_ptr<Formula> const& subformula) : SteadyStateFormula(boost::optional<ComparisonType>(comparisonType), boost::optional<double>(bound), boost::optional<OptimalityType>(optimalityType), subformula) {
            // Intentionally left empty.
        }
        
        SteadyStateFormula::SteadyStateFormula(OptimalityType optimalityType, std::shared_ptr<Formula> const& subformula) : SteadyStateFormula(boost::optional<ComparisonType>(), boost::optional<double>(), boost::optional<OptimalityType>(optimalityType), subformula) {
            // Intentionally left empty.
        }
        
        bool SteadyStateFormula::isSteadyStateFormula() const {
            return true;
        }
        
        SteadyStateFormula::SteadyStateFormula(boost::optional<ComparisonType> comparisonType, boost::optional<double> bound, boost::optional<OptimalityType> optimalityType, std::shared_ptr<Formula> const& subformula) : OperatorFormula(comparisonType, bound, optimalityType, subformula) {
            // Intentionally left empty.
        }
        
        std::ostream& SteadyStateFormula::writeToStream(std::ostream& out) const {
            out << "S";
            OperatorFormula::writeToStream(out);
            return out;
        }
    }
}