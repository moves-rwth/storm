#ifndef STORM_LOGIC_STATEFORMULA_H_
#define STORM_LOGIC_STATEFORMULA_H_

#include "src/properties/logic/Formula.h"

namespace storm {
    namespace logic {
        class StateFormula : public Formula {
        public:
            virtual ~StateFormula() {
                // Intentionally left empty.
            };
            
            virtual bool isStateFormula() const override;
        };
    }
}

#endif /* STORM_LOGIC_STATEFORMULA_H_ */