#ifndef STORM_LOGIC_UNARYBOOLEANSTATEFORMULA_H_
#define STORM_LOGIC_UNARYBOOLEANSTATEFORMULA_H_

#include "src/logic/UnaryStateFormula.h"

namespace storm {
    namespace logic {
        class UnaryBooleanStateFormula : public UnaryStateFormula {
        public:
            virtual ~UnaryBooleanStateFormula() {
                // Intentionally left empty.
            };
            
            virtual bool isUnaryBooleanStateFormula() const override;
        };
    }
}

#endif /* STORM_LOGIC_UNARYBOOLEANSTATEFORMULA_H_ */