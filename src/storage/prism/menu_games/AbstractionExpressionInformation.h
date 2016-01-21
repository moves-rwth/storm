#ifndef STORM_STORAGE_PRISM_MENU_GAMES_ABSTRACTIONEXPRESSIONINFORMATION_H_
#define STORM_STORAGE_PRISM_MENU_GAMES_ABSTRACTIONEXPRESSIONINFORMATION_H_

#include <vector>
#include <set>

namespace storm {
    namespace expressions {
        class ExpressionManager;
        class Expression;
        class Variable;
    }
    
    namespace prism {
        namespace menu_games {
            
            struct AbstractionExpressionInformation {
            public:
                /*!
                 * Creates an expression information object with the given expression manager.
                 *
                 * @param manager The expression manager to use.
                 * @param predicates The initial set of predicates.
                 * @param variables The variables.
                 * @param rangeExpressions A set of expressions that enforce the variable bounds.
                 */
                AbstractionExpressionInformation(storm::expressions::ExpressionManager& manager, std::vector<storm::expressions::Expression> const& predicates = std::vector<storm::expressions::Expression>(), std::set<storm::expressions::Variable> const& variables = std::set<storm::expressions::Variable>(), std::vector<storm::expressions::Expression> const& rangeExpressions = std::vector<storm::expressions::Expression>());
                
                /*!
                 * Adds the given predicate.
                 *
                 * @param predicate The predicate to add.
                 */
                void addPredicate(storm::expressions::Expression const& predicate);

                /*!
                 * Adds the given predicates.
                 *
                 * @param predicates The predicates to add.
                 */
                void addPredicates(std::vector<storm::expressions::Expression> const& predicates);
                
                /*!
                 * Retrieves the expression manager.
                 *
                 * @return The manager.
                 */
                storm::expressions::ExpressionManager& getManager();

                /*!
                 * Retrieves the expression manager.
                 *
                 * @return The manager.
                 */
                storm::expressions::ExpressionManager const& getManager() const;
                
                /*!
                 * Retrieves all currently known predicates.
                 *
                 * @return The list of known predicates.
                 */
                std::vector<storm::expressions::Expression>& getPredicates();

                /*!
                 * Retrieves all currently known predicates.
                 *
                 * @return The list of known predicates.
                 */
                std::vector<storm::expressions::Expression> const& getPredicates() const;
                
                /*!
                 * Retrieves the predicate with the given index.
                 *
                 * @param index The index of the predicate.
                 */
                storm::expressions::Expression const& getPredicateByIndex(uint_fast64_t index) const;

                /*!
                 * Retrieves the number of predicates.
                 *
                 * @return The number of predicates.
                 */
                std::size_t getNumberOfPredicates() const;
                
                /*!
                 * Retrieves all currently known variables.
                 *
                 * @return The set of known variables.
                 */
                std::set<storm::expressions::Variable>& getVariables();
                
                /*!
                 * Retrieves all currently known variables.
                 *
                 * @return The set of known variables.
                 */
                std::set<storm::expressions::Variable> const& getVariables() const;
                
                /*!
                 * Retrieves a list of expressions that ensure the ranges of the variables.
                 *
                 * @return The range expressions.
                 */
                std::vector<storm::expressions::Expression>& getRangeExpressions();
                
                /*!
                 * Retrieves a list of expressions that ensure the ranges of the variables.
                 *
                 * @return The range expressions.
                 */
                std::vector<storm::expressions::Expression> const& getRangeExpressions() const;
                
            private:
                // The manager responsible for the expressions of the program and the SMT solvers.
                storm::expressions::ExpressionManager& manager;
                
                // The current set of predicates used in the abstraction.
                std::vector<storm::expressions::Expression> predicates;
                
                // The set of all variables.
                std::set<storm::expressions::Variable> variables;

                // The expression characterizing the legal ranges of all variables.
                std::vector<storm::expressions::Expression> rangeExpressions;
            };
        }
    }
}

#endif /* STORM_STORAGE_PRISM_MENU_GAMES_ABSTRACTIONEXPRESSIONINFORMATION_H_ */