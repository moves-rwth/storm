#ifndef STORM_STORAGE_EXPRESSIONS_UNARYBOOLEANFUNCTIONEXPRESSION_H_
#define STORM_STORAGE_EXPRESSIONS_UNARYBOOLEANFUNCTIONEXPRESSION_H_

#include "src/storage/expressions/UnaryExpression.h"

namespace storm {
    namespace expressions {
        class UnaryBooleanFunctionExpression : public UnaryExpression {
        public:
            /*!
             * An enum type specifying the different functions applicable.
             */
            enum class OperatorType { Not };
            
            /*!
             * Creates a unary boolean function expression with the given return type, operand and operator.
             *
             * @param returnType The return type of the expression.
             * @param operand The operand of the expression.
             * @param operatorType The operator of the expression.
             */
            UnaryBooleanFunctionExpression(ExpressionReturnType returnType, std::shared_ptr<BaseExpression const> const& operand, OperatorType operatorType);

            // Instantiate constructors and assignments with their default implementations.
            UnaryBooleanFunctionExpression(UnaryBooleanFunctionExpression const& other) = default;
            UnaryBooleanFunctionExpression& operator=(UnaryBooleanFunctionExpression const& other) = default;
            UnaryBooleanFunctionExpression(UnaryBooleanFunctionExpression&&) = default;
            UnaryBooleanFunctionExpression& operator=(UnaryBooleanFunctionExpression&&) = default;
            virtual ~UnaryBooleanFunctionExpression() = default;
            
            // Override base class methods.
            virtual bool evaluateAsBool(Valuation const& valuation) const override;
            virtual std::shared_ptr<BaseExpression const> simplify() const override;
            virtual void accept(ExpressionVisitor* visitor) const override;

            /*!
             * Retrieves the operator associated with this expression.
             *
             * @return The operator associated with this expression.
             */
            OperatorType getOperatorType() const;
            
        protected:
            // Override base class method.
            virtual void printToStream(std::ostream& stream) const override;

        private:
            // The operator of this expression.
            OperatorType operatorType;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_UNARYBOOLEANFUNCTIONEXPRESSION_H_ */
