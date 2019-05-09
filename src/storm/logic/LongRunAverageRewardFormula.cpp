#include "storm/logic/LongRunAverageRewardFormula.h"

#include "storm/logic/FormulaVisitor.h"

namespace storm {
    namespace logic {
        LongRunAverageRewardFormula::LongRunAverageRewardFormula(boost::optional<RewardAccumulation> rewardAccumulation) : rewardAccumulation(rewardAccumulation) {
            // Intentionally left empty.
        }
        
        bool LongRunAverageRewardFormula::isLongRunAverageRewardFormula() const {
            return true;
        }
        
        bool LongRunAverageRewardFormula::isRewardPathFormula() const {
            return true;
        }
        
        bool LongRunAverageRewardFormula::hasRewardAccumulation() const {
            return rewardAccumulation.is_initialized();
        }
        
        RewardAccumulation const& LongRunAverageRewardFormula::getRewardAccumulation() const {
            return rewardAccumulation.get();
        }
        
        boost::any LongRunAverageRewardFormula::accept(FormulaVisitor const& visitor, boost::any const& data) const {
            return visitor.visit(*this, data);
        }
        
        std::ostream& LongRunAverageRewardFormula::writeToStream(std::ostream& out) const {
            out << "LRA";
            if (hasRewardAccumulation()) {
                out << "[" << getRewardAccumulation() << "]";
            }
            return out;
        }
        
    }
}
