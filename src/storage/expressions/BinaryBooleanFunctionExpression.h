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
            enum OperatorType {AND, OR};
            
            /*!
             * Creates a binary boolean function expression with the given return type, operands and operator.
             *
             * @param returnType The return type of the expression.
             * @param firstOperand The first operand of the expression.
             * @param secondOperand The second operand of the expression.
             * @param functionType The operator of the expression.
             */
            BinaryBooleanFunctionExpression(ExpressionReturnType returnType, std::unique_ptr<BaseExpression>&& fistOperand, std::unique_ptr<BaseExpression>&& secondOperand, OperatorType operatorType);
            
            // Provide custom versions of copy construction and assignment.
            BinaryBooleanFunctionExpression(BinaryBooleanFunctionExpression const& other);
            BinaryBooleanFunctionExpression& operator=(BinaryBooleanFunctionExpression const& other);
            
            // Create default variants of move construction/assignment and virtual destructor.
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