#pragma once

#include "storm/storage/expressions/ExpressionVisitor.h"
#include "storm/storage/jani/expressions/JaniExpressions.h"

namespace storm {
namespace expressions {
class JaniExpressionVisitor {
   public:
    virtual boost::any visit(ValueArrayExpression const& expression, boost::any const& data) = 0;
    virtual boost::any visit(ConstructorArrayExpression const& expression, boost::any const& data) = 0;
    virtual boost::any visit(ArrayAccessExpression const& expression, boost::any const& data) = 0;
    virtual boost::any visit(FunctionCallExpression const& expression, boost::any const& data) = 0;
};
}  // namespace expressions
}  // namespace storm
