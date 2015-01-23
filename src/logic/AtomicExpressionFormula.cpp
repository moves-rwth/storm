#include "src/logic/AtomicExpressionFormula.h"

namespace storm {
    namespace logic {
        AtomicExpressionFormula::AtomicExpressionFormula(storm::expressions::Expression const& expression) : expression(expression) {
            // Intentionally left empty.
        }
        
        bool AtomicExpressionFormula::isAtomicExpressionFormula() const {
            return true;
        }
        
        bool AtomicExpressionFormula::isPropositionalFormula() const {
            return true;
        }
        
        storm::expressions::Expression const& AtomicExpressionFormula::getExpression() const {
            return expression;
        }
        
        std::ostream& AtomicExpressionFormula::writeToStream(std::ostream& out) const {
            out << expression;
            return out;
        }
    }
}