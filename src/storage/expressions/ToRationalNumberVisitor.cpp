#include "src/storage/expressions/ToRationalNumberVisitor.h"

#include "src/utility/macros.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/exceptions/NotSupportedException.h"

namespace storm {
    namespace expressions {
        template<typename RationalNumberType>
        ToRationalNumberVisitor<RationalNumberType>::ToRationalNumberVisitor() : ExpressionVisitor() {
            // Intentionally left empty.
        }
        
        template<typename RationalNumberType>
        RationalNumberType ToRationalNumberVisitor<RationalNumberType>::toRationalNumber(Expression const& expression) {
            return boost::any_cast<RationalNumberType>(expression.accept(*this));
        }
        
        template<typename RationalNumberType>
        boost::any ToRationalNumberVisitor<RationalNumberType>::visit(IfThenElseExpression const& expression) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expression cannot be translated into a rational number.");
        }
        
        template<typename RationalNumberType>
        boost::any ToRationalNumberVisitor<RationalNumberType>::visit(BinaryBooleanFunctionExpression const& expression) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expression cannot be translated into a rational number.");
        }
        
        template<typename RationalNumberType>
        boost::any ToRationalNumberVisitor<RationalNumberType>::visit(BinaryNumericalFunctionExpression const& expression) {
            RationalNumberType firstOperandAsRationalFunction = boost::any_cast<RationalNumberType>(expression.getFirstOperand()->accept(*this));
            RationalNumberType secondOperandAsRationalFunction = boost::any_cast<RationalNumberType>(expression.getSecondOperand()->accept(*this));
            switch(expression.getOperatorType()) {
                case BinaryNumericalFunctionExpression::OperatorType::Plus:
                    return firstOperandAsRationalFunction + secondOperandAsRationalFunction;
                    break;
                case BinaryNumericalFunctionExpression::OperatorType::Minus:
                    return firstOperandAsRationalFunction - secondOperandAsRationalFunction;
                    break;
                case BinaryNumericalFunctionExpression::OperatorType::Times:
                    return firstOperandAsRationalFunction * secondOperandAsRationalFunction;
                    break;
                case BinaryNumericalFunctionExpression::OperatorType::Divide:
                    return firstOperandAsRationalFunction / secondOperandAsRationalFunction;
                    break;
                default:
                    STORM_LOG_ASSERT(false, "Illegal operator type.");
            }
            
            // Return a dummy. This point must, however, never be reached.
            return boost::any();
        }
        
        template<typename RationalNumberType>
        boost::any ToRationalNumberVisitor<RationalNumberType>::visit(BinaryRelationExpression const& expression) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expression cannot be translated into a rational number.");
        }
        
        template<typename RationalNumberType>
        boost::any ToRationalNumberVisitor<RationalNumberType>::visit(VariableExpression const& expression) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Cannot transform expressions containing variables to a rational number.");
        }
        
        template<typename RationalNumberType>
        boost::any ToRationalNumberVisitor<RationalNumberType>::visit(UnaryBooleanFunctionExpression const& expression) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expression cannot be translated into a rational number.");
        }
        
        template<typename RationalNumberType>
        boost::any ToRationalNumberVisitor<RationalNumberType>::visit(UnaryNumericalFunctionExpression const& expression) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expression cannot be translated into a rational number.");
        }
        
        template<typename RationalNumberType>
        boost::any ToRationalNumberVisitor<RationalNumberType>::visit(BooleanLiteralExpression const& expression) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expression cannot be translated into a rational number.");
        }
        
        template<typename RationalNumberType>
        boost::any ToRationalNumberVisitor<RationalNumberType>::visit(IntegerLiteralExpression const& expression) {
#ifdef STORM_HAVE_CARL
            return RationalNumberType(carl::rationalize<storm::RationalNumber>(static_cast<size_t>(expression.getValue())));
#else
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Rational numbers are not supported in this build.");
#endif
        }
        
        template<typename RationalNumberType>
        boost::any ToRationalNumberVisitor<RationalNumberType>::visit(DoubleLiteralExpression const& expression) {
#ifdef STORM_HAVE_CARL
            return RationalNumberType(carl::rationalize<storm::RationalNumber>(expression.getValue()));
#else
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Rational numbers are not supported in this build.");
#endif
        }
        
#ifdef STORM_HAVE_CARL
        template class ToRationalNumberVisitor<storm::RationalNumber>;
#endif
    }
}
