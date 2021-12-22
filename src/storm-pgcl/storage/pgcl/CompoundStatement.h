#pragma once

#include "storm-pgcl/storage/pgcl/Statement.h"

namespace storm {
namespace pgcl {
/**
 * This abstract class represents compound statements. A compound
 * statement is a statement which has again a PGCL program as a child.
 * Examples are if and loop statements.
 */
class CompoundStatement : public Statement {
   public:
    CompoundStatement() = default;
    CompoundStatement(const CompoundStatement& orig) = default;
    virtual ~CompoundStatement() = default;
    virtual void accept(class AbstractStatementVisitor&) = 0;

   private:
};
}  // namespace pgcl
}  // namespace storm
