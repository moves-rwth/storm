#include "AssignmentStatement.h"
#include "storm-pgcl/storage/pgcl/AbstractStatementVisitor.h"

namespace storm {
namespace pgcl {
AssignmentStatement::AssignmentStatement(storm::expressions::Variable const& variable,
                                         boost::variant<storm::expressions::Expression, storm::pgcl::UniformExpression> const& expression)
    : variable(variable), expression(expression) {}

bool AssignmentStatement::isDeterministic() const {
    return expression.which() == 0;
}

boost::variant<storm::expressions::Expression, storm::pgcl::UniformExpression> const& AssignmentStatement::getExpression() const {
    return this->expression;
}

storm::expressions::Variable const& AssignmentStatement::getVariable() const {
    return this->variable;
}

void AssignmentStatement::accept(storm::pgcl::AbstractStatementVisitor& visitor) {
    visitor.visit(*this);
}
}  // namespace pgcl
}  // namespace storm
