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