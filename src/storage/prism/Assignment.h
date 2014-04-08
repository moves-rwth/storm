#ifndef STORM_STORAGE_PRISM_ASSIGNMENT_H_
#define STORM_STORAGE_PRISM_ASSIGNMENT_H_

#include <map>

#include "src/storage/prism/LocatedInformation.h"
#include "src/storage/expressions/Expression.h"

namespace storm {
    namespace prism {
        class Assignment : public LocatedInformation {
        public:
            /*!
             * Constructs an assignment using the given variable name and expression.
             *
             * @param variableName The variable that this assignment targets.
             * @param expression The expression to assign to the variable.
             * @param filename The filename in which the assignment is defined.
             * @param lineNumber The line number in which the assignment is defined.
             */
            Assignment(std::string const& variableName, storm::expressions::Expression const& expression, std::string const& filename = "", uint_fast64_t lineNumber = 0);
            
            /*!
             * Creates a copy of the given assignment and performs the provided renaming.
             *
             * @param oldAssignment The assignment to copy.
             * @param renaming A mapping from names that are to be renamed to the names they are to be replaced with.
             * @param filename The filename in which the assignment is defined.
             * @param lineNumber The line number in which the assignment is defined.
             */
            Assignment(Assignment const& oldAssignment, std::map<std::string, std::string> const& renaming, std::string const& filename = "", uint_fast64_t lineNumber = 0);
            
            // Create default implementations of constructors/assignment.
            Assignment() = default;
            Assignment(Assignment const& other) = default;
            Assignment& operator=(Assignment const& other)= default;
            Assignment(Assignment&& other) = default;
            Assignment& operator=(Assignment&& other) = default;
            
            /*!
             * Retrieves the name of the variable that this assignment targets.
             *
             * @return The name of the variable that this assignment targets.
             */
            std::string const& getVariableName() const;
            
            /*!
             * Retrieves the expression that is assigned to the variable.
             *
             * @return The expression that is assigned to the variable.
             */
            storm::expressions::Expression const& getExpression() const;
            
            friend std::ostream& operator<<(std::ostream& stream, Assignment const& assignment);
            
        private:
            // The name of the variable that this assignment targets.
            std::string variableName;
            
            // The expression that is assigned to the variable.
            storm::expressions::Expression expression;
        };
    } // namespace ir
} // namespace storm

#endif /* STORM_STORAGE_PRISM_ASSIGNMENT_H_ */
