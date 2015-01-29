#ifndef STORM_LOGIC_INSTANTANEOUSREWARDFORMULA_H_
#define STORM_LOGIC_INSTANTANEOUSREWARDFORMULA_H_

#include "src/logic/RewardPathFormula.h"

namespace storm {
    namespace logic {
        class InstantaneousRewardFormula : public RewardPathFormula {
        public:
            InstantaneousRewardFormula(uint_fast64_t stepCount);
            
            virtual ~InstantaneousRewardFormula() {
                // Intentionally left empty.
            }
            
            virtual bool isInstantaneousRewardFormula() const override;
            
            virtual std::ostream& writeToStream(std::ostream& out) const override;
            
            uint_fast64_t getStepCount() const;
            
        private:
            uint_fast64_t stepCount;
        };
    }
}

#endif /* STORM_LOGIC_INSTANTANEOUSREWARDFORMULA_H_ */