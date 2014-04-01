#include "src/storage/expressions/BaseExpression.h"

namespace storm {
    namespace expressions {
        BaseExpression::BaseExpression() : returnType(undefined) {
            // Intentionally left empty.
        }
        
        BaseExpression::BaseExpression(ExpressionReturnType returnType) : returnType(returnType) {
            // Intentionally left empty.
        }

        ExpressionReturnType BaseExpression::getReturnType() const {
            return this->returnType;
        }
        
        void BaseExpression::checkType(ExpressionReturnType actualType, ExpressionReturnType expectedType, std::string const& errorMessage) const {
            if (actualType != expectedType) {
                throw storm::exceptions::InvalidArgumentException() << errorMessage;
            }
        }
    }
}