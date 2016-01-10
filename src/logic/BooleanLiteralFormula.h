#ifndef STORM_LOGIC_BOOLEANLITERALFORMULA_H_
#define STORM_LOGIC_BOOLEANLITERALFORMULA_H_

#include "src/logic/StateFormula.h"

namespace storm {
    namespace logic {
        class BooleanLiteralFormula : public StateFormula {
        public:
            BooleanLiteralFormula(bool value);
            
            virtual ~BooleanLiteralFormula() {
                // Intentionally left empty.
            }
            
            virtual bool isBooleanLiteralFormula() const override;
            virtual bool isTrueFormula() const override;
            virtual bool isFalseFormula() const override;

            virtual bool isPctlStateFormula() const override;
            virtual bool isLtlFormula() const override;
            virtual bool isPropositionalFormula() const override;
            
            virtual std::shared_ptr<Formula> substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) const override;
            
            virtual std::ostream& writeToStream(std::ostream& out) const override;
                        
        private:
            bool value;
        };
    }
}

#endif /* STORM_LOGIC_BOOLEANLITERALFORMULA_H_ */