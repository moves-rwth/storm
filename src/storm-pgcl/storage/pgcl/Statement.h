#pragma once

#include <cstdint>
#include <memory>
#include "boost/optional/optional.hpp"

namespace storm {
namespace pgcl {
class PgclBlock;
/**
 * A PGCL program consists of various statements. Statements can again
 * save lists of statements as their children. To make life easier, the
 * line number where that statement was parsed is stored. Additionaly,
 * a unique non-negative index identifier (the location number) is
 * saved.
 */
class Statement {
   public:
    Statement() = default;
    Statement(const Statement& orig) = default;
    virtual ~Statement() = default;
    /**
     * Returns true iff the statements are equal, thus having the same
     * location number.
     * @param other The other statement to check equality of.
     * @return True iff the statements are equal.
     */
    bool operator==(const Statement& other) const;
    /**
     * Returns the line number inside the string where the PGCL program
     * was parsed from.
     * @return The line number of the statement.
     */
    std::size_t getLineNumber() const;
    /**
     * Sets the line number during the parsing process.
     * @param lineNumber The location number of the statement.
     */
    void setLineNumber(std::size_t lineNumber);
    /**
     * Returns the unique location number of the statement.
     * @return The line number of the statement.
     */
    std::size_t getLocationNumber() const;
    /**
     * Sets the unique location number of the statement.
     * @param lineNumber The location number of the statement.
     */
    void setLocationNumber(std::size_t locationNumber);
    /**
     * Returns true if the statement is the last of its direct parent
     * program.
     * @return true Whether the statement is the last statement.
     */
    bool isLast() const;
    /**
     * Sets the information whether the statement is the last of its
     * direct parent program.
     * @param isLast Whether the statement is the last statement.
     */
    void setLast(bool isLast);
    /**
     * Returns wether the statements represents nondeterminism.
     */
    virtual bool isNondet() const;
    /**
     * Returns the number of transitions this statement will produce.
     */
    virtual std::size_t getNumberOfOutgoingTransitions() const;
    virtual void accept(class AbstractStatementVisitor&) = 0;
    /**
     * Sets the parent program of the statement.
     * @param parentProgram The parent program of the statement.
     */
    void setParentBlock(PgclBlock* block);
    /**
     * Returns the parent program of the statement.
     * @return The parent program of the statement.
     */
    PgclBlock* getParentBlock();

   protected:
    /// The parent program of the statement.
    PgclBlock* parentBlock;
    /// Represents the line number of the statement.
    std::size_t lineNumber = 0;
    /// Represents the unique statement location.
    std::size_t locationNumber = 0;
    /// If set to true, the statement is the last one of its (sub)program.
    bool last = false;
};
}  // namespace pgcl
}  // namespace storm
