#pragma once

#include "storm-pgcl/storage/pgcl/BranchStatement.h"

namespace storm {
namespace pgcl {
/**
 * This class represents a nondeterministic branch that allows for a
 * nondeterministic path-taking between two subprograms.
 */
class NondeterministicBranch : public BranchStatement {
   public:
    NondeterministicBranch() = default;
    /**
     * Constructs a nondeterministic branch initialized with the given
     * left and right subprograms.
     * @param left The left (first) subprogram of the branch.
     * @param right The right (second) subprogram of the branch.
     */
    NondeterministicBranch(std::shared_ptr<storm::pgcl::PgclBlock> const& left, std::shared_ptr<storm::pgcl::PgclBlock> const& right);
    NondeterministicBranch(const NondeterministicBranch& orig) = default;
    virtual ~NondeterministicBranch() = default;
    void accept(class AbstractStatementVisitor&);
    bool isNondet() const;

   private:
};
}  // namespace pgcl
}  // namespace storm
