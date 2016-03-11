#include "src/logic/RewardOperatorFormula.h"

#include "src/logic/FormulaVisitor.h"

#include "src/utility/macros.h"
#include "src/exceptions/InvalidPropertyException.h"

namespace storm {
    namespace logic {
        RewardOperatorFormula::RewardOperatorFormula(std::shared_ptr<Formula const> const& subformula, boost::optional<std::string> const& rewardModelName, OperatorInformation const& operatorInformation, RewardMeasureType rewardMeasureType) : OperatorFormula(subformula, operatorInformation), rewardModelName(rewardModelName), rewardMeasureType(rewardMeasureType) {
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
        
        bool RewardOperatorFormula::hasRewardModelName() const {
            return static_cast<bool>(rewardModelName);
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
        
        std::shared_ptr<Formula> RewardOperatorFormula::substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) const {
            return std::make_shared<RewardOperatorFormula>(this->getSubformula().substitute(substitution), this->rewardModelName, this->operatorInformation);
        }
        
        RewardMeasureType RewardOperatorFormula::getMeasureType() const {
            return rewardMeasureType;
        }
        
        std::ostream& RewardOperatorFormula::writeToStream(std::ostream& out) const {
            out << "R";
            out << "[" << rewardMeasureType << "]";
            if (this->hasRewardModelName()) {
                out << "{\"" << this->getRewardModelName() << "\"}";
            }
            OperatorFormula::writeToStream(out);
            return out;
        }
    }
}