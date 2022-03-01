#pragma once

#include "storm-pgcl/storage/pgcl/PgclProgram.h"
#include "storm-pgcl/storage/pgcl/Statement.h"

namespace storm {
namespace pgcl {
/**
 * This abstract class handles the branching statements. Every branch
 * statement has a right and a left branch. Since branch statements are
 * compound statements, every branch is again a complete PGCL program
 * itself.
 */
class BranchStatement : public Statement {
   public:
    BranchStatement() = default;
    BranchStatement(const BranchStatement& orig) = default;
    virtual ~BranchStatement() = default;
    virtual void accept(class AbstractStatementVisitor&) = 0;
    /**
     * Returns the left branch of the statement.
     * @return The left branch PGCL program.
     */
    std::shared_ptr<storm::pgcl::PgclBlock> const& getLeftBranch() const;
    /**
     * Returns the right branch of the statement.
     * @return The right branch PGCL program.
     */
    std::shared_ptr<storm::pgcl::PgclBlock> const& getRightBranch() const;

   protected:
    std::shared_ptr<storm::pgcl::PgclBlock> leftBranch;
    std::shared_ptr<storm::pgcl::PgclBlock> rightBranch;
};
}  // namespace pgcl
}  // namespace storm
