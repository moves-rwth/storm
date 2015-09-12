#include "src/storage/prism/menu_games/AbstractModule.h"

#include "src/storage/prism/menu_games/AbstractionExpressionInformation.h"
#include "src/storage/prism/menu_games/AbstractionDdInformation.h"

#include "src/storage/dd/CuddDdManager.h"
#include "src/storage/dd/CuddAdd.h"

#include "src/storage/prism/Module.h"

namespace storm {
    namespace prism {
        namespace menu_games {
            
            template <storm::dd::DdType DdType, typename ValueType>
            AbstractModule<DdType, ValueType>::AbstractModule(storm::prism::Module const& module, AbstractionExpressionInformation const& expressionInformation, AbstractionDdInformation<DdType, ValueType> const& ddInformation, storm::utility::solver::SmtSolverFactory const& smtSolverFactory) : smtSolverFactory(smtSolverFactory), ddInformation(ddInformation), commands(), module(module) {
                
                // For each concrete command, we create an abstract counterpart.
                for (auto const& command : module.getCommands()) {
                    commands.emplace_back(command, expressionInformation, ddInformation, smtSolverFactory);
                }
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            storm::dd::Add<DdType> AbstractModule<DdType, ValueType>::computeDd() {
                // First, we retrieve the abstractions of all commands.
                std::vector<std::pair<storm::dd::Add<DdType>, uint_fast64_t>> commandDdsAndUsedOptionVariableCounts;
                for (auto const& command : commands) {
                    commandDdsAndUsedOptionVariableCounts.push_back(command.computeDd());
                }
                
                // Then, we build the module ADD by adding the single command DDs. We need to make sure that all command
                // DDs use the same amount DD variable encoding the choices of player 2.
                storm::dd::Add<DdType> result = ddInformation.ddManager->getAddZero();
                
                // TODO
                
                return result;
            }
            
            template class AbstractModule<storm::dd::DdType::CUDD, double>;
        }
    }
}