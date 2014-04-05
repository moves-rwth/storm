#ifndef STORM_STORAGE_EXPRESSIONS_INTEGERCONSTANTEXPRESSION_H_
#define STORM_STORAGE_EXPRESSIONS_INTEGERCONSTANTEXPRESSION_H_

#include "src/storage/expressions/ConstantExpression.h"

namespace storm {
    namespace expressions {
        class IntegerConstantExpression : public ConstantExpression {
        public:
            /*!
             * Creates an integer constant expression with the given constant name.
             *
             * @param constantName The name of the integer constant associated with this expression.
             */
            IntegerConstantExpression(std::string const& constantName);
            
            // Instantiate constructors and assignments with their default implementations.
            IntegerConstantExpression(IntegerConstantExpression const& other) = default;
            IntegerConstantExpression& operator=(IntegerConstantExpression const& other) = default;
            IntegerConstantExpression(IntegerConstantExpression&&) = default;
            IntegerConstantExpression& operator=(IntegerConstantExpression&&) = default;
            virtual ~IntegerConstantExpression() = default;
            
            // Override base class methods.
            virtual int_fast64_t evaluateAsInt(Valuation const& valuation) const override;
            virtual double evaluateAsDouble(Valuation const& valuation) const override;
            virtual std::shared_ptr<BaseExpression const> simplify() const override;
            virtual void accept(ExpressionVisitor* visitor) const;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_INTEGERCONSTANTEXPRESSION_H_ */
