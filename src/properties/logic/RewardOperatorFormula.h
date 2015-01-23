#ifndef STORM_LOGIC_REWARDOPERATORFORMULA_H_
#define STORM_LOGIC_REWARDOPERATORFORMULA_H_

#include "src/properties/logic/OperatorFormula.h"

namespace storm {
    namespace logic {
        class RewardOperatorFormula : public OperatorFormula {
        public:
            RewardOperatorFormula(std::shared_ptr<Formula> const& subformula);
            RewardOperatorFormula(ComparisonType comparisonType, double bound, std::shared_ptr<Formula> const& subformula);
            RewardOperatorFormula(ComparisonType comparisonType, double bound, OptimalityType optimalityType, std::shared_ptr<Formula> const& subformula);
            RewardOperatorFormula(OptimalityType optimalityType, std::shared_ptr<Formula> const& subformula);
            
            virtual ~RewardOperatorFormula() {
                // Intentionally left empty.
            }
            
            virtual bool isRewardOperator() const override;
            
            virtual std::ostream& writeToStream(std::ostream& out) const override;
            
        private:
            RewardOperatorFormula(boost::optional<ComparisonType> comparisonType, boost::optional<double> bound, boost::optional<OptimalityType> optimalityType, std::shared_ptr<Formula> const& subformula);
        };
    }
}

#endif /* STORM_LOGIC_REWARDOPERATORFORMULA_H_ */