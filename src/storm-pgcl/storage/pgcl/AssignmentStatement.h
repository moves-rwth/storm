#pragma once

#include <boost/variant/get.hpp>
#include <boost/variant/variant.hpp>
#include "storm-pgcl/storage/pgcl/Statement.h"
#include "storm-pgcl/storage/pgcl/UniformExpression.h"
#include "storm/storage/expressions/Expression.h"
#include "storm/storage/expressions/Variable.h"

namespace storm {
namespace pgcl {
/**
 * This class represents a simple assignment statement of the form
 * identifier := expression; where the expression is either handled by
 * the expression manager or is a uniform distribution expression.
 */
class AssignmentStatement : public Statement {
   public:
    AssignmentStatement() = default;
    /**
     * Constructs an assignment statement with the variable as the left
     * side and the expression as the right side of the assignment.
     * @param variable The left hand variable of the assignment.
     * @param expression The right hand expression of the assignment.
     */
    AssignmentStatement(storm::expressions::Variable const& variable,
                        boost::variant<storm::expressions::Expression, storm::pgcl::UniformExpression> const& expression);
    AssignmentStatement(const AssignmentStatement& orig) = default;
    virtual ~AssignmentStatement() = default;
    void accept(AbstractStatementVisitor&);
    /**
     * Returns the right hand expression of the assignemnt.
     * @return The expression of the assignment.
     */
    boost::variant<storm::expressions::Expression, storm::pgcl::UniformExpression> const& getExpression() const;

    bool isDeterministic() const;
    /**
     * Returns the left hand variable of the assignemnt.
     * @return The variable to which the expression is assigned.
     */
    storm::expressions::Variable const& getVariable() const;

   private:
    /// Represents the variable of our assignment statement.
    storm::expressions::Variable variable;
    /// Represents the right hand side of our assignment statement.
    boost::variant<storm::expressions::Expression, storm::pgcl::UniformExpression> expression;
};
}  // namespace pgcl
}  // namespace storm
