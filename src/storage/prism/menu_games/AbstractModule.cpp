#include "src/storage/prism/menu_games/AbstractModule.h"

#include "src/storage/prism/menu_games/AbstractionExpressionInformation.h"
#include "src/storage/prism/menu_games/AbstractionDdInformation.h"

#include "src/storage/prism/Module.h"

namespace storm {
    namespace prism {
        namespace menu_games {
            
            template <storm::dd::DdType DdType, typename ValueType>
            AbstractModule<DdType, ValueType>::AbstractModule(storm::prism::Module const& module, AbstractionExpressionInformation const& expressionInformation, AbstractionDdInformation<DdType, ValueType> const& ddInformation, storm::utility::solver::SmtSolverFactory const& smtSolverFactory) : smtSolverFactory(smtSolverFactory), commands(), module(module) {
                
                // For each concrete command, we create an abstract counterpart.
                for (auto const& command : module.getCommands()) {
                    commands.emplace_back(command, expressionInformation, ddInformation, smtSolverFactory);
                }
            }
            
            template class AbstractModule<storm::dd::DdType::CUDD, double>;
        }
    }
}