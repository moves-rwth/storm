#ifndef STORM_LOGIC_UNARYSTATEFORMULA_H_
#define STORM_LOGIC_UNARYSTATEFORMULA_H_

#include "src/logic/StateFormula.h"

namespace storm {
    namespace logic {
        class UnaryStateFormula : public StateFormula {
        public:
            UnaryStateFormula(std::shared_ptr<Formula> subformula);
            
            virtual ~UnaryStateFormula() {
                // Intentionally left empty.
            }
            
            virtual bool isUnaryStateFormula() const override;
            
            Formula& getSubformula();
            Formula const& getSubformula() const;
            
        private:
            std::shared_ptr<Formula> subformula;
        };
    }
}

#endif /* STORM_LOGIC_UNARYSTATEFORMULA_H_ */