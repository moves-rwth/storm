#include "src/storage/expressions/ConstantExpression.h"

namespace storm {
    namespace expressions {
        ConstantExpression::ConstantExpression(ReturnType returnType, std::string const& constantName) : BaseExpression(returnType), constantName(constantName) {
            // Intentionally left empty.
        }
        
        std::string const& ConstantExpression::getConstantName() const {
            return this->constantName;
        }
    }
}