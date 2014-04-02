#include "src/storage/expressions/IntegerConstantExpression.h"

namespace storm {
    namespace expressions {
        IntegerConstantExpression::IntegerConstantExpression(std::string const& constantName) : ConstantExpression(ReturnType::int_, constantName) {
            // Intentionally left empty.
        }
    }
}