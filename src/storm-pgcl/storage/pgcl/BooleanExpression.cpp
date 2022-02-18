#include "BooleanExpression.h"
#include "storm/storage/expressions/ExpressionManager.h"

namespace storm {
namespace pgcl {
BooleanExpression::BooleanExpression(storm::expressions::Expression const& booleanExpression) : booleanExpression(booleanExpression) {}

storm::expressions::Expression const& BooleanExpression::getBooleanExpression() const {
    return this->booleanExpression;
}
}  // namespace pgcl
}  // namespace storm
