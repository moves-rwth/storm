#include "src/storage/expressions/BaseExpression.h"
#include "src/exceptions/ExceptionMacros.h"

namespace storm {
    namespace expressions {        
        BaseExpression::BaseExpression(ExpressionReturnType returnType) : returnType(returnType) {
            // Intentionally left empty.
        }

        ExpressionReturnType BaseExpression::getReturnType() const {
            return this->returnType;
        }
        
        int_fast64_t BaseExpression::evaluateAsInt(Valuation const& evaluation) const {
            LOG_ASSERT(false, "Unable to evaluate expression as integer.");
        }
        
        bool BaseExpression::evaluateAsBool(Valuation const& evaluation) const {
            LOG_ASSERT(false, "Unable to evaluate expression as boolean.");
        }
        
        double BaseExpression::evaluateAsDouble(Valuation const& evaluation) const {
            LOG_ASSERT(false, "Unable to evaluate expression as double.");
        }
        
        bool BaseExpression::isTrue() const {
            return false;
        }

        bool BaseExpression::isFalse() const {
            return false;
        }
    }
}