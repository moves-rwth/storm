#pragma once

#include "storm/storage/expressions/Expression.h"

namespace storm {
namespace pgcl {
/**
 * This class wraps an ordinary expression but allows only for boolean
 * expressions to be handled, e.g. expressions of the form (x < 4) or
 * (x != y), but not (x + 5).
 */
class BooleanExpression {
   public:
    BooleanExpression() = default;
    /**
     * Constructs a boolean expression if the given expression is of a
     * boolean type. Note that it is not checked whether the expression
     * has a boolean type.
     * @param booleanExpression The expression of a boolean type.
     */
    BooleanExpression(storm::expressions::Expression const& booleanExpression);
    /**
     * Returns the expression.
     * @return The expression of boolean type.
     */
    storm::expressions::Expression const& getBooleanExpression() const;

   private:
    storm::expressions::Expression booleanExpression;
};
}  // namespace pgcl
}  // namespace storm
