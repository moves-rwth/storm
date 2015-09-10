#include "src/storage/prism/menu_games/AbstractCommand.h"

#include "src/storage/prism/Command.h"
#include "src/storage/prism/Update.h"

namespace storm {
    namespace prism {
        namespace menu_games {
            template <storm::dd::DdType DdType, typename ValueType>
            AbstractCommand<DdType, ValueType>::AbstractCommand(storm::expressions::ExpressionManager& expressionManager, storm::prism::Command const& command, std::vector<storm::expressions::Expression> const& initialPredicates, storm::utility::solver::SmtSolverFactory const& smtSolverFactory) : expressionManager(expressionManager), smtSolver(smtSolverFactory.create(expressionManager)), predicates(initialPredicates), command(command) {
                // Intentionally left empty.
            }
            
            template class AbstractCommand<storm::dd::DdType::CUDD, double>;
        }
    }
}