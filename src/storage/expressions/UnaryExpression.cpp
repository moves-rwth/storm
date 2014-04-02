#include "src/storage/expressions/UnaryExpression.h"

namespace storm {
    namespace expressions {
        UnaryExpression::UnaryExpression(ExpressionReturnType returnType, std::unique_ptr<BaseExpression>&& argument) : BaseExpression(returnType), argument(std::move(argument)) {
            // Intentionally left empty.
        }
    }
}