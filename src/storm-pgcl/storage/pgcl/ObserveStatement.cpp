#include "ObserveStatement.h"
#include "storm-pgcl/storage/pgcl/AbstractStatementVisitor.h"

namespace storm {
namespace pgcl {
ObserveStatement::ObserveStatement(storm::pgcl::BooleanExpression const& condition) : condition(condition) {}

void ObserveStatement::accept(storm::pgcl::AbstractStatementVisitor& visitor) {
    visitor.visit(*this);
}

storm::pgcl::BooleanExpression const& ObserveStatement::getCondition() const {
    return this->condition;
}

}  // namespace pgcl
}  // namespace storm
