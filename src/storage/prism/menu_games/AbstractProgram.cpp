#include "src/storage/prism/menu_games/AbstractProgram.h"

#include "src/storage/prism/Program.h"

#include "src/storage/dd/CuddDdManager.h"
#include "src/storage/dd/CuddAdd.h"

#include "src/utility/macros.h"
#include "src/utility/solver.h"
#include "src/exceptions/WrongFormatException.h"
#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace prism {
        namespace menu_games {
            
            template <storm::dd::DdType DdType, typename ValueType>
            AbstractProgram<DdType, ValueType>::AbstractProgram(storm::expressions::ExpressionManager& expressionManager, storm::prism::Program const& program, std::vector<storm::expressions::Expression> const& initialPredicates, std::unique_ptr<storm::utility::solver::SmtSolverFactory>&& smtSolverFactory, bool addAllGuards) : smtSolverFactory(std::move(smtSolverFactory)), ddInformation(std::make_shared<storm::dd::DdManager<DdType>>()), expressionInformation(expressionManager, initialPredicates, program.getAllExpressionVariables(), program.getAllRangeExpressions()), modules(), program(program), initialStateAbstractor(expressionInformation, ddInformation, *this->smtSolverFactory), lastAbstractBdd(ddInformation.manager->getBddZero()), lastAbstractAdd(ddInformation.manager->getAddZero()), lastReachableStates(ddInformation.manager->getBddZero()) {
                
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
                    ddInformation.optionDdVariables.push_back(std::make_pair(newOptionVar, ddInformation.manager->getIdentity(newOptionVar).toBdd()));
                }
                
                // Create DD variables for all predicates.
                for (auto const& predicate : expressionInformation.predicates) {
                    ddInformation.addPredicate(predicate);
                }
                
                // For each module of the concrete program, we create an abstract counterpart.
                for (auto const& module : program.getModules()) {
                    modules.emplace_back(module, expressionInformation, ddInformation, *this->smtSolverFactory);
                }
                
                // Add the initial state expression to the initial state abstractor.
                initialStateAbstractor.addPredicate(program.getInitialConstruct().getInitialStatesExpression());
                
                // Finally, retrieve the command-update probability ADD, so we can multiply it with the abstraction BDD later.
                commandUpdateProbabilitiesAdd = modules.front().getCommandUpdateProbabilitiesAdd();
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            void AbstractProgram<DdType, ValueType>::refine(std::vector<storm::expressions::Expression> const& predicates) {
                std::cout << " >>>> refine <<<<<<" << std::endl;
                
                // Add the predicates to the global list of predicates.
                uint_fast64_t firstNewPredicateIndex = expressionInformation.predicates.size();
                expressionInformation.predicates.insert(expressionInformation.predicates.end(), predicates.begin(), predicates.end());
                
                // Create DD variables and some auxiliary data structures for the new predicates.
                for (auto const& predicate : predicates) {
                    STORM_LOG_THROW(predicate.hasBooleanType(), storm::exceptions::InvalidArgumentException, "Expecting a predicate of type bool.");
                    ddInformation.addPredicate(predicate);
                }

                // Create a list of indices of the predicates, so we can refine the abstract modules and the state set abstractors.
                std::vector<uint_fast64_t> newPredicateIndices;
                for (uint_fast64_t index = firstNewPredicateIndex; index < expressionInformation.predicates.size(); ++index) {
                    newPredicateIndices.push_back(index);
                }
                
                // Refine all abstract modules.
                for (auto& module : modules) {
                    module.refine(newPredicateIndices);
                }
                
                // Refine initial state abstractor.
                initialStateAbstractor.refine(newPredicateIndices);
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            storm::dd::Add<DdType> AbstractProgram<DdType, ValueType>::getAbstractAdd() {
                // As long as there is only one module, we only build its game representation.
                std::pair<storm::dd::Bdd<DdType>, uint_fast64_t> gameBdd = modules.front().getAbstractBdd();
                
                // If the abstraction did not change, we can return the most recenty obtained ADD.
                if (gameBdd.first == lastAbstractBdd) {
                    return lastAbstractAdd;
                }
                
                // Otherwise, we remember that the abstract BDD changed and perform a reachability analysis.
                lastAbstractBdd = gameBdd.first;

                // Construct a set of all unnecessary variables, so we can abstract from it.
                std::set<storm::expressions::Variable> variablesToAbstract = {ddInformation.commandDdVariable, ddInformation.updateDdVariable};
                for (uint_fast64_t index = 0; index < gameBdd.second; ++index) {
                    variablesToAbstract.insert(ddInformation.optionDdVariables[index].first);
                }
                
                // Do a reachability analysis on the raw transition relation.
                storm::dd::Bdd<DdType> transitionRelation = lastAbstractBdd.existsAbstract(variablesToAbstract);
                lastReachableStates = this->getReachableStates(initialStateAbstractor.getAbstractStates(), transitionRelation);

                // Find the deadlock states in the model.
                storm::dd::Bdd<DdType> deadlockStates = transitionRelation.existsAbstract(ddInformation.successorVariables);
                deadlockStates = lastReachableStates && !deadlockStates;
                
                // If there are deadlock states, we fix them now.
                storm::dd::Add<DdType> deadlockTransitions = ddInformation.manager->getAddZero();
                if (!deadlockStates.isZero()) {
                    deadlockTransitions = (deadlockStates && ddInformation.allPredicateIdentities && ddInformation.manager->getEncoding(ddInformation.commandDdVariable, 0)  && ddInformation.manager->getEncoding(ddInformation.updateDdVariable, 0) && ddInformation.getMissingOptionVariableCube(0, gameBdd.second)).toAdd();
                }
                
                // Construct the final game by cutting away the transitions of unreachable states.
                lastAbstractAdd = (lastAbstractBdd && lastReachableStates).toAdd() * commandUpdateProbabilitiesAdd + deadlockTransitions;
                return lastAbstractAdd;
            }
            
            
            template <storm::dd::DdType DdType, typename ValueType>
            storm::dd::Bdd<DdType> AbstractProgram<DdType, ValueType>::getReachableStates() {
                return lastReachableStates;
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            storm::dd::Bdd<DdType> AbstractProgram<DdType, ValueType>::getReachableStates(storm::dd::Bdd<DdType> const& initialStates, storm::dd::Bdd<DdType> const& transitionRelation) {
                storm::dd::Bdd<storm::dd::DdType::CUDD> frontier = initialStates;

                storm::dd::Bdd<storm::dd::DdType::CUDD> reachableStates = initialStates;
                uint_fast64_t reachabilityIteration = 0;
                while (!frontier.isZero()) {
                    ++reachabilityIteration;
                    frontier = frontier.andExists(transitionRelation, ddInformation.sourceVariables);
                    frontier = frontier.swapVariables(ddInformation.predicateDdVariables);
                    frontier &= !reachableStates;
                    reachableStates |= frontier;
                    STORM_LOG_TRACE("Iteration " << reachabilityIteration << " of reachability analysis.");
                }
                
                return reachableStates;
            }
            
            // Explicitly instantiate the class.
            template class AbstractProgram<storm::dd::DdType::CUDD, double>;
            
        }
    }
}