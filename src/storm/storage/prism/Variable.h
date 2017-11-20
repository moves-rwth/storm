#ifndef STORM_STORAGE_PRISM_VARIABLE_H_
#define STORM_STORAGE_PRISM_VARIABLE_H_

#include <map>

#include "storm/storage/prism/LocatedInformation.h"
#include "storm/storage/expressions/Variable.h"
#include "storm/storage/expressions/Expression.h"
#include "storm/utility/OsDetection.h"

namespace storm {
    namespace prism {
        class Variable : public LocatedInformation {
        public:
            // Create default implementations of constructors/assignment.
            Variable(Variable const& otherVariable) = default;
            Variable& operator=(Variable const& otherVariable)= default;
            Variable(Variable&& otherVariable) = default;
            Variable& operator=(Variable&& otherVariable) = default;
            virtual ~Variable() = default;
            
            /*!
             * Retrieves the name of the variable.
             *
             * @return The name of the variable.
             */
            std::string const& getName() const;
            
            /*!
             * Retrieves whether the variable has an initial value.
             *
             * @return True iff the variable has an initial value.
             */
            bool hasInitialValue() const;
            
            /*!
             * Retrieves the expression defining the initial value of the variable. This can only be called if there is
             * an initial value (expression).
             *
             * @return The expression defining the initial value of the variable.
             */
            storm::expressions::Expression const& getInitialValueExpression() const;

            /*!
             * Sets the expression defining the initial value of the variable.
             *
             * @param initialValueExpression The expression defining the initial value of the variable.
             */
            void setInitialValueExpression(storm::expressions::Expression const& initialValueExpression);

            /*!
             * Retrieves the expression variable associated with this variable.
             *
             * @return The expression variable associated with this variable.
             */
            storm::expressions::Variable const& getExpressionVariable() const;
            
            /*!
             * Retrieves the expression associated with this variable.
             *
             * @return The expression associated with this variable.
             */
            storm::expressions::Expression getExpression() const;
            
            /*!
             * Equips the variable with an initial value based on its type if not initial value is present.
             */
            virtual void createMissingInitialValue() = 0;
            
            // Make the constructors protected to forbid instantiation of this class.
        protected:
            Variable() = default;
            
            /*!
             * Creates a variable with the given initial value.
             *
             * @param variable The associated expression variable.
             * @param initialValueExpression The constant expression that defines the initial value of the variable.
             * @param filename The filename in which the variable is defined.
             * @param lineNumber The line number in which the variable is defined.
             */
            Variable(storm::expressions::Variable const& variable, storm::expressions::Expression const& initialValueExpression, std::string const& filename = "", uint_fast64_t lineNumber = 0);
            
            /*!
             * Creates a copy of the given variable and performs the provided renaming.
             *
             * @param manager The manager responsible for the variable.
             * @param oldVariable The variable to copy.
             * @param newName New name of this variable.
             * @param renaming A mapping from variables to the expressions with which they are to be replaced.
             * @param filename The filename in which the variable is defined.
             * @param lineNumber The line number in which the variable is defined.
             */
            Variable(storm::expressions::ExpressionManager& manager, Variable const& oldVariable, std::string const& newName, std::map<storm::expressions::Variable, storm::expressions::Expression> const& renaming, std::string const& filename = "", uint_fast64_t lineNumber = 0);
            
        private:
            // The expression variable associated with this variable.
            storm::expressions::Variable variable;
            
            // The constant expression defining the initial value of the variable.
            storm::expressions::Expression initialValueExpression;
        };
        
    } // namespace prism
} // namespace storm

#endif /* STORM_STORAGE_PRISM_VARIABLE_H_ */
