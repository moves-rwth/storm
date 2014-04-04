#ifndef STORM_STORAGE_EXPRESSIONS_BOOLEANCONSTANTEXPRESSION_H_
#define STORM_STORAGE_EXPRESSIONS_BOOLEANCONSTANTEXPRESSION_H_

#include "src/storage/expressions/ConstantExpression.h"

namespace storm {
    namespace expressions {
        class BooleanConstantExpression : public ConstantExpression {
        public:
            /*!
             * Creates a boolean constant expression with the given constant name.
             *
             * @param constantName The name of the boolean constant associated with this expression.
             */
            BooleanConstantExpression(std::string const& constantName);
            
            // Instantiate constructors and assignments with their default implementations.
            BooleanConstantExpression(BooleanConstantExpression const& other) = default;
            BooleanConstantExpression& operator=(BooleanConstantExpression const& other) = default;
            BooleanConstantExpression(BooleanConstantExpression&&) = default;
            BooleanConstantExpression& operator=(BooleanConstantExpression&&) = default;
            virtual ~BooleanConstantExpression() = default;
            
            // Override base class methods.
            virtual bool evaluateAsBool(Valuation const& valuation) const;
            virtual std::unique_ptr<BaseExpression> clone() const;
            virtual void accept(ExpressionVisitor* visitor) const;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_BOOLEANCONSTANTEXPRESSION_H_ */
