#ifndef STORM_LOGIC_REACHABILITYREWARDFORMULA_H_
#define STORM_LOGIC_REACHABILITYREWARDFORMULA_H_

#include <memory>

#include "src/logic/RewardPathFormula.h"


namespace storm {
    namespace logic {
        class ReachabilityRewardFormula : public RewardPathFormula {
        public:
            ReachabilityRewardFormula(std::shared_ptr<Formula const> const& subformula);
            
            virtual ~ReachabilityRewardFormula() {
                // Intentionally left empty.
            }
            
            virtual bool isReachabilityRewardFormula() const override;
            
            Formula const& getSubformula() const;
            
            virtual void gatherAtomicExpressionFormulas(std::vector<std::shared_ptr<AtomicExpressionFormula const>>& atomicExpressionFormulas) const override;
            virtual void gatherAtomicLabelFormulas(std::vector<std::shared_ptr<AtomicLabelFormula const>>& atomicLabelFormulas) const override;
            
            virtual std::ostream& writeToStream(std::ostream& out) const override;
            
        private:
            std::shared_ptr<Formula const> subformula;
        };
    }
}

#endif /* STORM_LOGIC_REACHABILITYREWARDFORMULA_H_ */