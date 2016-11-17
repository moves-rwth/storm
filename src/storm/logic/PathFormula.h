#ifndef STORM_LOGIC_PATHFORMULA_H_
#define STORM_LOGIC_PATHFORMULA_H_

#include "storm/logic/Formula.h"

namespace storm {
    namespace logic {
        class PathFormula : public Formula {
        public:
            virtual ~PathFormula() {
                // Intentionally left empty.
            };
            
            virtual bool isPathFormula() const override;
        };
    }
}

#endif /* STORM_LOGIC_PATHFORMULA_H_ */
