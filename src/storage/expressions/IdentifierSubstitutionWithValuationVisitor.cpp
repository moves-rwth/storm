#include <map>
#include <unordered_map>
#include <string>

#include "src/storage/expressions/IdentifierSubstitutionWithValuationVisitor.h"
#include "src/storage/expressions/Expressions.h"

namespace storm {
    namespace expressions  {
        IdentifierSubstitutionWithValuationVisitor::IdentifierSubstitutionWithValuationVisitor(Valuation const& valuation) : valuation(valuation) {
            // Intentionally left empty.
        }
        
        void IdentifierSubstitutionWithValuationVisitor::visit(VariableExpression const* expression) {
            // If the variable is the valuation, we need to replace it.
            if (expression->getReturnType() == ExpressionReturnType::Bool) {
                if (valuation.containsBooleanIdentifier(expression->getVariableName())) {
                    this->expressionStack.push(std::shared_ptr<BaseExpression>(new BooleanLiteralExpression(valuation.getBooleanValue(expression->getVariableName()))));
                } else {
                    this->expressionStack.push(expression->getSharedPointer());
                }
            } else if (expression->getReturnType() == ExpressionReturnType::Int) {
                if (valuation.containsIntegerIdentifier(expression->getVariableName())) {
                    this->expressionStack.push(std::shared_ptr<BaseExpression>(new IntegerLiteralExpression(valuation.getIntegerValue(expression->getVariableName()))));
                } else {
                    this->expressionStack.push(expression->getSharedPointer());
                }
            } else if (expression->getReturnType() == ExpressionReturnType::Double) {
                if (valuation.containsDoubleIdentifier(expression->getVariableName())) {
                    this->expressionStack.push(std::shared_ptr<BaseExpression>(new DoubleLiteralExpression(valuation.getDoubleValue(expression->getVariableName()))));
                } else {
                    this->expressionStack.push(expression->getSharedPointer());
                }
            }
        }
    }
}
