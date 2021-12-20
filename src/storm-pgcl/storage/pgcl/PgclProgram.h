#pragma once

#include <vector>
#include "Block.h"
#include "VariableDeclaration.h"
#include "storm-pgcl/storage/pgcl/Statement.h"
#include "storm-pgcl/storage/pgcl/StatementPrinterVisitor.h"
#include "storm/storage/expressions/ExpressionManager.h"

namespace storm {
namespace pgcl {

/**
 * This class represents a complete and functional PGCL program. It
 * contains an expression manager which keeps track of the current
 * identifiers and variable valuations. Other than that, it basically
 * wraps a std::vector of program statements and is intended to be used
 * as such.
 */
class PgclProgram : public PgclBlock {
   public:
    PgclProgram() = default;
    /**
     * Constructs the PGCL program with the given sequence of statements
     * (they may contain other PGCL programs). The expression manager
     * handles the expressions and variables of the PGCL program.
     * @param statements The sequence of statements representing the
     * program.
     * @param locationToStatements A vector containing the statement
     * with location number i at the i-th position.
     * @param parameters The list of parameters of the program.
     * @param expressions The manager responsible for the expressions
     * and variables of the program.
     * @param hasLoop Whether the program contains a loop
     * @param hasNondet Whether the program contains a nondeterministic
     * statement.
     * @param hasParam Whether the program is parameterized.
     */
    PgclProgram(std::vector<VariableDeclaration> variables, vector const& statements, vector const& locationToStatement,
                std::vector<storm::expressions::Variable> const& parameters, std::shared_ptr<storm::expressions::ExpressionManager> expressions, bool hasLoop,
                bool hasNondet, bool hasObserve);

    PgclProgram(const PgclProgram& orig) = default;

    /**
     * Returns a vector that has the statement with location number i at
     * its i-th position. This allows for O(1)-access of statements if
     * only the location number is given.
     */
    vector getLocationToStatementVector();

    std::vector<storm::expressions::Variable> getVariables() const;
    std::vector<storm::pgcl::VariableDeclaration> const& getVariableDeclarations() const;

   private:
    /**
     * Contains the statement with location i at its i-th position.
     * Imagine this as the "unrolled" sequence of statements, so the
     * recursion is resolved here.
     */
    vector locationToStatement;
    std::vector<storm::pgcl::VariableDeclaration> variables;
};
/**
 * Prints every statement of the program along with their location
 * numbers.
 * @param stream The stream to print the program to.
 * @param program The program to print.
 */
std::ostream& operator<<(std::ostream& stream, PgclProgram& program);
}  // namespace pgcl
}  // namespace storm
