#include "src/storage/prism/menu_games/AbstractProgram.h"

#include "src/storage/prism/Program.h"

#include "src/storage/dd/CuddDdManager.h"
#include "src/storage/dd/CuddAdd.h"

#include "src/utility/macros.h"
#include "src/utility/solver.h"
#include "src/exceptions/WrongFormatException.h"

namespace storm {
    namespace prism {
        namespace menu_games {
            
            template <storm::dd::DdType DdType, typename ValueType>
            AbstractProgram<DdType, ValueType>::AbstractProgram(storm::expressions::ExpressionManager& expressionManager, storm::prism::Program const& program, std::vector<storm::expressions::Expression> const& initialPredicates, std::unique_ptr<storm::utility::solver::SmtSolverFactory>&& smtSolverFactory, bool addAllGuards) : smtSolverFactory(std::move(smtSolverFactory)), ddInformation(std::make_shared<storm::dd::DdManager<DdType>>()), expressionInformation(expressionManager, initialPredicates, program.getAllExpressionVariables(), program.getAllRangeExpressions()), modules(), program(program) {
                
                // For now, we assume that there is a single module. If the program has more than one module, it needs
                // to be flattened before the procedure.
                STORM_LOG_THROW(program.getNumberOfModules() == 1, storm::exceptions::WrongFormatException, "Cannot create abstract program from program containing too many modules.");
                
                uint_fast64_t totalNumberOfCommands = 0;
                uint_fast64_t maximalUpdateCount = 0;
                for (auto const& module : program.getModules()) {
                    // If we were requested to add all guards to the set of predicates, we do so now.
                    for (auto const& command : module.getCommands()) {
                        if (addAllGuards) {
                            expressionInformation.predicates.push_back(command.getGuardExpression());
                        }
                        maximalUpdateCount = std::max(maximalUpdateCount, static_cast<uint_fast64_t>(command.getNumberOfUpdates()));
                    }
                
                    totalNumberOfCommands += module.getNumberOfCommands();
                }
                
                // Create DD variables for all predicates.
                for (auto const& predicate : expressionInformation.predicates) {
                    ddInformation.addPredicate(predicate);
                }
                
                // Create DD variable for the command encoding.
                ddInformation.commandDdVariable = ddInformation.manager->addMetaVariable("command", 0, totalNumberOfCommands - 1).first;
                
                // Create DD variable for update encoding.
                ddInformation.updateDdVariable = ddInformation.manager->addMetaVariable("update", 0, maximalUpdateCount - 1).first;
                
                // Create DD variables encoding the nondeterministic choices of player 2.
                // NOTE: currently we assume that 100 variables suffice, which corresponds to 2^100 possible choices.
                // If for some reason this should not be enough, we could grow this vector dynamically, but odds are
                // that it's impossible to treat such models in any event.
                for (uint_fast64_t index = 0; index < 100; ++index) {
                    storm::expressions::Variable newOptionVar = ddInformation.manager->addMetaVariable("opt" + std::to_string(index)).first;
                    ddInformation.optionDdVariables.push_back(std::make_pair(newOptionVar, ddInformation.manager->getRange(newOptionVar)));
                }
                
                // For each module of the concrete program, we create an abstract counterpart.
                for (auto const& module : program.getModules()) {
                    modules.emplace_back(module, expressionInformation, ddInformation, *smtSolverFactory);
                }
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            storm::dd::Add<DdType> AbstractProgram<DdType, ValueType>::computeDd() {
                // As long as there is only one module, we build its game representation and return it.
                // FIXME: multiply with probabilities for updates.
                return modules.front().computeDd().toAdd();
            }
            
            // Explicitly instantiate the class.
            template class AbstractProgram<storm::dd::DdType::CUDD, double>;
            
        }
    }
}