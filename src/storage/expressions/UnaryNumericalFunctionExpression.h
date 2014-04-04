#ifndef STORM_STORAGE_EXPRESSIONS_UNARYNUMERICALFUNCTIONEXPRESSION_H_
#define STORM_STORAGE_EXPRESSIONS_UNARYNUMERICALFUNCTIONEXPRESSION_H_

#include "src/storage/expressions/UnaryExpression.h"

namespace storm {
    namespace expressions {
        class UnaryNumericalFunctionExpression : public UnaryExpression {
            /*!
             * An enum type specifying the different functions applicable.
             */
            enum class OperatorType {Minus, Floor, Ceil};
            
            /*!
             * Creates a unary numerical function expression with the given return type, operand and operator.
             *
             * @param returnType The return type of the expression.
             * @param operand The operand of the expression.
             * @param operatorType The operator of the expression.
             */
            UnaryNumericalFunctionExpression(ExpressionReturnType returnType, std::unique_ptr<BaseExpression>&& operand, OperatorType operatorType);
            
            // Instantiate constructors and assignments with their default implementations.
            UnaryNumericalFunctionExpression(UnaryNumericalFunctionExpression const& other) = default;
            UnaryNumericalFunctionExpression& operator=(UnaryNumericalFunctionExpression const& other) = default;
            UnaryNumericalFunctionExpression(UnaryNumericalFunctionExpression&&) = default;
            UnaryNumericalFunctionExpression& operator=(UnaryNumericalFunctionExpression&&) = default;
            virtual ~UnaryNumericalFunctionExpression() = default;
            
            // Override base class methods.
            virtual int_fast64_t evaluateAsInt(Valuation const& valuation) const override;
            virtual double evaluateAsDouble(Valuation const& valuation) const override;
            virtual std::unique_ptr<BaseExpression> simplify() const override;
            virtual void accept(ExpressionVisitor* visitor) const override;
            virtual std::unique_ptr<BaseExpression> clone() const override;
            
            /*!
             * Retrieves the operator associated with this expression.
             *
             * @return The operator associated with this expression.
             */
            OperatorType getOperatorType() const;
            
        private:
            // The operator of this expression.
            OperatorType operatorType;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_UNARYNUMERICALFUNCTIONEXPRESSION_H_ */
