#ifndef STORM_STORAGE_PRISM_CONSTANT_H_
#define STORM_STORAGE_PRISM_CONSTANT_H_

#include <map>

#include "src/storage/prism/LocatedInformation.h"
#include "src/storage/expressions/Expression.h"
#include "src/utility/OsDetection.h"

namespace storm {
    namespace prism {
        class Constant : public LocatedInformation {
        public:
            /*!
             * Creates a constant with the given type, name and defining expression.
             *
             * @param type The type of the constant.
             * @param name The name of the constant.
             * @param expression The expression that defines the constant.
             * @param filename The filename in which the transition reward is defined.
             * @param lineNumber The line number in which the transition reward is defined.
             */
            Constant(storm::expressions::ExpressionReturnType type, std::string const& name, storm::expressions::Expression const& expression, std::string const& filename = "", uint_fast64_t lineNumber = 0);

            /*!
             * Creates an undefined constant with the given type and name.
             *
             * @param constantType The type of the constant.
             * @param constantName The name of the constant.
             * @param filename The filename in which the transition reward is defined.
             * @param lineNumber The line number in which the transition reward is defined.
             */
            Constant(storm::expressions::ExpressionReturnType constantType, std::string const& constantName, std::string const& filename = "", uint_fast64_t lineNumber = 0);
            
            // Create default implementations of constructors/assignment.
            Constant() = default;
            Constant(Constant const& other) = default;
            Constant& operator=(Constant const& other)= default;
#ifndef WINDOWS
            Constant(Constant&& other) = default;
            Constant& operator=(Constant&& other) = default;
#endif
            
            /*!
             * Retrieves the name of the constant.
             *
             * @return The name of the constant.
             */
            std::string const& getName() const;
            
            /*!
             * Retrieves the type of the constant.
             *
             * @return The type of the constant;
             */
            storm::expressions::ExpressionReturnType getType() const;
            
            /*!
             * Retrieves whether the constant is defined, i.e., whether there is an expression defining its value.
             *
             * @return True iff the constant is defined.
             */
            bool isDefined() const;
            
            /*!
             * Retrieves the expression that defines the constant. This may only be called if the object is a defined
             * constant.
             *
             * @return The expression that defines the constant.
             */
            storm::expressions::Expression const& getExpression() const;
            
            /*!
             * Substitutes all identifiers in the constant according to the given map.
             *
             * @param substitution The substitution to perform.
             * @return The resulting constant.
             */
            Constant substitute(std::map<std::string, storm::expressions::Expression> const& substitution) const;
            
            friend std::ostream& operator<<(std::ostream& stream, Constant const& constant);
            
        private:
            // The type of the constant.
            storm::expressions::ExpressionReturnType type;
            
            // The name of the constant.
            std::string name;
            
            // A flag that stores whether or not the constant is defined.
            bool defined;
            
            // The expression that defines the constant (in case it is defined).
            storm::expressions::Expression expression;
        };
    } // namespace prism
} // namespace storm

#endif /* STORM_STORAGE_PRISM_CONSTANT_H_ */
