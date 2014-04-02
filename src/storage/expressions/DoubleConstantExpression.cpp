#include "src/storage/expressions/DoubleConstantExpression.h"

namespace storm {
    namespace expressions {
        DoubleConstantExpression::DoubleConstantExpression(std::string const& constantName) : ConstantExpression(ReturnType::double_, constantName) {
            // Intentionally left empty.
        }
    }
}