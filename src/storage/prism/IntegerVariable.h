#ifndef STORM_STORAGE_PRISM_INTEGERVARIABLE_H_
#define STORM_STORAGE_PRISM_INTEGERVARIABLE_H_

#include <map>

#include "src/storage/prism/Variable.h"
#include "src/utility/OsDetection.h"

namespace storm {
    namespace prism {
        class IntegerVariable : public Variable {
        public:
            // Create default implementations of constructors/assignment.
            IntegerVariable() = default;
            IntegerVariable(IntegerVariable const& other) = default;
            IntegerVariable& operator=(IntegerVariable const& other)= default;
#ifndef WINDOWS
            IntegerVariable(IntegerVariable&& other) = default;
            IntegerVariable& operator=(IntegerVariable&& other) = default;
#endif

            /*!
             * Creates an integer variable with the given name and a default initial value.
             *
             * @param variableName The name of the variable.
             * @param lowerBoundExpression A constant expression defining the lower bound of the domain of the variable.
             * @param upperBoundExpression A constant expression defining the upper bound of the domain of the variable.
             * @param filename The filename in which the variable is defined.
             * @param lineNumber The line number in which the variable is defined.
             */
            IntegerVariable(std::string const& variableName, storm::expressions::Expression const& lowerBoundExpression, storm::expressions::Expression const& upperBoundExpression, std::string const& filename = "", uint_fast64_t lineNumber = 0);

            /*!
             * Creates an integer variable with the given name and the given initial value expression.
             *
             * @param variableName The name of the variable.
             * @param lowerBoundExpression A constant expression defining the lower bound of the domain of the variable.
             * @param upperBoundExpression A constant expression defining the upper bound of the domain of the variable.
             * @param initialValueExpression A constant expression that defines the initial value of the variable.
             * @param filename The filename in which the variable is defined.
             * @param lineNumber The line number in which the variable is defined.
             */
            IntegerVariable(std::string const& variableName, storm::expressions::Expression const& lowerBoundExpression, storm::expressions::Expression const& upperBoundExpression, storm::expressions::Expression const& initialValueExpression, std::string const& filename = "", uint_fast64_t lineNumber = 0);
            
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
            
            /*!
             * Substitutes all identifiers in the boolean variable according to the given map.
             *
             * @param substitution The substitution to perform.
             * @return The resulting boolean variable.
             */
            IntegerVariable substitute(std::map<std::string, storm::expressions::Expression> const& substitution) const;
            
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
