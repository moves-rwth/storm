#include <map>
#include <unordered_map>

#include "src/storage/expressions/Expression.h"
#include "src/storage/expressions/ExpressionManager.h"
#include "src/storage/expressions/SubstitutionVisitor.h"
#include "src/storage/expressions/LinearityCheckVisitor.h"
#include "src/storage/expressions/Expressions.h"
#include "src/exceptions/InvalidTypeException.h"
#include "src/utility/macros.h"

namespace storm {
    namespace expressions {
        /*!
         * Checks whether the two expressions share the same expression manager.
         *
         * @param a The first expression.
         * @param b The second expression.
         * @return True iff the expressions share the same manager.
         */
        static void assertSameManager(BaseExpression const& a, BaseExpression const& b) {
            STORM_LOG_THROW(a.getManager() == b.getManager(), storm::exceptions::InvalidArgumentException, "Expressions are managed by different manager.");
        }
        
        Expression::Expression(std::shared_ptr<BaseExpression const> const& expressionPtr) : expressionPtr(expressionPtr) {
            // Intentionally left empty.
        }
        
        Expression::Expression(Variable const& variable) : expressionPtr(std::shared_ptr<BaseExpression>(new VariableExpression(variable))) {
            // Intentionally left empty.
        }
        
		Expression Expression::substitute(std::map<Variable, Expression> const& identifierToExpressionMap) const {
            return SubstitutionVisitor<std::map<Variable, Expression>>(identifierToExpressionMap).substitute(*this);
        }

		Expression Expression::substitute(std::unordered_map<Variable, Expression> const& identifierToExpressionMap) const {
			return SubstitutionVisitor<std::unordered_map<Variable, Expression>>(identifierToExpressionMap).substitute(*this);
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
        
        Expression Expression::simplify() const {
            return Expression(this->getBaseExpression().simplify());
        }
        
        OperatorType Expression::getOperator() const {
            return this->getBaseExpression().getOperator();
        }
        
        bool Expression::isFunctionApplication() const {
            return this->getBaseExpression().isFunctionApplication();
        }
        
        uint_fast64_t Expression::getArity() const {
            return this->getBaseExpression().getArity();
        }
        
        Expression Expression::getOperand(uint_fast64_t operandIndex) const {
            return Expression(this->getBaseExpression().getOperand(operandIndex));
        }
        
        std::string const& Expression::getIdentifier() const {
            return this->getBaseExpression().getIdentifier();
        }
        
        bool Expression::containsVariables() const {
            return this->getBaseExpression().containsVariables();
        }
        
        bool Expression::isLiteral() const {
            return this->getBaseExpression().isLiteral();
        }
        
        bool Expression::isVariable() const {
            return this->getBaseExpression().isVariable();
        }
        
        bool Expression::isTrue() const {
            return this->getBaseExpression().isTrue();
        }
        
        bool Expression::isFalse() const {
            return this->getBaseExpression().isFalse();
        }

		std::set<storm::expressions::Variable> Expression::getVariables() const {
            std::set<storm::expressions::Variable> result;
			this->getBaseExpression().gatherVariables(result);
            return result;
		}
        
        bool Expression::isRelationalExpression() const {
            if (!this->isFunctionApplication()) {
                return false;
            }
            
            return this->getOperator() == OperatorType::Equal || this->getOperator() == OperatorType::NotEqual
            || this->getOperator() == OperatorType::Less || this->getOperator() == OperatorType::LessOrEqual
            || this->getOperator() == OperatorType::Greater || this->getOperator() == OperatorType::GreaterOrEqual;
        }
        
        bool Expression::isLinear() const {
            return LinearityCheckVisitor().check(*this);
        }
        
        BaseExpression const& Expression::getBaseExpression() const {
            return *this->expressionPtr;
        }
        
        std::shared_ptr<BaseExpression const> const& Expression::getBaseExpressionPointer() const {
            return this->expressionPtr;
        }
        
        ExpressionManager const& Expression::getManager() const {
            return this->getBaseExpression().getManager();
        }
        
        Type const& Expression::getType() const {
            return this->getBaseExpression().getType();
        }
        
        bool Expression::hasNumericalType() const {
            return this->getBaseExpression().hasNumericalType();
        }
        
        bool Expression::hasRationalType() const {
            return this->getBaseExpression().hasRationalType();
        }
        
        bool Expression::hasBooleanType() const {
            return this->getBaseExpression().hasBooleanType();
        }
        
        bool Expression::hasIntegerType() const {
            return this->getBaseExpression().hasIntegerType();
        }
        
        bool Expression::hasBitVectorType() const {
            return this->getBaseExpression().hasBitVectorType();
        }
        
        boost::any Expression::accept(ExpressionVisitor& visitor) const {
            return this->getBaseExpression().accept(visitor);
        }
        
        std::ostream& operator<<(std::ostream& stream, Expression const& expression) {
            stream << expression.getBaseExpression();
            return stream;
        }
        
        Expression operator+(Expression const& first, Expression const& second) {
            assertSameManager(first.getBaseExpression(), second.getBaseExpression());
            return Expression(std::shared_ptr<BaseExpression>(new BinaryNumericalFunctionExpression(first.getManager(), first.getType().plusMinusTimes(second.getType()), first.getBaseExpressionPointer(), second.getBaseExpressionPointer(), BinaryNumericalFunctionExpression::OperatorType::Plus)));
        }
        
        Expression operator-(Expression const& first, Expression const& second) {
            assertSameManager(first.getBaseExpression(), second.getBaseExpression());
            return Expression(std::shared_ptr<BaseExpression>(new BinaryNumericalFunctionExpression(first.getBaseExpression().getManager(), first.getType().plusMinusTimes(second.getType()), first.getBaseExpressionPointer(), second.getBaseExpressionPointer(), BinaryNumericalFunctionExpression::OperatorType::Minus)));
        }
        
        Expression operator-(Expression const& first) {
            return Expression(std::shared_ptr<BaseExpression>(new UnaryNumericalFunctionExpression(first.getBaseExpression().getManager(), first.getType().minus(), first.getBaseExpressionPointer(), UnaryNumericalFunctionExpression::OperatorType::Minus)));
        }
        
        Expression operator*(Expression const& first, Expression const& second) {
            assertSameManager(first.getBaseExpression(), second.getBaseExpression());
            return Expression(std::shared_ptr<BaseExpression>(new BinaryNumericalFunctionExpression(first.getBaseExpression().getManager(), first.getType().plusMinusTimes(second.getType()), first.getBaseExpressionPointer(), second.getBaseExpressionPointer(), BinaryNumericalFunctionExpression::OperatorType::Times)));
        }
        
        Expression operator/(Expression const& first, Expression const& second) {
            assertSameManager(first.getBaseExpression(), second.getBaseExpression());
            return Expression(std::shared_ptr<BaseExpression>(new BinaryNumericalFunctionExpression(first.getBaseExpression().getManager(), first.getType().divide(second.getType()), first.getBaseExpressionPointer(), second.getBaseExpressionPointer(), BinaryNumericalFunctionExpression::OperatorType::Divide)));
        }
        
        Expression operator^(Expression const& first, Expression const& second) {
            assertSameManager(first.getBaseExpression(), second.getBaseExpression());
            return Expression(std::shared_ptr<BaseExpression>(new BinaryNumericalFunctionExpression(first.getBaseExpression().getManager(), first.getType().power(second.getType()), first.getBaseExpressionPointer(), second.getBaseExpressionPointer(), BinaryNumericalFunctionExpression::OperatorType::Power)));
        }
        
        Expression operator&&(Expression const& first, Expression const& second) {
            assertSameManager(first.getBaseExpression(), second.getBaseExpression());
            return Expression(std::shared_ptr<BaseExpression>(new BinaryBooleanFunctionExpression(first.getBaseExpression().getManager(), first.getType().logicalConnective(second.getType()), first.getBaseExpressionPointer(), second.getBaseExpressionPointer(), BinaryBooleanFunctionExpression::OperatorType::And)));
        }
        
        Expression operator||(Expression const& first, Expression const& second) {
            assertSameManager(first.getBaseExpression(), second.getBaseExpression());
            return Expression(std::shared_ptr<BaseExpression>(new BinaryBooleanFunctionExpression(first.getBaseExpression().getManager(), first.getType().logicalConnective(second.getType()), first.getBaseExpressionPointer(), second.getBaseExpressionPointer(), BinaryBooleanFunctionExpression::OperatorType::Or)));
        }
        
        Expression operator!(Expression const& first) {
            return Expression(std::shared_ptr<BaseExpression>(new UnaryBooleanFunctionExpression(first.getBaseExpression().getManager(), first.getType().logicalConnective(), first.getBaseExpressionPointer(), UnaryBooleanFunctionExpression::OperatorType::Not)));
        }
        
        Expression operator==(Expression const& first, Expression const& second) {
            assertSameManager(first.getBaseExpression(), second.getBaseExpression());
            return Expression(std::shared_ptr<BaseExpression>(new BinaryRelationExpression(first.getBaseExpression().getManager(), first.getType().numericalComparison(second.getType()), first.getBaseExpressionPointer(), second.getBaseExpressionPointer(), BinaryRelationExpression::RelationType::Equal)));
        }
        
        Expression operator!=(Expression const& first, Expression const& second) {
            assertSameManager(first.getBaseExpression(), second.getBaseExpression());
            if (first.hasNumericalType() && second.hasNumericalType()) {
                return Expression(std::shared_ptr<BaseExpression>(new BinaryRelationExpression(first.getBaseExpression().getManager(), first.getType().numericalComparison(second.getType()), first.getBaseExpressionPointer(), second.getBaseExpressionPointer(), BinaryRelationExpression::RelationType::NotEqual)));
            } else {
                return Expression(std::shared_ptr<BaseExpression>(new BinaryBooleanFunctionExpression(first.getBaseExpression().getManager(), first.getType().logicalConnective(second.getType()), first.getBaseExpressionPointer(), second.getBaseExpressionPointer(), BinaryBooleanFunctionExpression::OperatorType::Xor)));
            }
        }
        
        Expression operator>(Expression const& first, Expression const& second) {
            assertSameManager(first.getBaseExpression(), second.getBaseExpression());
            return Expression(std::shared_ptr<BaseExpression>(new BinaryRelationExpression(first.getBaseExpression().getManager(), first.getType().numericalComparison(second.getType()), first.getBaseExpressionPointer(), second.getBaseExpressionPointer(), BinaryRelationExpression::RelationType::Greater)));
        }
        
        Expression operator>=(Expression const& first, Expression const& second) {
            assertSameManager(first.getBaseExpression(), second.getBaseExpression());
            return Expression(std::shared_ptr<BaseExpression>(new BinaryRelationExpression(first.getBaseExpression().getManager(), first.getType().numericalComparison(second.getType()), first.getBaseExpressionPointer(), second.getBaseExpressionPointer(), BinaryRelationExpression::RelationType::GreaterOrEqual)));
        }
        
        Expression operator<(Expression const& first, Expression const& second) {
            assertSameManager(first.getBaseExpression(), second.getBaseExpression());
            return Expression(std::shared_ptr<BaseExpression>(new BinaryRelationExpression(first.getBaseExpression().getManager(), first.getType().numericalComparison(second.getType()), first.getBaseExpressionPointer(), second.getBaseExpressionPointer(), BinaryRelationExpression::RelationType::Less)));
        }
        
        Expression operator<=(Expression const& first, Expression const& second) {
            assertSameManager(first.getBaseExpression(), second.getBaseExpression());
            return Expression(std::shared_ptr<BaseExpression>(new BinaryRelationExpression(first.getBaseExpression().getManager(), first.getType().numericalComparison(second.getType()), first.getBaseExpressionPointer(), second.getBaseExpressionPointer(), BinaryRelationExpression::RelationType::LessOrEqual)));
        }

        Expression minimum(Expression const& first, Expression const& second) {
            assertSameManager(first.getBaseExpression(), second.getBaseExpression());
            return Expression(std::shared_ptr<BaseExpression>(new BinaryNumericalFunctionExpression(first.getBaseExpression().getManager(), first.getType().minimumMaximum(second.getType()), first.getBaseExpressionPointer(), second.getBaseExpressionPointer(), BinaryNumericalFunctionExpression::OperatorType::Min)));
        }
        
        Expression maximum(Expression const& first, Expression const& second) {
            assertSameManager(first.getBaseExpression(), second.getBaseExpression());
            return Expression(std::shared_ptr<BaseExpression>(new BinaryNumericalFunctionExpression(first.getBaseExpression().getManager(), first.getType().minimumMaximum(second.getType()), first.getBaseExpressionPointer(), second.getBaseExpressionPointer(), BinaryNumericalFunctionExpression::OperatorType::Max)));
        }
        
        Expression ite(Expression const& condition, Expression const& thenExpression, Expression const& elseExpression) {
            assertSameManager(condition.getBaseExpression(), thenExpression.getBaseExpression());
            assertSameManager(thenExpression.getBaseExpression(), elseExpression.getBaseExpression());
            return Expression(std::shared_ptr<BaseExpression>(new IfThenElseExpression(condition.getBaseExpression().getManager(), condition.getType().ite(thenExpression.getType(), elseExpression.getType()), condition.getBaseExpressionPointer(), thenExpression.getBaseExpressionPointer(), elseExpression.getBaseExpressionPointer())));
        }
        
        Expression implies(Expression const& first, Expression const& second) {
            assertSameManager(first.getBaseExpression(), second.getBaseExpression());
            return Expression(std::shared_ptr<BaseExpression>(new BinaryBooleanFunctionExpression(first.getBaseExpression().getManager(), first.getType().logicalConnective(second.getType()), first.getBaseExpressionPointer(), second.getBaseExpressionPointer(), BinaryBooleanFunctionExpression::OperatorType::Implies)));
        }
        
        Expression iff(Expression const& first, Expression const& second) {
            assertSameManager(first.getBaseExpression(), second.getBaseExpression());
            return Expression(std::shared_ptr<BaseExpression>(new BinaryBooleanFunctionExpression(first.getBaseExpression().getManager(), first.getType().logicalConnective(second.getType()), first.getBaseExpressionPointer(), second.getBaseExpressionPointer(), BinaryBooleanFunctionExpression::OperatorType::Iff)));
        }
        
        Expression floor(Expression const& first) {
            STORM_LOG_THROW(first.hasNumericalType(), storm::exceptions::InvalidTypeException, "Operator 'floor' requires numerical operand.");
            return Expression(std::shared_ptr<BaseExpression>(new UnaryNumericalFunctionExpression(first.getBaseExpression().getManager(), first.getType().floorCeil(), first.getBaseExpressionPointer(), UnaryNumericalFunctionExpression::OperatorType::Floor)));
        }
        
        Expression ceil(Expression const& first) {
            STORM_LOG_THROW(first.hasNumericalType(), storm::exceptions::InvalidTypeException, "Operator 'ceil' requires numerical operand.");
            return Expression(std::shared_ptr<BaseExpression>(new UnaryNumericalFunctionExpression(first.getBaseExpression().getManager(), first.getType().floorCeil(), first.getBaseExpressionPointer(), UnaryNumericalFunctionExpression::OperatorType::Ceil)));
        }
    }
}
