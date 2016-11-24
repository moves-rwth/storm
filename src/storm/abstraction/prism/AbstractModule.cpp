#include "storm/abstraction/prism/AbstractModule.h"

#include "storm/abstraction/AbstractionInformation.h"
#include "storm/abstraction/BottomStateResult.h"
#include "storm/abstraction/GameBddResult.h"

#include "storm/storage/dd/DdManager.h"
#include "storm/storage/dd/Add.h"

#include "storm/storage/prism/Module.h"

#include "storm-config.h"
#include "storm/adapters/CarlAdapter.h"

#include "storm/utility/macros.h"

namespace storm {
    namespace abstraction {
        namespace prism {
            
            template <storm::dd::DdType DdType, typename ValueType>
            AbstractModule<DdType, ValueType>::AbstractModule(storm::prism::Module const& module, AbstractionInformation<DdType>& abstractionInformation, std::shared_ptr<storm::utility::solver::SmtSolverFactory> const& smtSolverFactory, bool allGuardsAdded) : smtSolverFactory(smtSolverFactory), abstractionInformation(abstractionInformation), commands(), module(module) {
                
                // For each concrete command, we create an abstract counterpart.
                for (auto const& command : module.getCommands()) {
                    commands.emplace_back(command, abstractionInformation, smtSolverFactory, allGuardsAdded);
                }
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            void AbstractModule<DdType, ValueType>::refine(std::vector<uint_fast64_t> const& predicates) {
                for (uint_fast64_t index = 0; index < commands.size(); ++index) {
                    STORM_LOG_TRACE("Refining command with index " << index << ".");
                    AbstractCommand<DdType, ValueType>& command = commands[index];
                    command.refine(predicates);
                }
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            GameBddResult<DdType> AbstractModule<DdType, ValueType>::getAbstractBdd() {
                // First, we retrieve the abstractions of all commands.
                std::vector<GameBddResult<DdType>> commandDdsAndUsedOptionVariableCounts;
                uint_fast64_t maximalNumberOfUsedOptionVariables = 0;
                for (auto& command : commands) {
                    commandDdsAndUsedOptionVariableCounts.push_back(command.getAbstractBdd());
                    maximalNumberOfUsedOptionVariables = std::max(maximalNumberOfUsedOptionVariables, commandDdsAndUsedOptionVariableCounts.back().numberOfPlayer2Variables);
                }
                
                // Then, we build the module BDD by adding the single command DDs. We need to make sure that all command
                // DDs use the same amount DD variable encoding the choices of player 2.
                storm::dd::Bdd<DdType> result = this->getAbstractionInformation().getDdManager().getBddZero();
                for (auto const& commandDd : commandDdsAndUsedOptionVariableCounts) {
                    result |= commandDd.bdd && this->getAbstractionInformation().getPlayer2ZeroCube(commandDd.numberOfPlayer2Variables, maximalNumberOfUsedOptionVariables);
                }
                return GameBddResult<DdType>(result, maximalNumberOfUsedOptionVariables);
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            BottomStateResult<DdType> AbstractModule<DdType, ValueType>::getBottomStateTransitions(storm::dd::Bdd<DdType> const& reachableStates, uint_fast64_t numberOfPlayer2Variables) {
                BottomStateResult<DdType> result(this->getAbstractionInformation().getDdManager().getBddZero(), this->getAbstractionInformation().getDdManager().getBddZero());
                
                for (auto& command : commands) {
                    BottomStateResult<DdType> commandBottomStateResult = command.getBottomStateTransitions(reachableStates, numberOfPlayer2Variables);
                    result.states |= commandBottomStateResult.states;
                    result.transitions |= commandBottomStateResult.transitions;
                }
                
                return result;
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            storm::dd::Add<DdType, ValueType> AbstractModule<DdType, ValueType>::getCommandUpdateProbabilitiesAdd() const {
                storm::dd::Add<DdType, ValueType> result = this->getAbstractionInformation().getDdManager().template getAddZero<ValueType>();
                for (auto const& command : commands) {
                    result += command.getCommandUpdateProbabilitiesAdd();
                }
                return result;
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            std::vector<AbstractCommand<DdType, ValueType>> const& AbstractModule<DdType, ValueType>::getCommands() const {
                return commands;
            }

            template <storm::dd::DdType DdType, typename ValueType>
            std::vector<AbstractCommand<DdType, ValueType>>& AbstractModule<DdType, ValueType>::getCommands() {
                return commands;
            }

            template <storm::dd::DdType DdType, typename ValueType>
            AbstractionInformation<DdType> const& AbstractModule<DdType, ValueType>::getAbstractionInformation() const {
                return abstractionInformation.get();
            }
            
            template class AbstractModule<storm::dd::DdType::CUDD, double>;
            template class AbstractModule<storm::dd::DdType::Sylvan, double>;
#ifdef STORM_HAVE_CARL
			template class AbstractModule<storm::dd::DdType::Sylvan, storm::RationalFunction>;
#endif
        }
    }
}
