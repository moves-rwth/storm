#ifndef STORM_STORAGE_PRISM_CONSTANT_H_
#define STORM_STORAGE_PRISM_CONSTANT_H_

#include "src/storage/prism/LocatedInformation.h"
#include "src/storage/expressions/Expression.h"

namespace storm {
    namespace prism {
        class Constant : public LocatedInformation {
        public:
            /*!
             * The possible constant types.
             */
            enum class ConstantType {Bool, Integer, Double};
            
            /*!
             * Creates a constant with the given type, name and defining expression.
             *
             * @param constantType The type of the constant.
             * @param constantName The name of the constant.
             * @param expression The expression that defines the constant.
             * @param filename The filename in which the transition reward is defined.
             * @param lineNumber The line number in which the transition reward is defined.
             */
            Constant(ConstantType constantType, std::string const& constantName, storm::expressions::Expression const& expression, std::string const& filename = "", uint_fast64_t lineNumber = 0);

            /*!
             * Creates an undefined constant with the given type and name.
             *
             * @param constantType The type of the constant.
             * @param constantName The name of the constant.
             * @param filename The filename in which the transition reward is defined.
             * @param lineNumber The line number in which the transition reward is defined.
             */
            Constant(ConstantType constantType, std::string const& constantName, std::string const& filename = "", uint_fast64_t lineNumber = 0);
            
            // Create default implementations of constructors/assignment.
            Constant() = default;
            Constant(Constant const& other) = default;
            Constant& operator=(Constant const& other)= default;
            Constant(Constant&& other) = default;
            Constant& operator=(Constant&& other) = default;
            
            /*!
             * Retrieves the name of the constant.
             *
             * @return The name of the constant.
             */
            std::string const& getConstantName() const;
            
            /*!
             * Retrieves the type of the constant.
             *
             * @return The type of the constant;
             */
            ConstantType getConstantType() const;
            
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
            
            friend std::ostream& operator<<(std::ostream& stream, Constant const& constant);
            
        private:
            // The type of the constant.
            ConstantType constantType;
            
            // The name of the constant.
            std::string constantName;
            
            // A flag that stores whether or not the constant is defined.
            bool defined;
            
            // The expression that defines the constant (in case it is defined).
            storm::expressions::Expression expression;
        };
    } // namespace prism
} // namespace storm

#endif /* STORM_STORAGE_PRISM_CONSTANT_H_ */
