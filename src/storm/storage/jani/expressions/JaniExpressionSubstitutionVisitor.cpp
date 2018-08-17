#include "storm/storage/jani/expressions/JaniExpressionSubstitutionVisitor.h"

#include "storm/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace expressions {
        template<typename MapType>
        boost::any JaniExpressionSubstitutionVisitor<MapType>::visit(ValueArrayExpression const& expression, boost::any const& data) {
            uint64_t size = expression.getSize()->evaluateAsInt();
            std::vector<std::shared_ptr<BaseExpression const>> newElements;
            newElements.reserve(size);
            for (uint64_t i = 0; i < size; ++i) {
                newElements.push_back(boost::any_cast<std::shared_ptr<BaseExpression const>>(expression.at(i)->accept(*this, data)));
            }
            return std::const_pointer_cast<BaseExpression const>(std::shared_ptr<BaseExpression>(new ValueArrayExpression(expression.getManager(), expression.getType(), newElements)));
        }
    
        template<typename MapType>
        boost::any JaniExpressionSubstitutionVisitor<MapType>::visit(ConstructorArrayExpression const& expression, boost::any const& data) {
            std::shared_ptr<BaseExpression const> newSize = boost::any_cast<std::shared_ptr<BaseExpression const>>(expression.getSize()->accept(*this, data));
            std::shared_ptr<BaseExpression const> elementExpression = boost::any_cast<std::shared_ptr<BaseExpression const>>(expression.getElementExpression()->accept(*this, data));
            STORM_LOG_THROW(this->variableToExpressionMapping.find(expression.getIndexVar()) == this->variableToExpressionMapping.end(), storm::exceptions::InvalidArgumentException, "substitution of the index variable of a constructorArrayExpression is not possible.");
            
            // If the arguments did not change, we simply push the expression itself.
            if (newSize.get() == expression.getSize().get() && elementExpression.get() == expression.getElementExpression().get()) {
                return expression.getSharedPointer();
            } else {
				return std::const_pointer_cast<BaseExpression const>(std::shared_ptr<BaseExpression>(new ConstructorArrayExpression(expression.getManager(), expression.getType(), newSize, expression.getIndexVar(), elementExpression)));
            }
        }

        template<typename MapType>
        boost::any JaniExpressionSubstitutionVisitor<MapType>::visit(ArrayAccessExpression const& expression, boost::any const& data) {
            std::shared_ptr<BaseExpression const> firstExpression = boost::any_cast<std::shared_ptr<BaseExpression const>>(expression.getFirstOperand()->accept(*this, data));
            std::shared_ptr<BaseExpression const> secondExpression = boost::any_cast<std::shared_ptr<BaseExpression const>>(expression.getSecondOperand()->accept(*this, data));
            
            // If the arguments did not change, we simply push the expression itself.
            if (firstExpression.get() == expression.getFirstOperand().get() && secondExpression.get() == expression.getSecondOperand().get()) {
                return expression.getSharedPointer();
            } else {
				return std::const_pointer_cast<BaseExpression const>(std::shared_ptr<BaseExpression>(new ArrayAccessExpression(expression.getManager(), expression.getType(), firstExpression, secondExpression)));
            }
        }


        // Explicitly instantiate the class with map and unordered_map.
		template class JaniExpressionSubstitutionVisitor<std::map<Variable, Expression>>;
		template class JaniExpressionSubstitutionVisitor<std::unordered_map<Variable, Expression>>;
    
    }
}
