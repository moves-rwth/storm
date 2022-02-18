#include "storm/storage/expressions/FullPredicateSplitter.h"

#include "storm/storage/expressions/Expression.h"
#include "storm/storage/expressions/Expressions.h"

#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/utility/macros.h"

namespace storm {
namespace expressions {

std::vector<storm::expressions::Expression> FullPredicateSplitter::split(storm::expressions::Expression const& expression) {
    STORM_LOG_THROW(expression.hasBooleanType(), storm::exceptions::InvalidArgumentException, "Expected predicate of boolean type.");

    // Gather all atoms.
    atomicExpressions.clear();
    expression.accept(*this, boost::none);

    // Remove all boolean literals from the atoms.
    std::vector<storm::expressions::Expression> atomsToKeep;
    for (auto const& atom : atomicExpressions) {
        if (!atom.isTrue() && !atom.isFalse()) {
            atomsToKeep.push_back(atom);
        }
    }
    atomicExpressions = std::move(atomsToKeep);

    return atomicExpressions;
}

boost::any FullPredicateSplitter::visit(IfThenElseExpression const& expression, boost::any const&) {
    atomicExpressions.push_back(expression.toExpression());
    return boost::any();
}

boost::any FullPredicateSplitter::visit(BinaryBooleanFunctionExpression const& expression, boost::any const& data) {
    expression.getFirstOperand()->accept(*this, data);
    expression.getSecondOperand()->accept(*this, data);
    return boost::any();
}

boost::any FullPredicateSplitter::visit(BinaryNumericalFunctionExpression const&, boost::any const&) {
    return boost::any();
}

boost::any FullPredicateSplitter::visit(BinaryRelationExpression const& expression, boost::any const&) {
    atomicExpressions.push_back(expression.toExpression());
    return boost::any();
}

boost::any FullPredicateSplitter::visit(VariableExpression const& expression, boost::any const&) {
    if (expression.hasBooleanType()) {
        atomicExpressions.push_back(expression.toExpression());
    }
    return boost::any();
}

boost::any FullPredicateSplitter::visit(UnaryBooleanFunctionExpression const& expression, boost::any const& data) {
    expression.getOperand()->accept(*this, data);
    return boost::any();
}

boost::any FullPredicateSplitter::visit(UnaryNumericalFunctionExpression const&, boost::any const&) {
    return boost::any();
}

boost::any FullPredicateSplitter::visit(BooleanLiteralExpression const&, boost::any const&) {
    return boost::any();
}

boost::any FullPredicateSplitter::visit(IntegerLiteralExpression const&, boost::any const&) {
    return boost::any();
}

boost::any FullPredicateSplitter::visit(RationalLiteralExpression const&, boost::any const&) {
    return boost::any();
}

}  // namespace expressions
}  // namespace storm
