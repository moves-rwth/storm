#ifndef STORM_LOGIC_CONDITIONALPATHFORMULA_H_
#define STORM_LOGIC_CONDITIONALPATHFORMULA_H_

#include "src/logic/BinaryPathFormula.h"

namespace storm {
    namespace logic {
        class ConditionalPathFormula : public BinaryPathFormula {
        public:
            ConditionalPathFormula(std::shared_ptr<Formula> const& leftSubformula, std::shared_ptr<Formula> const& rightSubformula);
            
            virtual ~ConditionalPathFormula() {
                // Intentionally left empty.
            }
            
            virtual bool isConditionalPathFormula() const override;
            
            virtual std::ostream& writeToStream(std::ostream& out) const override;
        };
    }
}

#endif /* STORM_LOGIC_CONDITIONALPATHFORMULA_H_ */