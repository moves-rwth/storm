#ifndef STORM_LOGIC_ATOMICEXPRESSIONFORMULA_H_
#define STORM_LOGIC_ATOMICEXPRESSIONFORMULA_H_

#include "src/logic/StateFormula.h"
#include "src/storage/expressions/Expression.h"

namespace storm {
    namespace logic {
        class AtomicExpressionFormula : public StateFormula {
        public:
            AtomicExpressionFormula(storm::expressions::Expression const& expression);
            
            virtual ~AtomicExpressionFormula() {
                // Intentionally left empty.
            }
            
            virtual bool isAtomicExpressionFormula() const override;
            virtual bool isPropositionalFormula() const override;

            storm::expressions::Expression const& getExpression() const;
            
            virtual std::ostream& writeToStream(std::ostream& out) const override;
            
        private:
            // The atomic expression represented by this node in the formula tree.
            storm::expressions::Expression expression;
        };
    }
}

#endif /* STORM_LOGIC_ATOMICEXPRESSIONFORMULA_H_ */