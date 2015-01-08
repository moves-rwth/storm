#include "src/storage/expressions/BaseExpression.h"
#include "src/storage/expressions/ExpressionManager.h"
#include "src/utility/macros.h"
#include "src/exceptions/InvalidTypeException.h"
#include "src/exceptions/InvalidAccessException.h"

namespace storm {
    namespace expressions {        
        BaseExpression::BaseExpression(ExpressionManager const& manager, Type const& type) : manager(manager), type(type) {
            // Intentionally left empty.
        }

        Type const& BaseExpression::getType() const {
            return this->type;
        }
        
        bool BaseExpression::hasIntegerType() const {
            return this->getType().isIntegerType();
        }

        bool BaseExpression::hasBitVectorType() const {
            return this->getType().isBitVectorType();
        }
        
        bool BaseExpression::hasNumericalType() const {
            return this->getType().isNumericalType();
        }
        
        bool BaseExpression::hasBooleanType() const {
            return this->getType().isBooleanType();
        }
        
        bool BaseExpression::hasRationalType() const {
            return this->getType().isRationalType();
        }
        
        int_fast64_t BaseExpression::evaluateAsInt(Valuation const* valuation) const {
            STORM_LOG_THROW(false, storm::exceptions::InvalidTypeException, "Unable to evaluate expression as integer.");
        }
        
        bool BaseExpression::evaluateAsBool(Valuation const* valuation) const {
            STORM_LOG_THROW(false, storm::exceptions::InvalidTypeException, "Unable to evaluate expression as boolean.");
        }
        
        double BaseExpression::evaluateAsDouble(Valuation const* valuation) const {
            STORM_LOG_THROW(false, storm::exceptions::InvalidTypeException, "Unable to evaluate expression as double.");
        }
        
        uint_fast64_t BaseExpression::getArity() const {
            return 0;
        }
        
        std::shared_ptr<BaseExpression const> BaseExpression::getOperand(uint_fast64_t operandIndex) const {
            STORM_LOG_THROW(false, storm::exceptions::InvalidAccessException, "Unable to access operand " << operandIndex << " in expression of arity 0.");
        }
        
        std::string const& BaseExpression::getIdentifier() const {
            STORM_LOG_THROW(false, storm::exceptions::InvalidAccessException, "Unable to access identifier of non-constant, non-variable expression.");
        }
        
        OperatorType BaseExpression::getOperator() const {
            STORM_LOG_THROW(false, storm::exceptions::InvalidAccessException, "Unable to access operator of non-function application expression.");
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
        
        ExpressionManager const& BaseExpression::getManager() const {
            return manager;
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