#include "src/logic/RewardOperatorFormula.h"

#include "src/logic/FormulaVisitor.h"

namespace storm {
    namespace logic {
        RewardOperatorFormula::RewardOperatorFormula(boost::optional<std::string> const& rewardModelName, std::shared_ptr<Formula const> const& subformula) : RewardOperatorFormula(rewardModelName, boost::none, boost::none, subformula) {
            // Intentionally left empty.
        }
        
        RewardOperatorFormula::RewardOperatorFormula(boost::optional<std::string> const& rewardModelName, Bound<double> const& bound, std::shared_ptr<Formula const> const& subformula) : RewardOperatorFormula(rewardModelName, boost::none, bound, subformula) {
            // Intentionally left empty.
        }
        
        RewardOperatorFormula::RewardOperatorFormula(boost::optional<std::string> const& rewardModelName, OptimizationDirection optimalityType, Bound<double> const& bound, std::shared_ptr<Formula const> const& subformula) : RewardOperatorFormula(rewardModelName, boost::optional<OptimizationDirection>(optimalityType), bound, subformula) {
            // Intentionally left empty.
        }
        
        RewardOperatorFormula::RewardOperatorFormula(boost::optional<std::string> const& rewardModelName, OptimizationDirection optimalityType, std::shared_ptr<Formula const> const& subformula) : RewardOperatorFormula(rewardModelName, boost::optional<OptimizationDirection>(optimalityType), boost::none, subformula) {
            // Intentionally left empty.
        }
        
        bool RewardOperatorFormula::isRewardOperatorFormula() const {
            return true;
        }
        
        boost::any RewardOperatorFormula::accept(FormulaVisitor const& visitor, boost::any const& data) const {
            return visitor.visit(*this, data);
        }
        
        std::string const& RewardOperatorFormula::getRewardModelName() const {
            return this->rewardModelName.get();
        }
        
        boost::optional<std::string> const& RewardOperatorFormula::getOptionalRewardModelName() const {
            return this->rewardModelName;
        }
        
        void RewardOperatorFormula::gatherReferencedRewardModels(std::set<std::string>& referencedRewardModels) const {
            if (this->hasRewardModelName()) {
                referencedRewardModels.insert(this->getRewardModelName());
            } else {
                referencedRewardModels.insert("");
            }
            this->getSubformula().gatherReferencedRewardModels(referencedRewardModels);
        }
        
        RewardOperatorFormula::RewardOperatorFormula(boost::optional<std::string> const& rewardModelName, boost::optional<OptimizationDirection> optimalityType, boost::optional<Bound<double>> bound, std::shared_ptr<Formula const> const& subformula) : OperatorFormula(optimalityType, bound, subformula), rewardModelName(rewardModelName) {
            // Intentionally left empty.
        }
        
        std::shared_ptr<Formula> RewardOperatorFormula::substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) const {
            return std::make_shared<RewardOperatorFormula>(this->rewardModelName, this->optimalityType, this->bound, this->getSubformula().substitute(substitution));
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