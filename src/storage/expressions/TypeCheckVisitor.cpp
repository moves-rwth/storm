#include "src/storage/expressions/TypeCheckVisitor.h"
#include "src/storage/expressions/Expressions.h"

#include "src/exceptions/ExceptionMacros.h"
#include "src/exceptions/InvalidTypeException.h"

namespace storm {
    namespace expressions {
        template<typename MapType>
        TypeCheckVisitor<MapType>::TypeCheckVisitor(MapType const& identifierToTypeMap) : identifierToTypeMap(identifierToTypeMap) {
            // Intentionally left empty.
        }
        
        template<typename MapType>
        void TypeCheckVisitor<MapType>::check(BaseExpression const* expression) {
            expression->accept(this);
        }
        
        template<typename MapType>
        void TypeCheckVisitor<MapType>::visit(IfThenElseExpression const* expression) {
            expression->getCondition()->accept(this);
            expression->getThenExpression()->accept(this);
            expression->getElseExpression()->accept(this);
        }
        
        template<typename MapType>
        void TypeCheckVisitor<MapType>::visit(BinaryBooleanFunctionExpression const* expression) {
            expression->getFirstOperand()->accept(this);
            expression->getSecondOperand()->accept(this);
        }
        
        template<typename MapType>
        void TypeCheckVisitor<MapType>::visit(BinaryNumericalFunctionExpression const* expression) {
            expression->getFirstOperand()->accept(this);
            expression->getSecondOperand()->accept(this);
        }
        
        template<typename MapType>
        void TypeCheckVisitor<MapType>::visit(BinaryRelationExpression const* expression) {
            expression->getFirstOperand()->accept(this);
            expression->getSecondOperand()->accept(this);
        }
        
        template<typename MapType>
        void TypeCheckVisitor<MapType>::visit(BooleanConstantExpression const* expression) {
            auto identifierTypePair = this->identifierToTypeMap.find(expression->getConstantName());
            LOG_THROW(identifierTypePair != this->identifierToTypeMap.end(), storm::exceptions::InvalidArgumentException, "No type available for identifier '" << expression->getConstantName() << "'.");
            LOG_THROW(identifierTypePair->second == ExpressionReturnType::Bool, storm::exceptions::InvalidTypeException, "Type mismatch for constant '" << expression->getConstantName() << "': expected bool, but found " << expression->getReturnType() << ".");
        }
        
        template<typename MapType>
        void TypeCheckVisitor<MapType>::visit(DoubleConstantExpression const* expression) {
            auto identifierTypePair = this->identifierToTypeMap.find(expression->getConstantName());
            LOG_THROW(identifierTypePair != this->identifierToTypeMap.end(), storm::exceptions::InvalidArgumentException, "No type available for identifier '" << expression->getConstantName() << "'.");
            LOG_THROW(identifierTypePair->second == ExpressionReturnType::Bool, storm::exceptions::InvalidTypeException, "Type mismatch for constant '" << expression->getConstantName() << "': expected double, but found " << expression->getReturnType() << ".");
        }
        
        template<typename MapType>
        void TypeCheckVisitor<MapType>::visit(IntegerConstantExpression const* expression) {
            auto identifierTypePair = this->identifierToTypeMap.find(expression->getConstantName());
            LOG_THROW(identifierTypePair != this->identifierToTypeMap.end(), storm::exceptions::InvalidArgumentException, "No type available for identifier '" << expression->getConstantName() << "'.");
            LOG_THROW(identifierTypePair->second == ExpressionReturnType::Bool, storm::exceptions::InvalidTypeException, "Type mismatch for constant '" << expression->getConstantName() << "': expected int, but found " << expression->getReturnType() << ".");
        }
        
        template<typename MapType>
        void TypeCheckVisitor<MapType>::visit(VariableExpression const* expression) {
            auto identifierTypePair = this->identifierToTypeMap.find(expression->getVariableName());
            LOG_THROW(identifierTypePair != this->identifierToTypeMap.end(), storm::exceptions::InvalidArgumentException, "No type available for identifier '" << expression->getVariableName() << "'.");
            LOG_THROW(identifierTypePair->second == ExpressionReturnType::Bool, storm::exceptions::InvalidTypeException, "Type mismatch for variable '" << expression->getVariableName() << "': expected " << identifierTypePair->first << ", but found " << expression->getReturnType() << ".");
        }
        
        template<typename MapType>
        void TypeCheckVisitor<MapType>::visit(UnaryBooleanFunctionExpression const* expression) {
            expression->getOperand()->accept(this);
        }
        
        template<typename MapType>
        void TypeCheckVisitor<MapType>::visit(UnaryNumericalFunctionExpression const* expression) {
            expression->getOperand()->accept(this);
        }
        
        template<typename MapType>
        void TypeCheckVisitor<MapType>::visit(BooleanLiteralExpression const* expression) {
            // Intentionally left empty.
        }
        
        template<typename MapType>
        void TypeCheckVisitor<MapType>::visit(IntegerLiteralExpression const* expression) {
            // Intentionally left empty.
        }
        
        template<typename MapType>
        void TypeCheckVisitor<MapType>::visit(DoubleLiteralExpression const* expression) {
            // Intentionally left empty.
        }
    }
}