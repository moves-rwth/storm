#ifndef STORM_LOGIC_PATHREWARDFORMULA_H_
#define STORM_LOGIC_PATHREWARDFORMULA_H_

#include "src/logic/PathFormula.h"

namespace storm {
    namespace logic {
        class RewardPathFormula : public Formula {
        public:
            virtual ~RewardPathFormula() {
                // Intentionally left empty.
            }
            
            virtual bool isRewardPathFormula() const override;
        };
    }
}

#endif /* STORM_LOGIC_PATHREWARDFORMULA_H_ */