#include "src/storage/expressions/ToRationalFunctionVisitor.h"

#include <sstream>

#include "src/utility/constants.h"
#include "src/utility/macros.h"
#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace expressions {

#ifdef STORM_HAVE_CARL
        template<typename RationalFunctionType>
        ToRationalFunctionVisitor<RationalFunctionType>::ToRationalFunctionVisitor() : ExpressionVisitor(), cache(new carl::Cache<carl::PolynomialFactorizationPair<RawPolynomial>>()) {
            // Intentionally left empty.
        }
        
        template<typename RationalFunctionType>
        RationalFunctionType ToRationalFunctionVisitor<RationalFunctionType>::toRationalFunction(Expression const& expression) {
            return boost::any_cast<RationalFunctionType>(expression.accept(*this));
        }
        
        template<typename RationalFunctionType>
        boost::any ToRationalFunctionVisitor<RationalFunctionType>::visit(IfThenElseExpression const& expression) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expression cannot be translated into a rational function.");
        }
        
        template<typename RationalFunctionType>
        boost::any ToRationalFunctionVisitor<RationalFunctionType>::visit(BinaryBooleanFunctionExpression const& expression) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expression cannot be translated into a rational function.");
        }
        
        template<typename RationalFunctionType>
        boost::any ToRationalFunctionVisitor<RationalFunctionType>::visit(BinaryNumericalFunctionExpression const& expression) {
            RationalFunctionType firstOperandAsRationalFunction = boost::any_cast<RationalFunctionType>(expression.getFirstOperand()->accept(*this));
            RationalFunctionType secondOperandAsRationalFunction = boost::any_cast<RationalFunctionType>(expression.getSecondOperand()->accept(*this));
            uint_fast64_t exponentAsInteger = 0;
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
                case BinaryNumericalFunctionExpression::OperatorType::Power:
                    STORM_LOG_THROW(storm::utility::isInteger(secondOperandAsRationalFunction), storm::exceptions::InvalidArgumentException, "Exponent of power operator must be a positive integer.");
                    exponentAsInteger = storm::utility::convertNumber<uint_fast64_t>(secondOperandAsRationalFunction);
                    return storm::utility::pow(firstOperandAsRationalFunction, exponentAsInteger);
                    break;
                default:
                    STORM_LOG_ASSERT(false, "Illegal operator type.");
            }
            
            // Return a dummy. This point must, however, never be reached.
            return boost::any();
        }
        
        template<typename RationalFunctionType>
        boost::any ToRationalFunctionVisitor<RationalFunctionType>::visit(BinaryRelationExpression const& expression) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expression cannot be translated into a rational function.");
        }
        
        template<typename RationalFunctionType>
        boost::any ToRationalFunctionVisitor<RationalFunctionType>::visit(VariableExpression const& expression) {
            auto variablePair = variableToVariableMap.find(expression.getVariable());
            if (variablePair != variableToVariableMap.end()) {
                return convertVariableToPolynomial(variablePair->second);
            } else {
                carl::Variable carlVariable = carl::freshRealVariable(expression.getVariableName());
                variableToVariableMap.emplace(expression.getVariable(), carlVariable);
                return convertVariableToPolynomial(carlVariable);
            }
        }
        
        template<typename RationalFunctionType>
        boost::any ToRationalFunctionVisitor<RationalFunctionType>::visit(UnaryBooleanFunctionExpression const& expression) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expression cannot be translated into a rational function.");
        }
        
        template<typename RationalFunctionType>
        boost::any ToRationalFunctionVisitor<RationalFunctionType>::visit(UnaryNumericalFunctionExpression const& expression) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expression cannot be translated into a rational function.");
        }
        
        template<typename RationalFunctionType>
        boost::any ToRationalFunctionVisitor<RationalFunctionType>::visit(BooleanLiteralExpression const& expression) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expression cannot be translated into a rational function.");
        }
        
        template<typename RationalFunctionType>
        boost::any ToRationalFunctionVisitor<RationalFunctionType>::visit(IntegerLiteralExpression const& expression) {
            return RationalFunctionType(carl::rationalize<storm::RationalNumber>(static_cast<size_t>(expression.getValue())));
        }
        
        template<typename RationalFunctionType>
        boost::any ToRationalFunctionVisitor<RationalFunctionType>::visit(RationalLiteralExpression const& expression) {
            return storm::utility::convertNumber<storm::RationalFunction>(expression.getValue());
        }

        template class ToRationalFunctionVisitor<storm::RationalFunction>;
#endif
    }
}
