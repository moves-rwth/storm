#ifndef STORM_LOGIC_NEXTFORMULA_H_
#define STORM_LOGIC_NEXTFORMULA_H_

#include "src/logic/UnaryPathFormula.h"

namespace storm {
    namespace logic {
        class NextFormula : public UnaryPathFormula {
        public:
            NextFormula(std::shared_ptr<Formula> const& subformula);
            
            virtual ~NextFormula() {
                // Intentionally left empty.
            }
            
            virtual bool isNextFormula() const override;
            
            virtual std::ostream& writeToStream(std::ostream& out) const override;
        };
    }
}

#endif /* STORM_LOGIC_NEXTFORMULA_H_ */