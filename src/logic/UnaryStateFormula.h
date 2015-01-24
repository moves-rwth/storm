#ifndef STORM_LOGIC_UNARYSTATEFORMULA_H_
#define STORM_LOGIC_UNARYSTATEFORMULA_H_

#include "src/logic/StateFormula.h"

namespace storm {
    namespace logic {
        class UnaryStateFormula : public StateFormula {
        public:
            UnaryStateFormula(std::shared_ptr<Formula const> subformula);
            
            virtual ~UnaryStateFormula() {
                // Intentionally left empty.
            }
            
            virtual bool isUnaryStateFormula() const override;
            virtual bool isPropositionalFormula() const override;

            Formula const& getSubformula() const;
            
        private:
            std::shared_ptr<Formula const> subformula;
        };
    }
}

#endif /* STORM_LOGIC_UNARYSTATEFORMULA_H_ */