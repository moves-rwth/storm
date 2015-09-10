#ifndef STORM_STORAGE_PRISM_MENU_GAMES_ABSTRACTCOMMAND_H_
#define STORM_STORAGE_PRISM_MENU_GAMES_ABSTRACTCOMMAND_H_

#include <memory>

#include "src/storage/dd/DdType.h"
#include "src/storage/expressions/Expression.h"

#include "src/solver/SmtSolver.h"
#include "src/utility/solver.h"

namespace storm {
    namespace prism {
        // Forward-declare concrete command and update classes.
        class Command;
        
        namespace menu_games {
            template <storm::dd::DdType DdType, typename ValueType>
            class AbstractCommand {
            public:
                /*!
                 * Constructs an abstract command from the given command and the initial predicates.
                 *
                 * @param expressionManager The manager responsible for the expressions of the command.
                 * @param command The concrete command for which to build the abstraction.
                 * @param initialPredicates The initial set of predicates.
                 * @param smtSolverFactory A factory that is to be used for creating new SMT solvers.
                 */
                AbstractCommand(storm::expressions::ExpressionManager& expressionManager, storm::prism::Command const& command, std::vector<storm::expressions::Expression> const& initialPredicates, storm::utility::solver::SmtSolverFactory const& smtSolverFactory);
                
            private:
                // The manager responsible for the expressions of the command and the SMT solvers.
                storm::expressions::ExpressionManager& expressionManager;
                
                // An SMT responsible for this abstract command.
                std::unique_ptr<storm::solver::SmtSolver> smtSolver;
                
                // The current set of predicates used in the abstraction.
                std::vector<storm::expressions::Expression> predicates;
                
                // The concrete command this abstract command refers to.
                std::reference_wrapper<Command const> command;
            };
        }
    }
}

#endif /* STORM_STORAGE_PRISM_MENU_GAMES_ABSTRACTCOMMAND_H_ */