#include "src/storage/expressions/UnaryNumericalFunctionExpression.h"

namespace storm {
    namespace expressions {
        UnaryNumericalFunctionExpression::UnaryNumericalFunctionExpression(ReturnType returnType, std::unique_ptr<BaseExpression>&& argument, FunctionType functionType) : UnaryExpression(returnType, std::move(argument)), functionType(functionType) {
            // Intentionally left empty.
        }
    }
}