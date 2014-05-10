#include "src/storage/expressions/BaseExpression.h"
#include "src/exceptions/ExceptionMacros.h"
#include "src/exceptions/InvalidTypeException.h"
#include "src/exceptions/InvalidAccessException.h"

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
        
        int_fast64_t BaseExpression::evaluateAsInt(Valuation const* valuation) const {
            LOG_THROW(false, storm::exceptions::InvalidTypeException, "Unable to evaluate expression as integer.");
        }
        
        bool BaseExpression::evaluateAsBool(Valuation const* valuation) const {
            LOG_THROW(false, storm::exceptions::InvalidTypeException, "Unable to evaluate expression as boolean.");
        }
        
        double BaseExpression::evaluateAsDouble(Valuation const* valuation) const {
            LOG_THROW(false, storm::exceptions::InvalidTypeException, "Unable to evaluate expression as double.");
        }
        
        uint_fast64_t BaseExpression::getArity() const {
            return 0;
        }
        
        std::shared_ptr<BaseExpression const> BaseExpression::getOperand(uint_fast64_t operandIndex) const {
            LOG_THROW(false, storm::exceptions::InvalidAccessException, "Unable to access operand " << operandIndex << " in expression of arity 0.");
        }
        
        std::string const& BaseExpression::getIdentifier() const {
            LOG_THROW(false, storm::exceptions::InvalidAccessException, "Unable to access identifier of non-constant, non-variable expression.");
        }
        
        OperatorType BaseExpression::getOperator() const {
            LOG_THROW(false, storm::exceptions::InvalidAccessException, "Unable to access operator of non-function application expression.");
        }
        
        bool BaseExpression::containsVariables() const {
            return false;
        }
        
        bool BaseExpression::isLiteral() const {
            return false;
        }
        
        bool BaseExpression::isVariable() const {
            return false;
        }
        
        bool BaseExpression::isTrue() const {
            return false;
        }

        bool BaseExpression::isFalse() const {
            return false;
        }
        
        bool BaseExpression::isFunctionApplication() const {
            return false;
        }
        
        std::shared_ptr<BaseExpression const> BaseExpression::getSharedPointer() const {
            return this->shared_from_this();
        }
        
        std::ostream& operator<<(std::ostream& stream, BaseExpression const& expression) {
            expression.printToStream(stream);
            return stream;
        }
    }
}