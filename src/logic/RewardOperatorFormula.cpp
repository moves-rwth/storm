#include "src/logic/RewardOperatorFormula.h"

namespace storm {
    namespace logic {
        RewardOperatorFormula::RewardOperatorFormula(std::shared_ptr<Formula const> const& subformula) : RewardOperatorFormula(boost::optional<OptimalityType>(), boost::optional<ComparisonType>(), boost::optional<double>(), subformula) {
            // Intentionally left empty.
        }
        
        RewardOperatorFormula::RewardOperatorFormula(ComparisonType comparisonType, double bound, std::shared_ptr<Formula const> const& subformula) : RewardOperatorFormula(boost::optional<OptimalityType>(), boost::optional<ComparisonType>(comparisonType), boost::optional<double>(bound), subformula) {
            // Intentionally left empty.
        }
        
        RewardOperatorFormula::RewardOperatorFormula(OptimalityType optimalityType, ComparisonType comparisonType, double bound, std::shared_ptr<Formula const> const& subformula) : RewardOperatorFormula(boost::optional<OptimalityType>(optimalityType), boost::optional<ComparisonType>(comparisonType), boost::optional<double>(bound), subformula) {
            // Intentionally left empty.
        }
        
        RewardOperatorFormula::RewardOperatorFormula(OptimalityType optimalityType, std::shared_ptr<Formula const> const& subformula) : RewardOperatorFormula(boost::optional<OptimalityType>(optimalityType), boost::optional<ComparisonType>(), boost::optional<double>(), subformula) {
            // Intentionally left empty.
        }
        
        bool RewardOperatorFormula::isRewardOperatorFormula() const {
            return true;
        }
        
        bool RewardOperatorFormula::isPctlStateFormula() const {
            return this->getSubformula().isRewardPathFormula();
        }
        
        bool RewardOperatorFormula::containsRewardOperator() const {
            return true;
        }
        
        bool RewardOperatorFormula::containsNestedRewardOperators() const {
            return this->getSubformula().containsRewardOperator();
        }
        
        RewardOperatorFormula::RewardOperatorFormula(boost::optional<OptimalityType> optimalityType, boost::optional<ComparisonType> comparisonType, boost::optional<double> bound, std::shared_ptr<Formula const> const& subformula) : OperatorFormula(optimalityType, comparisonType, bound, subformula) {
            // Intentionally left empty.
        }
        
        std::ostream& RewardOperatorFormula::writeToStream(std::ostream& out) const {
            out << "R";
            OperatorFormula::writeToStream(out);
            return out;
        }
    }
}