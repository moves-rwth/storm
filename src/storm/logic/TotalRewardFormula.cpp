#include "storm/logic/TotalRewardFormula.h"

#include "storm/logic/FormulaVisitor.h"

namespace storm {
    namespace logic {
        TotalRewardFormula::TotalRewardFormula(boost::optional<RewardAccumulation> rewardAccumulation) : rewardAccumulation(rewardAccumulation) {
            // Intentionally left empty.
        }
        
        bool TotalRewardFormula::isTotalRewardFormula() const {
            return true;
        }
        
        bool TotalRewardFormula::isRewardPathFormula() const {
            return true;
        }
        
        bool TotalRewardFormula::hasRewardAccumulation() const {
            return rewardAccumulation.is_initialized();
        }
        
        RewardAccumulation const& TotalRewardFormula::getRewardAccumulation() const {
            return rewardAccumulation.get();
        }
        
        boost::any TotalRewardFormula::accept(FormulaVisitor const& visitor, boost::any const& data) const {
            return visitor.visit(*this, data);
        }

        std::ostream& TotalRewardFormula::writeToStream(std::ostream& out) const {
            out << "C";
            if (hasRewardAccumulation()) {
                out << "[" << getRewardAccumulation() << "]";
            }
            return out;
        }
    }
}
