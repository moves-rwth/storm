#ifndef STORM_LOGIC_GLOBALLYFORMULA_H_
#define STORM_LOGIC_GLOBALLYFORMULA_H_

#include "src/logic/UnaryPathFormula.h"

namespace storm {
    namespace logic {
        class GloballyFormula : public UnaryPathFormula {
        public:
            GloballyFormula(std::shared_ptr<Formula> const& subformula);
            
            virtual ~GloballyFormula() {
                // Intentionally left empty.
            }
            
            virtual bool isGloballyFormula() const override;
            
            virtual std::ostream& writeToStream(std::ostream& out) const override;
        };
    }
}

#endif /* STORM_LOGIC_GLOBALLYFORMULA_H_ */