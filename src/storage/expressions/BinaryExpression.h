#ifndef STORM_STORAGE_EXPRESSIONS_BINARYEXPRESSION_H_
#define STORM_STORAGE_EXPRESSIONS_BINARYEXPRESSION_H_

#include "src/storage/expressions/BaseExpression.h"

namespace storm {
    namespace expressions {
        /*!
         * The base class of all binary expressions.
         */
        class BinaryExpression : public BaseExpression {
        public:
            /*!
             * Constructs a binary expression with the given return type and operands.
             *
             * @param returnType The return type of the expression.
             * @param firstOperand The first operand of the expression.
             * @param secondOperand The second operand of the expression.
             */
            BinaryExpression(ExpressionReturnType returnType, std::unique_ptr<BaseExpression>&& firstOperand, std::unique_ptr<BaseExpression>&& secondOperand);
            
            // Provide custom versions of copy construction and assignment.
            BinaryExpression(BinaryExpression const& other);
            BinaryExpression& operator=(BinaryExpression const& other);

            // Create default variants of move construction/assignment and virtual destructor.
            BinaryExpression(BinaryExpression&&) = default;
            BinaryExpression& operator=(BinaryExpression&&) = default;
            virtual ~BinaryExpression() = default;

            // Override base class methods.
            virtual bool isConstant() const override;
            virtual std::set<std::string> getVariables() const override;
            virtual std::set<std::string> getConstants() const override;
            
            /*!
             * Retrieves the first operand of the expression.
             *
             * @return The first operand of the expression.
             */
            std::unique_ptr<BaseExpression> const& getFirstOperand() const;
            
            /*!
             * Retrieves the second operand of the expression.
             *
             * @return The second operand of the expression.
             */
            std::unique_ptr<BaseExpression> const& getSecondOperand() const;

        private:
            // The first operand of the expression.
            std::unique_ptr<BaseExpression> firstOperand;
            
            // The second operand of the expression.
            std::unique_ptr<BaseExpression> secondOperand;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_BINARYEXPRESSION_H_ */