#include "src/storage/expressions/UnaryBooleanFunctionExpression.h"
#include "src/storage/expressions/BooleanLiteralExpression.h"
#include "src/utility/macros.h"
#include "src/exceptions/InvalidTypeException.h"

namespace storm {
    namespace expressions {
        UnaryBooleanFunctionExpression::UnaryBooleanFunctionExpression(ExpressionManager const& manager, Type const& type, std::shared_ptr<BaseExpression const> const& operand, OperatorType operatorType) : UnaryExpression(manager, type, operand), operatorType(operatorType) {
            // Intentionally left empty.
        }
        
        UnaryBooleanFunctionExpression::OperatorType UnaryBooleanFunctionExpression::getOperatorType() const {
            return this->operatorType;
        }
        
        storm::expressions::OperatorType UnaryBooleanFunctionExpression::getOperator() const {
            switch (this->getOperatorType()) {
                case OperatorType::Not: return storm::expressions::OperatorType::Not;
            }
        }
        
        bool UnaryBooleanFunctionExpression::evaluateAsBool(Valuation const* valuation) const {
            STORM_LOG_THROW(this->hasBooleanType(), storm::exceptions::InvalidTypeException, "Unable to evaluate expression as boolean.");

            bool operandEvaluated = this->getOperand()->evaluateAsBool(valuation);
            switch (this->getOperatorType()) {
                case OperatorType::Not: return !operandEvaluated; break;
            }
        }
        
        std::shared_ptr<BaseExpression const> UnaryBooleanFunctionExpression::simplify() const {
            std::shared_ptr<BaseExpression const> operandSimplified = this->getOperand()->simplify();
            switch (this->getOperatorType()) {
                case OperatorType::Not: if (operandSimplified->isTrue()) {
                    return std::shared_ptr<BaseExpression>(new BooleanLiteralExpression(this->getManager(), false));
                } else {
                    return std::shared_ptr<BaseExpression>(new BooleanLiteralExpression(this->getManager(), true));
                }
            }
            
            if (operandSimplified.get() == this->getOperand().get()) {
                return this->shared_from_this();
            } else {
                return std::shared_ptr<BaseExpression>(new UnaryBooleanFunctionExpression(this->getManager(), this->getType(), operandSimplified, this->getOperatorType()));
            }
        }
        
        boost::any UnaryBooleanFunctionExpression::accept(ExpressionVisitor& visitor) const {
            return visitor.visit(*this);
        }
        
        void UnaryBooleanFunctionExpression::printToStream(std::ostream& stream) const {
            stream << "!(" << *this->getOperand() << ")";
        }
    }
}