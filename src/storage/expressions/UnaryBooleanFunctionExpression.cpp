#include "src/storage/expressions/UnaryBooleanFunctionExpression.h"

namespace storm {
    namespace expressions {
        UnaryBooleanFunctionExpression::UnaryBooleanFunctionExpression(ReturnType returnType, std::unique_ptr<BaseExpression>&& argument, FunctionType functionType) : UnaryExpression(returnType, std::move(argument)), functionType(functionType) {
            // Intentionally left empty.
        }
    }
}