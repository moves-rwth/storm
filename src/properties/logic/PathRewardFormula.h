#ifndef STORM_LOGIC_PATHREWARDFORMULA_H_
#define STORM_LOGIC_PATHREWARDFORMULA_H_

#include "src/properties/logic/PathFormula.h"

namespace storm {
    namespace logic {
        class PathRewardFormula : public PathFormula {
        public:
            virtual ~PathRewardFormula() {
                // Intentionally left empty.
            }
            
            virtual bool isPathRewardFormula() const override;
        };
    }
}

#endif /* STORM_LOGIC_PATHREWARDFORMULA_H_ */