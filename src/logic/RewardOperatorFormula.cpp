#include "src/logic/RewardOperatorFormula.h"

namespace storm {
    namespace logic {
        RewardOperatorFormula::RewardOperatorFormula(boost::optional<std::string> const& rewardModelName, std::shared_ptr<Formula const> const& subformula) : RewardOperatorFormula(rewardModelName, boost::optional<OptimalityType>(), boost::optional<ComparisonType>(), boost::optional<double>(), subformula) {
            // Intentionally left empty.
        }
        
        RewardOperatorFormula::RewardOperatorFormula(boost::optional<std::string> const& rewardModelName, ComparisonType comparisonType, double bound, std::shared_ptr<Formula const> const& subformula) : RewardOperatorFormula(rewardModelName, boost::optional<OptimalityType>(), boost::optional<ComparisonType>(comparisonType), boost::optional<double>(bound), subformula) {
            // Intentionally left empty.
        }
        
        RewardOperatorFormula::RewardOperatorFormula(boost::optional<std::string> const& rewardModelName, OptimalityType optimalityType, ComparisonType comparisonType, double bound, std::shared_ptr<Formula const> const& subformula) : RewardOperatorFormula(rewardModelName, boost::optional<OptimalityType>(optimalityType), boost::optional<ComparisonType>(comparisonType), boost::optional<double>(bound), subformula) {
            // Intentionally left empty.
        }
        
        RewardOperatorFormula::RewardOperatorFormula(boost::optional<std::string> const& rewardModelName, OptimalityType optimalityType, std::shared_ptr<Formula const> const& subformula) : RewardOperatorFormula(rewardModelName, boost::optional<OptimalityType>(optimalityType), boost::optional<ComparisonType>(), boost::optional<double>(), subformula) {
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
        
        bool RewardOperatorFormula::hasRewardModelName() const {
            return static_cast<bool>(this->rewardModelName);
        }
        
        std::string const& RewardOperatorFormula::getRewardModelName() const {
            return this->rewardModelName.get();
        }
        
        boost::optional<std::string> const& RewardOperatorFormula::getOptionalRewardModelName() const {
            return this->rewardModelName;
        }
        
        RewardOperatorFormula::RewardOperatorFormula(boost::optional<std::string> const& rewardModelName, boost::optional<OptimalityType> optimalityType, boost::optional<ComparisonType> comparisonType, boost::optional<double> bound, std::shared_ptr<Formula const> const& subformula) : OperatorFormula(optimalityType, comparisonType, bound, subformula), rewardModelName(rewardModelName) {
            // Intentionally left empty.
        }
        
        std::ostream& RewardOperatorFormula::writeToStream(std::ostream& out) const {
            out << "R";
            if (this->hasRewardModelName()) {
                out << "{\"" << this->getRewardModelName() << "\"}";
            }
            OperatorFormula::writeToStream(out);
            return out;
        }
    }
}