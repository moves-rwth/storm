#include "src/properties/logic/AtomicExpressionFormula.h"

namespace storm {
    namespace logic {
        bool AtomicExpressionFormula::isAtomicExpressionFormula() const {
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