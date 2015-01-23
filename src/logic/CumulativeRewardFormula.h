#ifndef STORM_LOGIC_CUMULATIVEREWARDFORMULA_H_
#define STORM_LOGIC_CUMULATIVEREWARDFORMULA_H_

#include "src/logic/PathRewardFormula.h"

namespace storm {
    namespace logic {
        class CumulativeRewardFormula : public PathRewardFormula {
        public:
            CumulativeRewardFormula(uint_fast64_t stepBound);
            
            virtual ~CumulativeRewardFormula() {
                // Intentionally left empty.
            }
            
            virtual bool isCumulativeRewardFormula() const override;
            
            virtual std::ostream& writeToStream(std::ostream& out) const override;
            
        private:
            uint_fast64_t stepBound;
        };
    }
}

#endif /* STORM_LOGIC_CUMULATIVEREWARDFORMULA_H_ */