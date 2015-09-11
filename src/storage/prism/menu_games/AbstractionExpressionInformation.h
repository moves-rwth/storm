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
                 * @param expressionManager The expression manager to use.
                 */
                AbstractionExpressionInformation(storm::expressions::ExpressionManager& expressionManager, std::vector<storm::expressions::Expression> const& predicates = std::vector<storm::expressions::Expression>(), std::set<storm::expressions::Variable> const& variables = std::set<storm::expressions::Variable>());
                
                // The manager responsible for the expressions of the program and the SMT solvers.
                storm::expressions::ExpressionManager& expressionManager;
                
                // The current set of predicates used in the abstraction.
                std::vector<storm::expressions::Expression> predicates;
                
                // The set of all variables.
                std::set<storm::expressions::Variable> variables;
            };
        }
    }
}

#endif /* STORM_STORAGE_PRISM_MENU_GAMES_ABSTRACTIONEXPRESSIONINFORMATION_H_ */