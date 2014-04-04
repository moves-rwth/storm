#ifndef STORM_STORAGE_EXPRESSIONS_BOOLEANLITERALEXPRESSION_H_
#define STORM_STORAGE_EXPRESSIONS_BOOLEANLITERALEXPRESSION_H_

#include "src/storage/expressions/BaseExpression.h"

namespace storm {
    namespace expressions {
        class BooleanLiteralExpression : BaseExpression {
        public:
            /*!
             * Creates a boolean literal expression with the given value.
             *
             * @param value The value of the boolean literal.
             */
            BooleanLiteralExpression(bool value);
            
            // Instantiate constructors and assignments with their default implementations.
            BooleanLiteralExpression(BooleanLiteralExpression const& other) = default;
            BooleanLiteralExpression& operator=(BooleanLiteralExpression const& other) = default;
            BooleanLiteralExpression(BooleanLiteralExpression&&) = default;
            BooleanLiteralExpression& operator=(BooleanLiteralExpression&&) = default;
            virtual ~BooleanLiteralExpression() = default;

            // Override base class methods.
            virtual bool evaluateAsBool(Valuation const& valuation) const override;
            virtual bool isConstant() const override;
            virtual bool isTrue() const override;
            virtual bool isFalse() const override;
            virtual std::set<std::string> getVariables() const override;
            virtual std::set<std::string> getConstants() const override;
            virtual std::unique_ptr<BaseExpression> simplify() const override;
            virtual void accept(ExpressionVisitor* visitor) const override;
            virtual std::unique_ptr<BaseExpression> clone() const override;
            
            /*!
             * Retrieves the value of the boolean literal.
             *
             * @return The value of the boolean literal.
             */
            bool getValue() const;
            
        private:
            // The value of the boolean literal.
            bool value;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_BOOLEANLITERALEXPRESSION_H_ */