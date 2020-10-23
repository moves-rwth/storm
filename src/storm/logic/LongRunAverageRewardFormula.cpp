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
        
        std::shared_ptr<LongRunAverageRewardFormula const> LongRunAverageRewardFormula::stripRewardAccumulation() const {
            return std::make_shared<LongRunAverageRewardFormula>();
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
