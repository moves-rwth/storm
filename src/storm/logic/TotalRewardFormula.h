#ifndef STORM_LOGIC_TOTALREWARDFORMULA_H_
#define STORM_LOGIC_TOTALREWARDFORMULA_H_

#include <boost/optional.hpp>

#include "storm/logic/RewardAccumulation.h"
#include "storm/logic/PathFormula.h"

namespace storm {
    namespace logic {
        class TotalRewardFormula : public PathFormula {
        public:
            TotalRewardFormula(boost::optional<RewardAccumulation> rewardAccumulation = boost::none);
            
            virtual ~TotalRewardFormula() {
                // Intentionally left empty.
            }

            virtual bool isTotalRewardFormula() const override;
            virtual bool isRewardPathFormula() const override;
            bool hasRewardAccumulation() const;
            RewardAccumulation const& getRewardAccumulation() const;

            virtual boost::any accept(FormulaVisitor const& visitor, boost::any const& data) const override;

            virtual std::ostream& writeToStream(std::ostream& out) const override;

        private:
            boost::optional<RewardAccumulation> rewardAccumulation;
        };
    }
}

#endif /* STORM_LOGIC_TOTALREWARDFORMULA_H_ */
