#include "JaniReduceNestingExpressionVisitor.h"

#include "storm/exceptions/InvalidArgumentException.h"

namespace storm {
    
    namespace jani {
        storm::expressions::Expression reduceNestingInJaniExpression(storm::expressions::Expression const& expression) {
            return storm::expressions::JaniReduceNestingExpressionVisitor().reduceNesting(expression);
        }
    }
    
    namespace expressions {
        
        JaniReduceNestingExpressionVisitor::JaniReduceNestingExpressionVisitor() : ReduceNestingVisitor() {
            // Intentionally left empty.
        }
        
        boost::any JaniReduceNestingExpressionVisitor::visit(ValueArrayExpression const& expression, boost::any const& data) {
            ValueArrayExpression::ValueArrayElements newElements;
            newElements.elementsWithValue = boost::any_cast<std::vector<std::shared_ptr<BaseExpression const>>>(visit(expression.getElements(), data));
            return std::const_pointer_cast<BaseExpression const>(std::shared_ptr<BaseExpression>(new ValueArrayExpression(expression.getManager(), expression.getType(), newElements)));
        }

        boost::any JaniReduceNestingExpressionVisitor::visit(ValueArrayExpression::ValueArrayElements const& elements, boost::any const& data) {
            std::vector<std::shared_ptr<BaseExpression const>> elementsWithValue;
            if (elements.elementsWithValue) {
                elementsWithValue.reserve(elements.elementsWithValue->size());
                for (auto& elem : elements.elementsWithValue.get()) {
                    elementsWithValue.push_back(boost::any_cast<std::shared_ptr<BaseExpression const>>(elem->accept(*this, data)));
                }
            } else {
                for (auto& elem : elements.elementsOfElements.get()) {
                    auto res = boost::any_cast<std::vector<std::shared_ptr<BaseExpression const>>>(visit(*elem, data));
                    for (auto& entry : res) {
                        elementsWithValue.push_back(entry);
                    }
                }
            }
            return elementsWithValue;
        }
    
        boost::any JaniReduceNestingExpressionVisitor::visit(ConstructorArrayExpression const& expression, boost::any const& data) {
            std::shared_ptr<BaseExpression const> newSize = boost::any_cast<std::shared_ptr<BaseExpression const>>(expression.size()->accept(*this, data));
            std::shared_ptr<BaseExpression const> elementExpression = boost::any_cast<std::shared_ptr<BaseExpression const>>(expression.getElementExpression()->accept(*this, data));
            
            // If the arguments did not change, we simply push the expression itself.
            if (newSize.get() == expression.size().get() && elementExpression.get() == expression.getElementExpression().get()) {
                return expression.getSharedPointer();
            } else {
                assert (false);
                // TODO: How to deal with arrays
//				return std::const_pointer_cast<BaseExpression const>(std::shared_ptr<BaseExpression>(new ConstructorArrayExpression(expression.getManager(), expression.getType(), newSize, expression.getIndexVar(), elementExpression)));
            }
        }

        boost::any JaniReduceNestingExpressionVisitor::visit(ArrayAccessExpression const& expression, boost::any const& data) {
            std::shared_ptr<BaseExpression const> firstExpression = boost::any_cast<std::shared_ptr<BaseExpression const>>(expression.getFirstOperand()->accept(*this, data));
            std::shared_ptr<BaseExpression const> secondExpression = boost::any_cast<std::shared_ptr<BaseExpression const>>(expression.getSecondOperand()->accept(*this, data));
            
            // If the arguments did not change, we simply push the expression itself.
            if (firstExpression.get() == expression.getFirstOperand().get() && secondExpression.get() == expression.getSecondOperand().get()) {
                return expression.getSharedPointer();
            } else {
				return std::const_pointer_cast<BaseExpression const>(std::shared_ptr<BaseExpression>(new ArrayAccessExpression(expression.getManager(), expression.getType(), firstExpression, secondExpression)));
            }
        }

        boost::any JaniReduceNestingExpressionVisitor::visit(ArrayAccessIndexExpression const& expression, boost::any const& data) {
            if (expression.getFirstOperand() == expression.getSecondOperand()) {
                std::shared_ptr<BaseExpression const> firstExpression = boost::any_cast<std::shared_ptr<BaseExpression const>>(expression.getFirstOperand()->accept(*this, data));

                // If the arguments did not change, we simply push the expression itself.
                if (firstExpression.get() == expression.getFirstOperand().get()) {
                    return expression.getSharedPointer();
                } else {
                    return std::const_pointer_cast<BaseExpression const>(std::shared_ptr<BaseExpression>(new ArrayAccessIndexExpression(expression.getManager(), expression.getType(), firstExpression)));
                }
            } else {
                std::shared_ptr<BaseExpression const> firstExpression = boost::any_cast<std::shared_ptr<BaseExpression const>>(expression.getFirstOperand()->accept(*this, data));
                std::shared_ptr<BaseExpression const> secondExpression = boost::any_cast<std::shared_ptr<BaseExpression const>>(expression.getSecondOperand()->accept(*this, data));

                // If the arguments did not change, we simply push the expression itself.
                if (firstExpression.get() == expression.getFirstOperand().get() && secondExpression.get() == expression.getSecondOperand().get()) {
                    return expression.getSharedPointer();
                } else {
                    return std::const_pointer_cast<BaseExpression const>(std::shared_ptr<BaseExpression>(new ArrayAccessIndexExpression(expression.getManager(), expression.getType(), firstExpression, secondExpression)));
                }
            }
        }

        boost::any JaniReduceNestingExpressionVisitor::visit(FunctionCallExpression const& expression, boost::any const& data) {
            std::vector<std::shared_ptr<BaseExpression const>> newArguments;
            newArguments.reserve(expression.getNumberOfArguments());
            for (uint64_t i = 0; i < expression.getNumberOfArguments(); ++i) {
                newArguments.push_back(boost::any_cast<std::shared_ptr<BaseExpression const>>(expression.getArgument(i)->accept(*this, data)));
            }
            return std::const_pointer_cast<BaseExpression const>(std::shared_ptr<BaseExpression>(new FunctionCallExpression(expression.getManager(), expression.getType(), expression.getFunctionIdentifier(), newArguments)));
        }
    }
}
