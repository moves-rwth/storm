#include "src/abstraction/prism/AbstractModule.h"

#include "src/abstraction/AbstractionExpressionInformation.h"
#include "src/abstraction/AbstractionDdInformation.h"

#include "src/storage/dd/DdManager.h"
#include "src/storage/dd/Add.h"

#include "src/storage/prism/Module.h"

namespace storm {
    namespace abstraction {
        namespace prism {
            
            template <storm::dd::DdType DdType, typename ValueType>
            AbstractModule<DdType, ValueType>::AbstractModule(storm::prism::Module const& module, AbstractionExpressionInformation& expressionInformation, AbstractionDdInformation<DdType, ValueType> const& ddInformation, storm::utility::solver::SmtSolverFactory const& smtSolverFactory) : smtSolverFactory(smtSolverFactory), ddInformation(ddInformation), commands(), module(module) {
                
                // For each concrete command, we create an abstract counterpart.
                for (auto const& command : module.getCommands()) {
                    commands.emplace_back(command, expressionInformation, ddInformation, smtSolverFactory);
                }
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            void AbstractModule<DdType, ValueType>::refine(std::vector<uint_fast64_t> const& predicates) {
                for (auto& command : commands) {
                    command.refine(predicates);
                }
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            std::pair<storm::dd::Bdd<DdType>, uint_fast64_t> AbstractModule<DdType, ValueType>::getAbstractBdd() {
                // First, we retrieve the abstractions of all commands.
                std::vector<std::pair<storm::dd::Bdd<DdType>, uint_fast64_t>> commandDdsAndUsedOptionVariableCounts;
                uint_fast64_t maximalNumberOfUsedOptionVariables = 0;
                for (auto& command : commands) {
                    commandDdsAndUsedOptionVariableCounts.push_back(command.getAbstractBdd());
                    maximalNumberOfUsedOptionVariables = std::max(maximalNumberOfUsedOptionVariables, commandDdsAndUsedOptionVariableCounts.back().second);
                }
                
                // Then, we build the module BDD by adding the single command DDs. We need to make sure that all command
                // DDs use the same amount DD variable encoding the choices of player 2.
                storm::dd::Bdd<DdType> result = ddInformation.manager->getBddZero();
                for (auto const& commandDd : commandDdsAndUsedOptionVariableCounts) {
                    result |= commandDd.first && ddInformation.getMissingOptionVariableCube(commandDd.second, maximalNumberOfUsedOptionVariables);
                }
                return std::make_pair(result, maximalNumberOfUsedOptionVariables);
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            storm::dd::Add<DdType, ValueType> AbstractModule<DdType, ValueType>::getCommandUpdateProbabilitiesAdd() const {
                storm::dd::Add<DdType, ValueType> result = ddInformation.manager->template getAddZero<ValueType>();
                for (auto const& command : commands) {
                    result += command.getCommandUpdateProbabilitiesAdd();
                }
                return result;
            }
            
            template class AbstractModule<storm::dd::DdType::CUDD, double>;
            template class AbstractModule<storm::dd::DdType::Sylvan, double>;
        }
    }
}