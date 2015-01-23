#ifndef STORM_LOGIC_INSTANTANEOUSREWARDFORMULA_H_
#define STORM_LOGIC_INSTANTANEOUSREWARDFORMULA_H_

#include "src/logic/PathRewardFormula.h"

namespace storm {
    namespace logic {
        class InstantaneousRewardFormula : public PathRewardFormula {
        public:
            InstantaneousRewardFormula(uint_fast64_t stepCount);
            
            virtual ~InstantaneousRewardFormula() {
                // Intentionally left empty.
            }
            
            virtual bool isInstantaneousRewardFormula() const override;
            
            virtual std::ostream& writeToStream(std::ostream& out) const override;
            
        private:
            uint_fast64_t stepCount;
        };
    }
}

#endif /* STORM_LOGIC_INSTANTANEOUSREWARDFORMULA_H_ */