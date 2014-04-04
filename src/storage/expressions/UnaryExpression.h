#ifndef STORM_STORAGE_EXPRESSIONS_UNARYEXPRESSION_H_
#define STORM_STORAGE_EXPRESSIONS_UNARYEXPRESSION_H_

#include "src/storage/expressions/BaseExpression.h"

namespace storm {
    namespace expressions {
        class UnaryExpression : public BaseExpression {
        public:
            /*!
             * Creates a unary expression with the given return type and operand.
             *
             * @param returnType The return type of the expression.
             * @param operand The operand of the unary expression.
             */
            UnaryExpression(ExpressionReturnType returnType, std::unique_ptr<BaseExpression>&& operand);

            // Provide custom versions of copy construction and assignment.
            UnaryExpression(UnaryExpression const& other);
            UnaryExpression& operator=(UnaryExpression const& other);
            
            // Create default variants of move construction/assignment and virtual destructor.
            UnaryExpression(UnaryExpression&&) = default;
            UnaryExpression& operator=(UnaryExpression&&) = default;
            virtual ~UnaryExpression() = default;
            
            // Override base class methods.
            virtual bool isConstant() const override;
            virtual std::set<std::string> getVariables() const override;
            virtual std::set<std::string> getConstants() const override;
            
            /*!
             * Retrieves the operand of the unary expression.
             *
             * @return The operand of the unary expression.
             */
            std::unique_ptr<BaseExpression> const& getOperand() const;
            
        private:
            // The operand of the unary expression.
            std::unique_ptr<BaseExpression> operand;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_UNARYEXPRESSION_H_ */