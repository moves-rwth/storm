#include <map>
#include <unordered_map>

#include "src/storage/expressions/Expression.h"
#include "src/storage/expressions/SubstitutionVisitor.h"
#include "src/storage/expressions/IdentifierSubstitutionVisitor.h"
#include "src/exceptions/InvalidTypeException.h"
#include "src/exceptions/ExceptionMacros.h"

#include "src/storage/expressions/IfThenElseExpression.h"
#include "src/storage/expressions/BinaryBooleanFunctionExpression.h"
#include "src/storage/expressions/BinaryNumericalFunctionExpression.h"
#include "src/storage/expressions/BinaryRelationExpression.h"
#include "src/storage/expressions/BooleanConstantExpression.h"
#include "src/storage/expressions/IntegerConstantExpression.h"
#include "src/storage/expressions/DoubleConstantExpression.h"
#include "src/storage/expressions/BooleanLiteralExpression.h"
#include "src/storage/expressions/IntegerLiteralExpression.h"
#include "src/storage/expressions/DoubleLiteralExpression.h"
#include "src/storage/expressions/VariableExpression.h"
#include "src/storage/expressions/UnaryBooleanFunctionExpression.h"
#include "src/storage/expressions/UnaryNumericalFunctionExpression.h"

namespace storm {
    namespace expressions {
        Expression::Expression(std::shared_ptr<BaseExpression const> const& expressionPtr) : expressionPtr(expressionPtr) {
            // Intentionally left empty.
        }
        
        template<template<typename... Arguments> class MapType>
        Expression Expression::substitute(MapType<std::string, Expression> const& identifierToExpressionMap) const {
            return SubstitutionVisitor<MapType>(identifierToExpressionMap).substitute(this->getBaseExpressionPointer().get());
        }
        
        template<template<typename... Arguments> class MapType>
        Expression Expression::substitute(MapType<std::string, std::string> const& identifierToIdentifierMap) const {
            return IdentifierSubstitutionVisitor<MapType>(identifierToIdentifierMap).substitute(this->getBaseExpressionPointer().get());
        }
        
        bool Expression::evaluateAsBool(Valuation const* valuation) const {
            return this->getBaseExpression().evaluateAsBool(valuation);
        }
        
        int_fast64_t Expression::evaluateAsInt(Valuation const* valuation) const {
            return this->getBaseExpression().evaluateAsInt(valuation);
        }
        
        double Expression::evaluateAsDouble(Valuation const* valuation) const {
            return this->getBaseExpression().evaluateAsDouble(valuation);
        }
        
        Expression Expression::simplify() {
            return Expression(this->getBaseExpression().simplify());
        }
        
        bool Expression::isConstant() const {
            return this->getBaseExpression().isConstant();
        }
        
        bool Expression::isTrue() const {
            return this->getBaseExpression().isTrue();
        }
        
        bool Expression::isFalse() const {
            return this->getBaseExpression().isFalse();
        }
        
        std::set<std::string> Expression::getVariables() const {
            return this->getBaseExpression().getVariables();
        }
        
        std::set<std::string> Expression::getConstants() const {
            return this->getBaseExpression().getConstants();
        }
        
        BaseExpression const& Expression::getBaseExpression() const {
            return *this->expressionPtr;
        }
        
        std::shared_ptr<BaseExpression const> const& Expression::getBaseExpressionPointer() const {
            return this->expressionPtr;
        }
        
        ExpressionReturnType Expression::getReturnType() const {
            return this->getBaseExpression().getReturnType();
        }
        
        bool Expression::hasNumericalReturnType() const {
            return this->getReturnType() == ExpressionReturnType::Int || this->getReturnType() == ExpressionReturnType::Double;
        }
        
        bool Expression::hasBooleanReturnType() const {
            return this->getReturnType() == ExpressionReturnType::Bool;
        }
        
        Expression Expression::createBooleanLiteral(bool value) {
            return Expression(std::shared_ptr<BaseExpression>(new BooleanLiteralExpression(value)));
        }
        
        Expression Expression::createTrue() {
            return createBooleanLiteral(true);
        }
        
        Expression Expression::createFalse() {
            return createBooleanLiteral(false);
        }
        
        Expression Expression::createIntegerLiteral(int_fast64_t value) {
            return Expression(std::shared_ptr<BaseExpression>(new IntegerLiteralExpression(value)));
        }
        
        Expression Expression::createDoubleLiteral(double value) {
            return Expression(std::shared_ptr<BaseExpression>(new DoubleLiteralExpression(value)));
        }
        
        Expression Expression::createBooleanVariable(std::string const& variableName) {
            return Expression(std::shared_ptr<BaseExpression>(new VariableExpression(ExpressionReturnType::Bool, variableName)));
        }
        
        Expression Expression::createIntegerVariable(std::string const& variableName) {
            return Expression(std::shared_ptr<BaseExpression>(new VariableExpression(ExpressionReturnType::Int, variableName)));
        }
        
        Expression Expression::createDoubleVariable(std::string const& variableName) {
            return Expression(std::shared_ptr<BaseExpression>(new VariableExpression(ExpressionReturnType::Double, variableName)));
        }
        
        Expression Expression::createUndefinedVariable(std::string const& variableName) {
            return Expression(std::shared_ptr<BaseExpression>(new VariableExpression(ExpressionReturnType::Undefined, variableName)));
        }
        
        Expression Expression::createBooleanConstant(std::string const& constantName) {
            return Expression(std::shared_ptr<BaseExpression>(new BooleanConstantExpression(constantName)));
        }
        
        Expression Expression::createIntegerConstant(std::string const& constantName) {
            return Expression(std::shared_ptr<BaseExpression>(new IntegerConstantExpression(constantName)));
        }
        
        Expression Expression::createDoubleConstant(std::string const& constantName) {
            return Expression(std::shared_ptr<BaseExpression>(new DoubleConstantExpression(constantName)));
        }
        
        Expression Expression::operator+(Expression const& other) const {
            LOG_THROW(this->hasNumericalReturnType() && other.hasNumericalReturnType(), storm::exceptions::InvalidTypeException, "Operator '+' requires numerical operands.");
            return Expression(std::shared_ptr<BaseExpression>(new BinaryNumericalFunctionExpression(this->getReturnType() == ExpressionReturnType::Int && other.getReturnType() == ExpressionReturnType::Int ? ExpressionReturnType::Int : ExpressionReturnType::Double, this->getBaseExpressionPointer(), other.getBaseExpressionPointer(), BinaryNumericalFunctionExpression::OperatorType::Plus)));
        }
        
        Expression Expression::operator-(Expression const& other) const {
            LOG_THROW(this->hasNumericalReturnType() && other.hasNumericalReturnType(), storm::exceptions::InvalidTypeException, "Operator '-' requires numerical operands.");
            return Expression(std::shared_ptr<BaseExpression>(new BinaryNumericalFunctionExpression(this->getReturnType() == ExpressionReturnType::Int && other.getReturnType() == ExpressionReturnType::Int ? ExpressionReturnType::Int : ExpressionReturnType::Double, this->getBaseExpressionPointer(), other.getBaseExpressionPointer(), BinaryNumericalFunctionExpression::OperatorType::Minus)));
        }
        
        Expression Expression::operator-() const {
            LOG_THROW(this->hasNumericalReturnType(), storm::exceptions::InvalidTypeException, "Operator '-' requires numerical operand.");
            return Expression(std::shared_ptr<BaseExpression>(new UnaryNumericalFunctionExpression(this->getReturnType(), this->getBaseExpressionPointer(), UnaryNumericalFunctionExpression::OperatorType::Minus)));
        }
        
        Expression Expression::operator*(Expression const& other) const {
            LOG_THROW(this->hasNumericalReturnType() && other.hasNumericalReturnType(), storm::exceptions::InvalidTypeException, "Operator '*' requires numerical operands.");
            return Expression(std::shared_ptr<BaseExpression>(new BinaryNumericalFunctionExpression(this->getReturnType() == ExpressionReturnType::Int && other.getReturnType() == ExpressionReturnType::Int ? ExpressionReturnType::Int : ExpressionReturnType::Double, this->getBaseExpressionPointer(), other.getBaseExpressionPointer(), BinaryNumericalFunctionExpression::OperatorType::Times)));
        }
        
        Expression Expression::operator/(Expression const& other) const {
            LOG_THROW(this->hasNumericalReturnType() && other.hasNumericalReturnType(), storm::exceptions::InvalidTypeException, "Operator '/' requires numerical operands.");
            return Expression(std::shared_ptr<BaseExpression>(new BinaryNumericalFunctionExpression(this->getReturnType() == ExpressionReturnType::Int && other.getReturnType() == ExpressionReturnType::Int ? ExpressionReturnType::Int : ExpressionReturnType::Double, this->getBaseExpressionPointer(), other.getBaseExpressionPointer(), BinaryNumericalFunctionExpression::OperatorType::Divide)));
        }
        
        Expression Expression::operator&&(Expression const& other) const {
            LOG_THROW(this->hasBooleanReturnType() && other.hasBooleanReturnType(), storm::exceptions::InvalidTypeException, "Operator '&&' requires boolean operands.");
            return Expression(std::shared_ptr<BaseExpression>(new BinaryBooleanFunctionExpression(ExpressionReturnType::Bool, this->getBaseExpressionPointer(), other.getBaseExpressionPointer(), BinaryBooleanFunctionExpression::OperatorType::And)));
        }
        
        Expression Expression::operator||(Expression const& other) const {
            LOG_THROW(this->hasBooleanReturnType() && other.hasBooleanReturnType(), storm::exceptions::InvalidTypeException, "Operator '||' requires numerical operands.");
            return Expression(std::shared_ptr<BaseExpression>(new BinaryBooleanFunctionExpression(ExpressionReturnType::Bool, this->getBaseExpressionPointer(), other.getBaseExpressionPointer(), BinaryBooleanFunctionExpression::OperatorType::Or)));
        }
        
        Expression Expression::operator!() const {
            LOG_THROW(this->hasBooleanReturnType(), storm::exceptions::InvalidTypeException, "Operator '!' requires boolean operand.");
            return Expression(std::shared_ptr<BaseExpression>(new UnaryBooleanFunctionExpression(ExpressionReturnType::Bool, this->getBaseExpressionPointer(), UnaryBooleanFunctionExpression::OperatorType::Not)));
        }
        
        Expression Expression::operator==(Expression const& other) const {
            LOG_THROW(this->hasNumericalReturnType() && other.hasNumericalReturnType(), storm::exceptions::InvalidTypeException, "Operator '==' requires numerical operands.");
            return Expression(std::shared_ptr<BaseExpression>(new BinaryRelationExpression(ExpressionReturnType::Bool, this->getBaseExpressionPointer(), other.getBaseExpressionPointer(), BinaryRelationExpression::RelationType::Equal)));
        }
        
        Expression Expression::operator!=(Expression const& other) const {
            LOG_THROW(this->hasNumericalReturnType() && other.hasNumericalReturnType(), storm::exceptions::InvalidTypeException, "Operator '!=' requires numerical operands.");
            return Expression(std::shared_ptr<BaseExpression>(new BinaryRelationExpression(ExpressionReturnType::Bool, this->getBaseExpressionPointer(), other.getBaseExpressionPointer(), BinaryRelationExpression::RelationType::NotEqual)));
        }
        
        Expression Expression::operator>(Expression const& other) const {
            LOG_THROW(this->hasNumericalReturnType() && other.hasNumericalReturnType(), storm::exceptions::InvalidTypeException, "Operator '>' requires numerical operands.");
            return Expression(std::shared_ptr<BaseExpression>(new BinaryRelationExpression(ExpressionReturnType::Bool, this->getBaseExpressionPointer(), other.getBaseExpressionPointer(), BinaryRelationExpression::RelationType::Greater)));
        }
        
        Expression Expression::operator>=(Expression const& other) const {
            LOG_THROW(this->hasNumericalReturnType() && other.hasNumericalReturnType(), storm::exceptions::InvalidTypeException, "Operator '>=' requires numerical operands.");
            return Expression(std::shared_ptr<BaseExpression>(new BinaryRelationExpression(ExpressionReturnType::Bool, this->getBaseExpressionPointer(), other.getBaseExpressionPointer(), BinaryRelationExpression::RelationType::GreaterOrEqual)));
        }
        
        Expression Expression::operator<(Expression const& other) const {
            LOG_THROW(this->hasNumericalReturnType() && other.hasNumericalReturnType(), storm::exceptions::InvalidTypeException, "Operator '<' requires numerical operands.");
            return Expression(std::shared_ptr<BaseExpression>(new BinaryRelationExpression(ExpressionReturnType::Bool, this->getBaseExpressionPointer(), other.getBaseExpressionPointer(), BinaryRelationExpression::RelationType::Less)));
        }
        
        Expression Expression::operator<=(Expression const& other) const {
            LOG_THROW(this->hasNumericalReturnType() && other.hasNumericalReturnType(), storm::exceptions::InvalidTypeException, "Operator '<=' requires numerical operands.");
            return Expression(std::shared_ptr<BaseExpression>(new BinaryRelationExpression(ExpressionReturnType::Bool, this->getBaseExpressionPointer(), other.getBaseExpressionPointer(), BinaryRelationExpression::RelationType::LessOrEqual)));
        }
        
        Expression Expression::minimum(Expression const& lhs, Expression const& rhs) {
            LOG_THROW(lhs.hasNumericalReturnType() && rhs.hasNumericalReturnType(), storm::exceptions::InvalidTypeException, "Operator 'min' requires numerical operands.");
            return Expression(std::shared_ptr<BaseExpression>(new BinaryNumericalFunctionExpression(lhs.getReturnType() == ExpressionReturnType::Int && rhs.getReturnType() == ExpressionReturnType::Int ? ExpressionReturnType::Int : ExpressionReturnType::Double, lhs.getBaseExpressionPointer(), rhs.getBaseExpressionPointer(), BinaryNumericalFunctionExpression::OperatorType::Min)));
        }
        
        Expression Expression::maximum(Expression const& lhs, Expression const& rhs) {
            LOG_THROW(lhs.hasNumericalReturnType() && rhs.hasNumericalReturnType(), storm::exceptions::InvalidTypeException, "Operator 'max' requires numerical operands.");
            return Expression(std::shared_ptr<BaseExpression>(new BinaryNumericalFunctionExpression(lhs.getReturnType() == ExpressionReturnType::Int && rhs.getReturnType() == ExpressionReturnType::Int ? ExpressionReturnType::Int : ExpressionReturnType::Double, lhs.getBaseExpressionPointer(), rhs.getBaseExpressionPointer(), BinaryNumericalFunctionExpression::OperatorType::Max)));
        }
        
        Expression Expression::ite(Expression const& thenExpression, Expression const& elseExpression) {
            LOG_THROW(this->hasBooleanReturnType(), storm::exceptions::InvalidTypeException, "Condition of if-then-else operator must be of boolean type.");
            LOG_THROW(thenExpression.hasBooleanReturnType() && elseExpression.hasBooleanReturnType() || thenExpression.hasNumericalReturnType() && elseExpression.hasNumericalReturnType(), storm::exceptions::InvalidTypeException, "'then' and 'else' expression of if-then-else operator must have equal return type.");
            return Expression(std::shared_ptr<BaseExpression>(new IfThenElseExpression(thenExpression.hasBooleanReturnType() && elseExpression.hasBooleanReturnType() ? ExpressionReturnType::Bool : (thenExpression.getReturnType() == ExpressionReturnType::Int && elseExpression.getReturnType() == ExpressionReturnType::Int ? ExpressionReturnType::Int : ExpressionReturnType::Double), this->getBaseExpressionPointer(), thenExpression.getBaseExpressionPointer(), elseExpression.getBaseExpressionPointer())));
        }
        
        Expression Expression::implies(Expression const& other) const {
            LOG_THROW(this->hasBooleanReturnType() && other.hasBooleanReturnType(), storm::exceptions::InvalidTypeException, "Operator '&&' requires boolean operands.");
            return Expression(std::shared_ptr<BaseExpression>(new BinaryBooleanFunctionExpression(ExpressionReturnType::Bool, this->getBaseExpressionPointer(), other.getBaseExpressionPointer(), BinaryBooleanFunctionExpression::OperatorType::Implies)));
        }
        
        Expression Expression::iff(Expression const& other) const {
            LOG_THROW(this->hasBooleanReturnType() && other.hasBooleanReturnType(), storm::exceptions::InvalidTypeException, "Operator '&&' requires boolean operands.");
            return Expression(std::shared_ptr<BaseExpression>(new BinaryBooleanFunctionExpression(ExpressionReturnType::Bool, this->getBaseExpressionPointer(), other.getBaseExpressionPointer(), BinaryBooleanFunctionExpression::OperatorType::Iff)));
        }
        
        Expression Expression::floor() const {
            LOG_THROW(this->hasNumericalReturnType(), storm::exceptions::InvalidTypeException, "Operator 'floor' requires numerical operand.");
            return Expression(std::shared_ptr<BaseExpression>(new UnaryNumericalFunctionExpression(ExpressionReturnType::Int, this->getBaseExpressionPointer(), UnaryNumericalFunctionExpression::OperatorType::Floor)));
        }
        
        Expression Expression::ceil() const {
            LOG_THROW(this->hasNumericalReturnType(), storm::exceptions::InvalidTypeException, "Operator 'ceil' requires numerical operand.");
            return Expression(std::shared_ptr<BaseExpression>(new UnaryNumericalFunctionExpression(ExpressionReturnType::Int, this->getBaseExpressionPointer(), UnaryNumericalFunctionExpression::OperatorType::Ceil)));
        }
        
        template Expression Expression::substitute<std::map>(std::map<std::string, storm::expressions::Expression> const&) const;
        template Expression Expression::substitute<std::unordered_map>(std::unordered_map<std::string, storm::expressions::Expression> const&) const;
        template Expression Expression::substitute<std::map>(std::map<std::string, std::string> const&) const;
        template Expression Expression::substitute<std::unordered_map>(std::unordered_map<std::string, std::string> const&) const;
        
        std::ostream& operator<<(std::ostream& stream, Expression const& expression) {
            stream << expression.getBaseExpression();
            return stream;
        }
    }
}
