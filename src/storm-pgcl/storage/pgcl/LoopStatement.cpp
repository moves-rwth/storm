
#include "LoopStatement.h"
#include "storm-pgcl/storage/pgcl/AbstractStatementVisitor.h"

namespace storm {
namespace pgcl {
LoopStatement::LoopStatement(storm::pgcl::BooleanExpression const& condition, std::shared_ptr<storm::pgcl::PgclBlock> const& body)
    : body(body), condition(condition) {}

std::shared_ptr<storm::pgcl::PgclBlock> const& LoopStatement::getBody() const {
    return this->body;
}

storm::pgcl::BooleanExpression const& LoopStatement::getCondition() const {
    return this->condition;
}

void LoopStatement::accept(storm::pgcl::AbstractStatementVisitor& visitor) {
    visitor.visit(*this);
}
}  // namespace pgcl
}  // namespace storm
