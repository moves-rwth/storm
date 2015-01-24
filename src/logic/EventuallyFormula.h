#ifndef STORM_LOGIC_EVENTUALLYFORMULA_H_
#define STORM_LOGIC_EVENTUALLYFORMULA_H_

#include "src/logic/UnaryPathFormula.h"

namespace storm {
    namespace logic {
        class EventuallyFormula : public UnaryPathFormula {
        public:
            EventuallyFormula(std::shared_ptr<Formula const> const& subformula);
            
            virtual ~EventuallyFormula() {
                // Intentionally left empty.
            }
            
            virtual bool isEventuallyFormula() const override;
            
            virtual std::ostream& writeToStream(std::ostream& out) const override;
        };
    }
}

#endif /* STORM_LOGIC_EVENTUALLYFORMULA_H_ */