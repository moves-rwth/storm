#ifndef STORM_LOGIC_GLOBALLYFORMULA_H_
#define STORM_LOGIC_GLOBALLYFORMULA_H_

#include "src/logic/UnaryPathFormula.h"

namespace storm {
    namespace logic {
        class GloballyFormula : public UnaryPathFormula {
        public:
            GloballyFormula(std::shared_ptr<Formula const> const& subformula);
            
            virtual ~GloballyFormula() {
                // Intentionally left empty.
            }
            
            virtual bool isGloballyFormula() const override;
            virtual bool isValidProbabilityPathFormula() const override;

            virtual std::shared_ptr<Formula> substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) const override;
            
            virtual std::ostream& writeToStream(std::ostream& out) const override;
        };
    }
}

#endif /* STORM_LOGIC_GLOBALLYFORMULA_H_ */