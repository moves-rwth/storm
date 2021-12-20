#include "IfStatement.h"
#include "storm-pgcl/storage/pgcl/AbstractStatementVisitor.h"

namespace storm {
namespace pgcl {
IfStatement::IfStatement(storm::pgcl::BooleanExpression const& condition, std::shared_ptr<storm::pgcl::PgclBlock> const& body)
    : ifBody(body), condition(condition) {}

IfStatement::IfStatement(storm::pgcl::BooleanExpression const& condition, std::shared_ptr<storm::pgcl::PgclBlock> const& ifBody,
                         std::shared_ptr<storm::pgcl::PgclBlock> const& elseBody)
    : ifBody(ifBody), elseBody(elseBody), condition(condition) {
    this->hasElseBody = true;
}

std::shared_ptr<storm::pgcl::PgclBlock> const& IfStatement::getIfBody() const {
    return this->ifBody;
}

std::shared_ptr<storm::pgcl::PgclBlock> const& IfStatement::getElseBody() const {
    assert(hasElse());
    return this->elseBody;
}

bool IfStatement::hasElse() const {
    return this->hasElseBody;
}

storm::pgcl::BooleanExpression const& IfStatement::getCondition() const {
    return this->condition;
}

void IfStatement::accept(storm::pgcl::AbstractStatementVisitor& visitor) {
    visitor.visit(*this);
}

}  // namespace pgcl
}  // namespace storm
