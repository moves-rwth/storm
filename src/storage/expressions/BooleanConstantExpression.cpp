#include "src/storage/expressions/BooleanConstantExpression.h"

namespace storm {
    namespace expressions {
        BooleanConstantExpression::BooleanConstantExpression(std::string const& constantName) : ConstantExpression(ReturnType::bool_, constantName) {
            // Intentionally left empty.
        }
    }
}