#ifndef STORM_STORAGE_EXPRESSIONS_BINARYBOOLEANFUNCTIONEXPRESSION_H_
#define STORM_STORAGE_EXPRESSIONS_BINARYBOOLEANFUNCTIONEXPRESSION_H_

#include "src/storage/expressions/BinaryExpression.h"

namespace storm {
    namespace expressions {
        class BinaryBooleanFunctionExpression : public BinaryExpression {
        public:
            /*!
             * An enum type specifying the different operators applicable.
             */
            enum class OperatorType {And, Or};
            
            /*!
             * Creates a binary boolean function expression with the given return type, operands and operator.
             *
             * @param returnType The return type of the expression.
             * @param firstOperand The first operand of the expression.
             * @param secondOperand The second operand of the expression.
             * @param functionType The operator of the expression.
             */
            BinaryBooleanFunctionExpression(ExpressionReturnType returnType, std::unique_ptr<BaseExpression>&& fistOperand, std::unique_ptr<BaseExpression>&& secondOperand, OperatorType operatorType);
            
            // Instantiate constructors and assignments with their default implementations.
            BinaryBooleanFunctionExpression(BinaryBooleanFunctionExpression const& other) = default;
            BinaryBooleanFunctionExpression& operator=(BinaryBooleanFunctionExpression const& other) = default;
            BinaryBooleanFunctionExpression(BinaryBooleanFunctionExpression&&) = default;
            BinaryBooleanFunctionExpression& operator=(BinaryBooleanFunctionExpression&&) = default;
            virtual ~BinaryBooleanFunctionExpression() = default;
            
            // Override base class methods.
            virtual bool evaluateAsBool(Valuation const& valuation) const override;
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

#endif /* STORM_STORAGE_EXPRESSIONS_BINARYBOOLEANFUNCTIONEXPRESSION_H_ */