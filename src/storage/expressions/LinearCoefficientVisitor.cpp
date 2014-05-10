#include "src/storage/expressions/LinearCoefficientVisitor.h"

#include "src/storage/expressions/Expressions.h"
#include "src/exceptions/ExceptionMacros.h"
#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace expressions {
        std::pair<SimpleValuation, double> LinearCoefficientVisitor::getLinearCoefficients(Expression const& expression) {
            expression.getBaseExpression().accept(this);
            return resultStack.top();
        }
        
        void LinearCoefficientVisitor::visit(IfThenElseExpression const* expression) {
            LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expression is non-linear.");
        }
        
        void LinearCoefficientVisitor::visit(BinaryBooleanFunctionExpression const* expression) {
            
        }
        
        void LinearCoefficientVisitor::visit(BinaryNumericalFunctionExpression const* expression) {
            
        }
        
        void LinearCoefficientVisitor::visit(BinaryRelationExpression const* expression) {
            
        }
        
        void LinearCoefficientVisitor::visit(VariableExpression const* expression) {
            SimpleValuation valuation;
            switch (expression->getReturnType()) {
                case ExpressionReturnType::Bool: LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expression is non-linear."); break;
                case ExpressionReturnType::Int:
                case ExpressionReturnType::Double: valuation.addDoubleIdentifier(expression->getVariableName(), 1); break;
                case ExpressionReturnType::Undefined: LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Illegal expression return type."); break;
            }
            
            resultStack.push(std::make_pair(valuation, 0));
        }
        
        void LinearCoefficientVisitor::visit(UnaryBooleanFunctionExpression const* expression) {
            LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expression is non-linear.");
        }
        
        void LinearCoefficientVisitor::visit(UnaryNumericalFunctionExpression const* expression) {
            if (expression->getOperatorType() == UnaryNumericalFunctionExpression::OperatorType::Minus) {
                // Here, we need to negate all double identifiers.
                std::pair<SimpleValuation, double>& valuationConstantPair = resultStack.top();
                for (auto const& identifier : valuationConstantPair.first.getDoubleIdentifiers()) {
                    valuationConstantPair.first.setDoubleValue(identifier, -valuationConstantPair.first.getDoubleValue(identifier));
                }
            } else {
                LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expression is non-linear.");
            }
        }
        
        void LinearCoefficientVisitor::visit(BooleanLiteralExpression const* expression) {
            LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expression is non-linear.");
        }
        
        void LinearCoefficientVisitor::visit(IntegerLiteralExpression const* expression) {
            resultStack.push(std::make_pair(SimpleValuation(), static_cast<double>(expression->getValue())));
        }
        
        void LinearCoefficientVisitor::visit(DoubleLiteralExpression const* expression) {
            resultStack.push(std::make_pair(SimpleValuation(), expression->getValue()));
        }
    }
}