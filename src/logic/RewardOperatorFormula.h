#ifndef STORM_LOGIC_REWARDOPERATORFORMULA_H_
#define STORM_LOGIC_REWARDOPERATORFORMULA_H_

#include "src/logic/OperatorFormula.h"

namespace storm {
    namespace logic {
        class RewardOperatorFormula : public OperatorFormula {
        public:
            RewardOperatorFormula(std::shared_ptr<Formula const> const& subformula);
            RewardOperatorFormula(ComparisonType comparisonType, double bound, std::shared_ptr<Formula const> const& subformula);
            RewardOperatorFormula(OptimalityType optimalityType, ComparisonType comparisonType, double bound, std::shared_ptr<Formula const> const& subformula);
            RewardOperatorFormula(OptimalityType optimalityType, std::shared_ptr<Formula const> const& subformula);
            RewardOperatorFormula(boost::optional<OptimalityType> optimalityType, boost::optional<ComparisonType> comparisonType, boost::optional<double> bound, std::shared_ptr<Formula const> const& subformula);

            virtual ~RewardOperatorFormula() {
                // Intentionally left empty.
            }
            
            virtual bool isRewardOperatorFormula() const override;
            
            virtual std::ostream& writeToStream(std::ostream& out) const override;
        };
    }
}

#endif /* STORM_LOGIC_REWARDOPERATORFORMULA_H_ */