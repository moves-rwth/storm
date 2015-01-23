#ifndef STORM_LOGIC_UNTILFORMULA_H_
#define STORM_LOGIC_UNTILFORMULA_H_

#include "src/properties/logic/BinaryPathFormula.h"

namespace storm {
    namespace logic {
        class UntilFormula : public BinaryPathFormula {
        public:
            UntilFormula(std::shared_ptr<Formula> const& leftSubformula, std::shared_ptr<Formula> const& rightSubformula);
            
            virtual ~UntilFormula() {
                // Intentionally left empty.
            }
            
            virtual bool isUntilFormula() const override;
            
            virtual std::ostream& writeToStream(std::ostream& out) const override;
        };
    }
}

#endif /* STORM_LOGIC_UNTILFORMULA_H_ */