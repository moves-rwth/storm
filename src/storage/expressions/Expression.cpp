#include "src/storage/expressions/Expression.h"

namespace storm {
    namespace expressions {
        virtual Expression Expression::operator+(Expression const& other) {
            return Expression(this->getBaseExpression() + other.getBaseExpression());
        }
        
        BaseExpression const& getBaseExpression() const {
            return *this->expressionPtr;
        }
    }
}
