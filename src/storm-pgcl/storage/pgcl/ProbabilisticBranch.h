#pragma once

#include "storm-pgcl/storage/pgcl/BranchStatement.h"

namespace storm {
namespace pgcl {
/**
 * This class represents a probabilistic branch. It branches into two
 * subprograms. The first program is executed with the probability p
 * given by the assigned expression, the second subprogram is executed
 * with probability (p - 1).
 */
class ProbabilisticBranch : public BranchStatement {
   public:
    ProbabilisticBranch() = default;
    /**
     * Constructs a probabilistic branch initialized with the given
     * probability and the left (first) and right (second) subprograms.
     * Note that no verification is made whether the probability lies
     * between 0 and 1 is made here. This is done at runtime of the PGCL
     * program.
     * @param probability The expression representing the probability of the branch.
     * @param left The left (first) subprogram of the branch.
     * @param right The right (second) subprogram of the branch.
     */
    ProbabilisticBranch(storm::expressions::Expression const& probability, std::shared_ptr<storm::pgcl::PgclBlock> const& left,
                        std::shared_ptr<storm::pgcl::PgclBlock> const& right);
    ProbabilisticBranch(const ProbabilisticBranch& orig) = default;
    virtual ~ProbabilisticBranch() = default;
    /**
     * Returns the expression representing the probability.
     * @return The expression representing the probability.
     */
    storm::expressions::Expression const& getProbability() const;
    void accept(class AbstractStatementVisitor&);

   private:
    /// The expression represents the probability of the branch.
    storm::expressions::Expression probability;
};
}  // namespace pgcl
}  // namespace storm
