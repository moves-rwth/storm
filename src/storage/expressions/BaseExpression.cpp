#include "src/storage/expressions/BaseExpression.h"
#include "src/exceptions/ExceptionMacros.h"
#include "src/exceptions/InvalidTypeException.h"

namespace storm {
    namespace expressions {        
        BaseExpression::BaseExpression(ExpressionReturnType returnType) : returnType(returnType) {
            // Intentionally left empty.
        }

        ExpressionReturnType BaseExpression::getReturnType() const {
            return this->returnType;
        }
        
        bool BaseExpression::hasIntegralReturnType() const {
            return this->getReturnType() == ExpressionReturnType::Int;
        }
        
        bool BaseExpression::hasNumericalReturnType() const {
            return this->getReturnType() == ExpressionReturnType::Double || this->getReturnType() == ExpressionReturnType::Int;
        }
        
        bool BaseExpression::hasBooleanReturnType() const {
            return this->getReturnType() == ExpressionReturnType::Bool;
        }
        
        int_fast64_t BaseExpression::evaluateAsInt(Valuation const& evaluation) const {
            LOG_THROW(false, storm::exceptions::InvalidTypeException, "Unable to evaluate expression as integer.");
        }
        
        bool BaseExpression::evaluateAsBool(Valuation const& evaluation) const {
            LOG_THROW(false, storm::exceptions::InvalidTypeException, "Unable to evaluate expression as boolean.");
        }
        
        double BaseExpression::evaluateAsDouble(Valuation const& evaluation) const {
            LOG_THROW(false, storm::exceptions::InvalidTypeException, "Unable to evaluate expression as double.");
        }
        
        bool BaseExpression::isConstant() const {
            return false;
        }

        bool BaseExpression::isTrue() const {
            return false;
        }

        bool BaseExpression::isFalse() const {
            return false;
        }
        
        std::shared_ptr<BaseExpression const> BaseExpression::getSharedPointer() const {
            return this->shared_from_this();
        }
        
        std::ostream& operator<<(std::ostream& stream, ExpressionReturnType const& enumValue) {
            stream << static_cast<std::underlying_type<ExpressionReturnType>::type>(enumValue);
            return stream;
        }
        
        std::ostream& operator<<(std::ostream& stream, BaseExpression const& expression) {
            expression.printToStream(stream);
            return stream;
        }
    }
}