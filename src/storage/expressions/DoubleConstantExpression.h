#ifndef STORM_STORAGE_EXPRESSIONS_DOUBLECONSTANTEXPRESSION_H_
#define STORM_STORAGE_EXPRESSIONS_DOUBLECONSTANTEXPRESSION_H_

#include "src/storage/expressions/ConstantExpression.h"

namespace storm {
    namespace expressions {
        class DoubleConstantExpression : public ConstantExpression {
        public:
            /*!
             * Creates a double constant expression with the given constant name.
             *
             * @param constantName The name of the double constant associated with this expression.
             */
            DoubleConstantExpression(std::string const& constantName);
            
            // Instantiate constructors and assignments with their default implementations.
            DoubleConstantExpression(DoubleConstantExpression const& other) = default;
            DoubleConstantExpression& operator=(DoubleConstantExpression const& other) = default;
            DoubleConstantExpression(DoubleConstantExpression&&) = default;
            DoubleConstantExpression& operator=(DoubleConstantExpression&&) = default;
            virtual ~DoubleConstantExpression() = default;
            
            // Override base class methods.
            virtual double evaluateAsDouble(Valuation const* valuation = nullptr) const override;
            virtual void accept(ExpressionVisitor* visitor) const override;
            virtual std::shared_ptr<BaseExpression const> simplify() const override;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_DOUBLECONSTANTEXPRESSION_H_ */
