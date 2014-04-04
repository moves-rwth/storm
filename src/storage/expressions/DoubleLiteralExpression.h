#ifndef STORM_STORAGE_EXPRESSIONS_DOUBLELITERALEXPRESSION_H_
#define STORM_STORAGE_EXPRESSIONS_DOUBLELITERALEXPRESSION_H_

#include "src/storage/expressions/BaseExpression.h"

namespace storm {
    namespace expressions {
        class DoubleLiteralExpression : BaseExpression {
        public:
            /*!
             * Creates an double literal expression with the given value.
             *
             * @param value The value of the double literal.
             */
            DoubleLiteralExpression(double value);
            
            // Instantiate constructors and assignments with their default implementations.
            DoubleLiteralExpression(DoubleLiteralExpression const& other) = default;
            DoubleLiteralExpression& operator=(DoubleLiteralExpression const& other) = default;
            DoubleLiteralExpression(DoubleLiteralExpression&&) = default;
            DoubleLiteralExpression& operator=(DoubleLiteralExpression&&) = default;
            virtual ~DoubleLiteralExpression() = default;
            
            // Override base class methods.
            virtual double evaluateAsDouble(Valuation const& valuation) const override;
            virtual bool isConstant() const override;
            virtual std::set<std::string> getVariables() const override;
            virtual std::set<std::string> getConstants() const override;
            virtual std::unique_ptr<BaseExpression> simplify() const override;
            virtual void accept(ExpressionVisitor* visitor) const override;
            virtual std::unique_ptr<BaseExpression> clone() const override;
            
            /*!
             * Retrieves the value of the double literal.
             *
             * @return The value of the double literal.
             */
            double getValue() const;
            
        private:
            // The value of the double literal.
            double value;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_DOUBLELITERALEXPRESSION_H_ */