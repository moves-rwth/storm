#include "src/storage/prism/menu_games/AbstractProgram.h"

#include <sstream>

#include "src/storage/prism/Program.h"

#include "src/storage/dd/CuddDdManager.h"

#include "src/utility/macros.h"
#include "src/exceptions/WrongFormatException.h"

namespace storm {
    namespace prism {
        namespace menu_games {
            
            template <storm::dd::DdType DdType, typename ValueType>
            AbstractProgram<DdType, ValueType>::AbstractProgram(storm::expressions::ExpressionManager& expressionManager, storm::prism::Program const& program, std::vector<storm::expressions::Expression> const& initialPredicates, std::unique_ptr<storm::utility::solver::SmtSolverFactory>&& smtSolverFactory, bool addAllGuards) : expressionManager(expressionManager), smtSolverFactory(std::move(smtSolverFactory)), predicates(initialPredicates), ddManager(new storm::dd::DdManager<DdType>()), predicateDdVariables(), commandDdVariable(), optionDdVariables(), modules(), program(program) {
                
                // For now, we assume that there is a single module. If the program has more than one module, it needs
                // to be flattened before the procedure.
                STORM_LOG_THROW(program.getNumberOfModules() == 1, storm::exceptions::WrongFormatException, "Cannot create abstract program from program containing too many modules.");
                
                uint_fast64_t totalNumberOfCommands = 0;
                for (auto const& module : program.getModules()) {
                    // If we were requested to add all guards to the set of predicates, we do so now.
                    if (addAllGuards) {
                        for (auto const& command : module.getCommands()) {
                            predicates.push_back(command.getGuardExpression());
                        }
                    }
                    
                    totalNumberOfCommands += module.getNumberOfCommands();
                }
                
                // Create DD variables for all predicates.
                for (auto const& predicate : predicates) {
                    std::stringstream stream;
                    stream << predicate;
                    predicateDdVariables.push_back(ddManager->addMetaVariable(stream.str()));
                }
                
                // Create DD variable for the command encoding.
                commandDdVariable = ddManager->addMetaVariable("command", 0, totalNumberOfCommands - 1);
                
                // Create DD variables encoding the nondeterministic choices of player 2.
                // NOTE: currently we assume that 100 variables suffice, which corresponds to 2^100 possible choices.
                // If for some reason this should not be enough, we could grow this vector dynamically, but odds are
                // that it's impossible to treat such models in any event.
                for (uint_fast64_t index = 0; index < 100; ++index) {
                    optionDdVariables.push_back(ddManager->addMetaVariable("opt" + std::to_string(index)));
                }
                
                // For each module of the concrete program, we create an abstract counterpart.
                for (auto const& module : program.getModules()) {
                    modules.emplace_back(expressionManager, module, predicates, *smtSolverFactory);
                }
            }
            
            // Explicitly instantiate the class.
            template class AbstractProgram<storm::dd::DdType::CUDD, double>;
            
        }
    }
}