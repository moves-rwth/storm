#include "src/properties/logic/RewardOperatorFormula.h"

namespace storm {
    namespace logic {
        RewardOperatorFormula::RewardOperatorFormula(std::shared_ptr<Formula> const& subformula) : RewardOperatorFormula(boost::optional<ComparisonType>(), boost::optional<double>(), boost::optional<OptimalityType>(), subformula) {
            // Intentionally left empty.
        }
        
        RewardOperatorFormula::RewardOperatorFormula(ComparisonType comparisonType, double bound, std::shared_ptr<Formula> const& subformula) : RewardOperatorFormula(boost::optional<ComparisonType>(comparisonType), boost::optional<double>(bound), boost::optional<OptimalityType>(), subformula) {
            // Intentionally left empty.
        }
        
        RewardOperatorFormula::RewardOperatorFormula(ComparisonType comparisonType, double bound, OptimalityType optimalityType, std::shared_ptr<Formula> const& subformula) : RewardOperatorFormula(boost::optional<ComparisonType>(comparisonType), boost::optional<double>(bound), boost::optional<OptimalityType>(optimalityType), subformula) {
            // Intentionally left empty.
        }
        
        RewardOperatorFormula::RewardOperatorFormula(OptimalityType optimalityType, std::shared_ptr<Formula> const& subformula) : RewardOperatorFormula(boost::optional<ComparisonType>(), boost::optional<double>(), boost::optional<OptimalityType>(optimalityType), subformula) {
            // Intentionally left empty.
        }
        
        bool RewardOperatorFormula::isRewardOperator() const {
            return true;
        }
        
        RewardOperatorFormula::RewardOperatorFormula(boost::optional<ComparisonType> comparisonType, boost::optional<double> bound, boost::optional<OptimalityType> optimalityType, std::shared_ptr<Formula> const& subformula) : OperatorFormula(comparisonType, bound, optimalityType, subformula) {
            // Intentionally left empty.
        }
        
        std::ostream& RewardOperatorFormula::writeToStream(std::ostream& out) const {
            out << "R";
            OperatorFormula::writeToStream(out);
            return out;
        }
    }
}