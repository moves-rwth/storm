#ifndef STORM_STORAGE_PRISM_FORMULA_H_
#define STORM_STORAGE_PRISM_FORMULA_H_

#include <map>

#include "src/storage/prism/LocatedInformation.h"
#include "src/storage/expressions/Expression.h"

namespace storm {
    namespace prism {
        class Formula : public LocatedInformation {
        public:
            /*!
             * Creates a formula with the given name and expression.
             *
             * @param formulaName The name of the label.
             * @param expression The predicate that needs to hold before taking a transition with the previously
             * specified name in order to obtain the reward.
             * @param filename The filename in which the transition reward is defined.
             * @param lineNumber The line number in which the transition reward is defined.
             */
            Formula(std::string const& formulaName, storm::expressions::Expression const& expression, std::string const& filename = "", uint_fast64_t lineNumber = 0);
            
            // Create default implementations of constructors/assignment.
            Formula() = default;
            Formula(Formula const& other) = default;
            Formula& operator=(Formula const& other)= default;
            Formula(Formula&& other) = default;
            Formula& operator=(Formula&& other) = default;
            
            /*!
             * Retrieves the name that is associated with this formula.
             *
             * @return The name that is associated with this formula.
             */
            std::string const& getFormulaName() const;
            
            /*!
             * Retrieves the expression that is associated with this formula.
             *
             * @return The expression that is associated with this formula.
             */
            storm::expressions::Expression const& getExpression() const;
            
            /*!
             * Retrieves the return type of the formula, i.e., the return-type of the defining expression.
             *
             * @return The return type of the formula.
             */
            storm::expressions::ExpressionReturnType getReturnType() const;
            
            /*!
             * Substitutes all identifiers in the expression of the formula according to the given map.
             *
             * @param substitution The substitution to perform.
             * @return The resulting formula.
             */
            Formula substitute(std::map<std::string, storm::expressions::Expression> const& substitution) const;
            
            friend std::ostream& operator<<(std::ostream& stream, Formula const& formula);
            
        private:
            // The name of the formula.
            std::string formulaName;
            
            // A predicate that needs to be satisfied by states for the label to be attached.
            storm::expressions::Expression expression;
        };
    } // namespace prism
} // namespace storm

#endif /* STORM_STORAGE_PRISM_FORMULA_H_ */
