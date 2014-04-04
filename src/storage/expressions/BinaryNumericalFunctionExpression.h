#ifndef STORM_STORAGE_EXPRESSIONS_BINARYNUMERICALFUNCTIONEXPRESSION_H_
#define STORM_STORAGE_EXPRESSIONS_BINARYNUMERICALFUNCTIONEXPRESSION_H_

#include "src/storage/expressions/BinaryExpression.h"

namespace storm {
    namespace expressions {
        class BinaryNumericalFunctionExpression : public BinaryExpression {
        public:
            /*!
             * An enum type specifying the different operators applicable.
             */
            enum class OperatorType {Plus, Minus, Times, Divide, Min, Max};
            
            /*!
             * Constructs a binary numerical function expression with the given return type, operands and operator.
             *
             * @param returnType The return type of the expression.
             * @param firstOperand The first operand of the expression.
             * @param secondOperand The second operand of the expression.
             * @param functionType The operator of the expression.
             */
            BinaryNumericalFunctionExpression(ExpressionReturnType returnType, std::unique_ptr<BaseExpression>&& firstOperand, std::unique_ptr<BaseExpression>&& secondOperand, OperatorType operatorType);
            
            // Instantiate constructors and assignments with their default implementations.
            BinaryNumericalFunctionExpression(BinaryNumericalFunctionExpression const& other) = default;
            BinaryNumericalFunctionExpression& operator=(BinaryNumericalFunctionExpression const& other) = default;
            BinaryNumericalFunctionExpression(BinaryNumericalFunctionExpression&&) = default;
            BinaryNumericalFunctionExpression& operator=(BinaryNumericalFunctionExpression&&) = default;
            virtual ~BinaryNumericalFunctionExpression() = default;
            
            // Override base class methods.
            virtual int_fast64_t evaluateAsInt(Valuation const& valuation) const override;
            virtual double evaluateAsDouble(Valuation const& valuation) const override;
            virtual std::unique_ptr<BaseExpression> simplify() const override;
            virtual void accept(ExpressionVisitor* visitor) const override;
            virtual std::unique_ptr<BaseExpression> clone() const override;
            
            /*!
             * Retrieves the operator associated with the expression.
             *
             * @return The operator associated with the expression.
             */
            OperatorType getOperatorType() const;
            
        private:
            // The operator of the expression.
            OperatorType operatorType;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_BINARYNUMERICALFUNCTIONEXPRESSION_H_ */