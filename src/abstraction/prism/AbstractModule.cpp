#include "src/abstraction/prism/AbstractModule.h"

#include "src/abstraction/AbstractionInformation.h"
#include "src/abstraction/BottomStateResult.h"
#include "src/abstraction/prism/GameBddResult.h"

#include "src/storage/dd/DdManager.h"
#include "src/storage/dd/Add.h"

#include "src/storage/prism/Module.h"

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
                for (auto& command : commands) {
                    command.refine(predicates);
                }
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            GameBddResult<DdType> AbstractModule<DdType, ValueType>::getAbstractBdd() {
                // First, we retrieve the abstractions of all commands.
                std::vector<GameBddResult<DdType>> commandDdsAndUsedOptionVariableCounts;
                uint_fast64_t maximalNumberOfUsedOptionVariables = 0;
                uint_fast64_t nextFreePlayer2Index = 0;
                for (auto& command : commands) {
                    commandDdsAndUsedOptionVariableCounts.push_back(command.getAbstractBdd());
                    maximalNumberOfUsedOptionVariables = std::max(maximalNumberOfUsedOptionVariables, commandDdsAndUsedOptionVariableCounts.back().numberOfPlayer2Variables);
                    nextFreePlayer2Index = std::max(nextFreePlayer2Index, commandDdsAndUsedOptionVariableCounts.back().nextFreePlayer2Index);
                }
                
                // Then, we build the module BDD by adding the single command DDs. We need to make sure that all command
                // DDs use the same amount DD variable encoding the choices of player 2.
                storm::dd::Bdd<DdType> result = this->getAbstractionInformation().getDdManager().getBddZero();
                for (auto const& commandDd : commandDdsAndUsedOptionVariableCounts) {
                    result |= commandDd.bdd && this->getAbstractionInformation().getPlayer2ZeroCube(maximalNumberOfUsedOptionVariables, commandDd.numberOfPlayer2Variables);
                }
                return GameBddResult<DdType>(result, maximalNumberOfUsedOptionVariables, nextFreePlayer2Index);
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
            AbstractionInformation<DdType> const& AbstractModule<DdType, ValueType>::getAbstractionInformation() const {
                return abstractionInformation.get();
            }
            
            template class AbstractModule<storm::dd::DdType::CUDD, double>;
            template class AbstractModule<storm::dd::DdType::Sylvan, double>;
        }
    }
}