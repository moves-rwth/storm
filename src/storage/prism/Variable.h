#ifndef STORM_STORAGE_PRISM_VARIABLE_H_
#define STORM_STORAGE_PRISM_VARIABLE_H_

#include <map>

#include "src/storage/prism/LocatedInformation.h"
#include "src/storage/expressions/Expression.h"

namespace storm {
    namespace prism {
        class Variable : public LocatedInformation {
        public:
            // Create default implementations of constructors/assignment.
            Variable(Variable const& otherVariable) = default;
            Variable& operator=(Variable const& otherVariable)= default;
            Variable(Variable&& otherVariable) = default;
            Variable& operator=(Variable&& otherVariable) = default;
            
            /*!
             * Retrieves the name of the variable.
             *
             * @return The name of the variable.
             */
            std::string const& getName() const;
            
            /*!
             * Retrieves the expression defining the initial value of the variable.
             *
             * @return The expression defining the initial value of the variable.
             */
            storm::expressions::Expression const& getInitialValueExpression() const;
            
            /*!
             * Retrieves whether the variable has the default initial value with respect to its type.
             *
             * @return True iff the variable has the default initial value.
             */
            bool hasDefaultInitialValue() const;
            
            // Make the constructors protected to forbid instantiation of this class.
        protected:
            Variable() = default;
            
            /*!
             * Creates a variable with the given name and initial value.
             *
             * @param variableName The name of the variable.
             * @param initialValueExpression The constant expression that defines the initial value of the variable.
             * @param hasDefaultInitialValue A flag indicating whether the initial value of the variable is its default
             * value.
             * @param filename The filename in which the variable is defined.
             * @param lineNumber The line number in which the variable is defined.
             */
            Variable(std::string const& variableName, storm::expressions::Expression const& initialValueExpression, bool defaultInitialValue, std::string const& filename = "", uint_fast64_t lineNumber = 0);
            
            /*!
             * Creates a copy of the given variable and performs the provided renaming.
             *
             * @param oldVariable The variable to copy.
             * @param newName New name of this variable.
             * @param renaming A mapping from names that are to be renamed to the names they are to be replaced with.
             * @param filename The filename in which the variable is defined.
             * @param lineNumber The line number in which the variable is defined.
             */
            Variable(Variable const& oldVariable, std::string const& newName, std::map<std::string, std::string> const& renaming, std::string const& filename = "", uint_fast64_t lineNumber = 0);
            
        private:
            // The name of the variable.
            std::string variableName;
            
            // The constant expression defining the initial value of the variable.
            storm::expressions::Expression initialValueExpression;
            
            // A flag that stores whether the variable has its default initial expression.
            bool defaultInitialValue;
        };
        
    } // namespace prism
} // namespace storm

#endif /* STORM_STORAGE_PRISM_VARIABLE_H_ */
