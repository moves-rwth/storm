#include "JaniSyntacticalEqualityCheckVisitor.h"

#include "storm/exceptions/InvalidArgumentException.h"

namespace storm {
    
    namespace expressions {
        
        JaniSyntacticalEqualityCheckVisitor::JaniSyntacticalEqualityCheckVisitor() : SyntacticalEqualityCheckVisitor() {
            // Intentionally left empty.
        }
        
        boost::any JaniSyntacticalEqualityCheckVisitor::visit(ValueArrayExpression const& expression, boost::any const& data) {
            if (data.type() == typeid(ValueArrayExpression)) {
                auto other = boost::any_cast<ValueArrayExpression>(data);
                return expression.getType() == other.getType()
                       && boost::any_cast<bool>(visit(expression.getElements(), other.getElements()));
            }
            return false;
        }

        boost::any JaniSyntacticalEqualityCheckVisitor::visit(ValueArrayExpression::ValueArrayElements const& elements, boost::any const& data) {
            if (data.type() == typeid(ValueArrayExpression::ValueArrayElements)) {
                auto other = boost::any_cast<ValueArrayExpression::ValueArrayElements>(data);
                if (elements.elementsWithValue) {
                    bool result = other.elementsWithValue && elements.elementsWithValue->size() == other.elementsWithValue->size();
                    for (auto i = 0; result && i < elements.elementsOfElements->size(); ++i) {
                        result &= boost::any_cast<bool>(elements.elementsWithValue->at(i)->accept(*this, other.elementsWithValue->at(i)));
                    }
                    return result;
                } else {
                    bool result = other.elementsOfElements && elements.elementsOfElements->size() == other.elementsOfElements->size();
                    for (auto i = 0; result && i < elements.elementsOfElements->size(); ++i) {
                        result &= boost::any_cast<bool>(visit(*elements.elementsOfElements->at(i), *other.elementsOfElements->at(i)));
                    }
                    return result;
                }
            }
            return false;
        }
    
        boost::any JaniSyntacticalEqualityCheckVisitor::visit(ConstructorArrayExpression const& expression, boost::any const& data) {
            assert (false);
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

        boost::any JaniSyntacticalEqualityCheckVisitor::visit(ArrayAccessExpression const& expression, boost::any const& data) {
            if (data.type() == typeid(ArrayAccessExpression)) {
                auto other = boost::any_cast<ArrayAccessExpression>(data);
                return expression.getType() == other.getType()
                    && boost::any_cast<bool>(expression.getFirstOperand()->accept(*this, other.getFirstOperand()))
                    && boost::any_cast<bool>(expression.getSecondOperand()->accept(*this, other.getSecondOperand()));
            } else {
                return false;
            }
        }

        boost::any JaniSyntacticalEqualityCheckVisitor::visit(ArrayAccessIndexExpression const& expression, boost::any const& data) {
            if (data.type() == typeid(ArrayAccessExpression)) {
                auto other = boost::any_cast<ArrayAccessExpression>(data);
                if (expression.getFirstOperand() == expression.getSecondOperand()) {
                    return expression.getFirstOperand()->accept(*this, other.getFirstOperand());
                } else {
                    return boost::any_cast<bool>(expression.getFirstOperand()->accept(*this, other.getFirstOperand())) && boost::any_cast<bool>(expression.getSecondOperand()->accept(*this, other.getSecondOperand()));
                }
            } else {
                return false;
            }
        }

        boost::any JaniSyntacticalEqualityCheckVisitor::visit(FunctionCallExpression const& expression, boost::any const& data) {
            STORM_LOG_ASSERT(false, "SyntacticalEquality for function calls not yet implemented");
            std::vector<std::shared_ptr<BaseExpression const>> newArguments;
            newArguments.reserve(expression.getNumberOfArguments());
            for (uint64_t i = 0; i < expression.getNumberOfArguments(); ++i) {
                newArguments.push_back(boost::any_cast<std::shared_ptr<BaseExpression const>>(expression.getArgument(i)->accept(*this, data)));
            }
            return std::const_pointer_cast<BaseExpression const>(std::shared_ptr<BaseExpression>(new FunctionCallExpression(expression.getManager(), expression.getType(), expression.getFunctionIdentifier(), newArguments)));
        }
    }
}
