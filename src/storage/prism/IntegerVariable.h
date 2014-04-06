#ifndef STORM_STORAGE_PRISM_INTEGERVARIABLE_H_
#define STORM_STORAGE_PRISM_INTEGERVARIABLE_H_

#include <map>

#include "src/storage/prism/Variable.h"

namespace storm {
    namespace prism {
        class IntegerVariable : public Variable {
        public:
            // Create default implementations of constructors/assignment.
            IntegerVariable() = default;
            IntegerVariable(IntegerVariable const& otherVariable) = default;
            IntegerVariable& operator=(IntegerVariable const& otherVariable)= default;
            IntegerVariable(IntegerVariable&& otherVariable) = default;
            IntegerVariable& operator=(IntegerVariable&& otherVariable) = default;

            /*!
             * Creates an integer variable with the given name and a default initial value.
             *
             * @param variableName The name of the variable.
             * @param lowerBoundExpression A constant expression defining the lower bound of the domain of the variable.
             * @param upperBoundExpression A constant expression defining the upper bound of the domain of the variable.
             */
            IntegerVariable(std::string const& variableName, storm::expressions::Expression const& lowerBoundExpression, storm::expressions::Expression const& upperBoundExpression);

            /*!
             * Creates an integer variable with the given name and the given initial value expression.
             *
             * @param variableName The name of the variable.
             * @param lowerBoundExpression A constant expression defining the lower bound of the domain of the variable.
             * @param upperBoundExpression A constant expression defining the upper bound of the domain of the variable.
             * @param initialValueExpression A constant expression that defines the initial value of the variable.
             */
            IntegerVariable(std::string const& variableName, storm::expressions::Expression const& lowerBoundExpression, storm::expressions::Expression const& upperBoundExpression, storm::expressions::Expression const& initialValueExpression);
            
            /*!
             * Creates a copy of the given integer variable and performs the provided renaming.
             *
             * @param oldVariable The variable to copy.
             * @param newName New name of this variable.
             * @param renaming A mapping from names that are to be renamed to the names they are to be replaced with.
             */
            IntegerVariable(IntegerVariable const& oldVariable, std::string const& newName, std::map<std::string, std::string> const& renaming);
            
            /*!
             * Retrieves an expression defining the lower bound for this integer variable.
             *
             * @return An expression defining the lower bound for this integer variable.
             */
            storm::expressions::Expression const& getLowerBoundExpression() const;
            
            /*!
             * Retrieves an expression defining the upper bound for this integer variable.
             *
             * @return An expression defining the upper bound for this integer variable.
             */
            storm::expressions::Expression const& getUpperBoundExpression() const;
            
            friend std::ostream& operator<<(std::ostream& stream, IntegerVariable const& variable);
            
        private:
            // A constant expression that specifies the lower bound of the domain of the variable.
            storm::expressions::Expression lowerBoundExpression;
            
            // A constant expression that specifies the upper bound of the domain of the variable.
            storm::expressions::Expression upperBoundExpression;
        };
        
    } // namespace prism
} // namespace storm

#endif /* STORM_STORAGE_PRISM_INTEGERVARIABLE_H_ */
