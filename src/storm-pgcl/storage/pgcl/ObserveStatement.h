#pragma once

#include "storm-pgcl/storage/pgcl/BooleanExpression.h"
#include "storm-pgcl/storage/pgcl/Statement.h"

namespace storm {
namespace pgcl {
/**
 * This class represents an observe statement. Observe statements
 * include a condition. If this condition doesn't hold, the program
 * stops at that point in its execution.
 */
class ObserveStatement : public Statement {
   public:
    ObserveStatement() = default;
    /**
     * Constructs an observe statement initialized with the given
     * condition.
     * @param condition The condition of the observe statement.
     */
    ObserveStatement(storm::pgcl::BooleanExpression const& condition);
    ObserveStatement(const ObserveStatement& orig) = default;
    virtual ~ObserveStatement() = default;
    /**
     * Returns the condition of the observe statement.
     * @return The boolean expression of the observe statement.
     */
    storm::pgcl::BooleanExpression const& getCondition() const;
    void accept(class AbstractStatementVisitor&);

   private:
    /// Represents the assigned condition.
    storm::pgcl::BooleanExpression condition;
};
}  // namespace pgcl
}  // namespace storm
