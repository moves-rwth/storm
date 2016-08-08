#include "src/abstraction/prism/AbstractProgram.h"

#include "src/storage/prism/Program.h"

#include "src/storage/dd/DdManager.h"
#include "src/storage/dd/Add.h"

#include "src/models/symbolic/StandardRewardModel.h"

#include "src/utility/macros.h"
#include "src/utility/solver.h"
#include "src/exceptions/WrongFormatException.h"
#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace abstraction {
        namespace prism {
            
            template <storm::dd::DdType DdType, typename ValueType>
            AbstractProgram<DdType, ValueType>::AbstractProgram(storm::expressions::ExpressionManager& expressionManager, storm::prism::Program const& program,
                                                                std::vector<storm::expressions::Expression> const& initialPredicates,
                                                                std::unique_ptr<storm::utility::solver::SmtSolverFactory>&& smtSolverFactory,
                                                                bool addAllGuards)
            : program(program), smtSolverFactory(std::move(smtSolverFactory)), abstractionInformation(expressionManager), modules(), initialStateAbstractor(abstractionInformation, program.getAllExpressionVariables(), {program.getInitialConstruct().getInitialStatesExpression()}, *this->smtSolverFactory), addedAllGuards(addAllGuards), bottomStateAbstractor(abstractionInformation, program.getAllExpressionVariables(), program.getAllGuards(true), *this->smtSolverFactory), currentGame(nullptr) {
                
                // For now, we assume that there is a single module. If the program has more than one module, it needs
                // to be flattened before the procedure.
                STORM_LOG_THROW(program.getNumberOfModules() == 1, storm::exceptions::WrongFormatException, "Cannot create abstract program from program containing too many modules.");
                
                // Add all variables and range expressions to the information object.
                for (auto const& variable : this->program.get().getAllExpressionVariables()) {
                    abstractionInformation.addExpressionVariable(variable);
                }
                for (auto const& range : this->program.get().getAllRangeExpressions()) {
                    abstractionInformation.addConstraint(range);
                    initialStateAbstractor.constrain(range);
                    bottomStateAbstractor.constrain(range);
                }
                
                uint_fast64_t totalNumberOfCommands = 0;
                uint_fast64_t maximalUpdateCount = 0;
                std::vector<storm::expressions::Expression> allGuards;
                for (auto const& module : program.getModules()) {
                    // If we were requested to add all guards to the set of predicates, we do so now.
                    for (auto const& command : module.getCommands()) {
                        if (addAllGuards) {
                            allGuards.push_back(command.getGuardExpression());
                        }
                        maximalUpdateCount = std::max(maximalUpdateCount, static_cast<uint_fast64_t>(command.getNumberOfUpdates()));
                    }
                
                    totalNumberOfCommands += module.getNumberOfCommands();
                }
                
                // NOTE: currently we assume that 100 player 2 variables suffice, which corresponds to 2^100 possible
                // choices. If for some reason this should not be enough, we could grow this vector dynamically, but
                // odds are that it's impossible to treat such models in any event.
                abstractionInformation.createEncodingVariables(static_cast<uint_fast64_t>(std::ceil(std::log2(totalNumberOfCommands))), 100, static_cast<uint_fast64_t>(std::ceil(std::log2(maximalUpdateCount))));
                
                // Now that we have created all other DD variables, we create the DD variables for the predicates.
                std::vector<uint_fast64_t> allPredicateIndices;
                if (addAllGuards) {
                    for (auto const& guard : allGuards) {
                        allPredicateIndices.push_back(abstractionInformation.addPredicate(guard));
                    }
                }
                for (auto const& predicate : initialPredicates) {
                    allPredicateIndices.push_back(abstractionInformation.addPredicate(predicate));
                }
                
                // For each module of the concrete program, we create an abstract counterpart.
                for (auto const& module : program.getModules()) {
                    this->modules.emplace_back(module, abstractionInformation, *this->smtSolverFactory);
                }
                
                // Refine the state abstractors using the initial predicates.
                initialStateAbstractor.refine(allPredicateIndices);
                bottomStateAbstractor.refine(allPredicateIndices);
                
                // Retrieve the command-update probability ADD, so we can multiply it with the abstraction BDD later.
                commandUpdateProbabilitiesAdd = modules.front().getCommandUpdateProbabilitiesAdd();
                
                // Finally, we build the game the first time.
                currentGame = buildGame();
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            void AbstractProgram<DdType, ValueType>::refine(std::vector<storm::expressions::Expression> const& predicates) {
                STORM_LOG_THROW(!predicates.empty(), storm::exceptions::InvalidArgumentException, "Cannot refine without predicates.");
                
                // Add the predicates to the global list of predicates.
                std::vector<uint_fast64_t> newPredicateIndices;
                for (auto const& predicate : predicates) {
                    STORM_LOG_THROW(predicate.hasBooleanType(), storm::exceptions::InvalidArgumentException, "Expecting a predicate of type bool.");
                    uint_fast64_t newPredicateIndex = abstractionInformation.addPredicate(predicate);
                    newPredicateIndices.push_back(newPredicateIndex);
                }
                
                // Refine all abstract modules.
                for (auto& module : modules) {
                    module.refine(newPredicateIndices);
                }
                
                // Refine initial state abstractor.
                initialStateAbstractor.refine(newPredicateIndices);
                
                // Refine bottom state abstractor.
                bottomStateAbstractor.refine(newPredicateIndices);
                
                // Finally, we rebuild the game.
                currentGame = buildGame();
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            MenuGame<DdType, ValueType> AbstractProgram<DdType, ValueType>::getAbstractGame() {
                STORM_LOG_ASSERT(currentGame != nullptr, "Game was not properly created.");
                return *currentGame;
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            storm::dd::Bdd<DdType> AbstractProgram<DdType, ValueType>::getStates(storm::expressions::Expression const& predicate) {
                STORM_LOG_ASSERT(currentGame != nullptr, "Game was not properly created.");
                return abstractionInformation.getPredicateSourceVariable(predicate);
            }
                        
            template <storm::dd::DdType DdType, typename ValueType>
            std::unique_ptr<MenuGame<DdType, ValueType>> AbstractProgram<DdType, ValueType>::buildGame() {
                // As long as there is only one module, we only build its game representation.
                std::pair<storm::dd::Bdd<DdType>, uint_fast64_t> gameBdd = modules.front().getAbstractBdd();

                // Construct a set of all unnecessary variables, so we can abstract from it.
                std::set<storm::expressions::Variable> variablesToAbstract(abstractionInformation.getPlayer1VariableSet(abstractionInformation.getPlayer1VariableCount()));
                auto player2Variables = abstractionInformation.getPlayer2VariableSet(gameBdd.second);
                variablesToAbstract.insert(player2Variables.begin(), player2Variables.end());
                auto probBranchingVariables = abstractionInformation.getProbabilisticBranchingVariableSet(abstractionInformation.getProbabilisticBranchingVariableCount());
                variablesToAbstract.insert(probBranchingVariables.begin(), probBranchingVariables.end());
                
                // Do a reachability analysis on the raw transition relation.
                storm::dd::Bdd<DdType> transitionRelation = gameBdd.first.existsAbstract(variablesToAbstract);
                storm::dd::Bdd<DdType> initialStates = initialStateAbstractor.getAbstractStates();
                storm::dd::Bdd<DdType> reachableStates = this->getReachableStates(initialStates, transitionRelation);
                
                // Determine the bottom states.
                storm::dd::Bdd<DdType> bottomStates;
                if (addedAllGuards) {
                    bottomStates = abstractionInformation.getDdManager().getBddZero();
                } else {
                    bottomStateAbstractor.constrain(reachableStates);
                    bottomStates = bottomStateAbstractor.getAbstractStates();
                }
                
                // Find the deadlock states in the model.
                storm::dd::Bdd<DdType> deadlockStates = transitionRelation.existsAbstract(abstractionInformation.getSuccessorVariables());
                deadlockStates = reachableStates && !deadlockStates;
                
                // If there are deadlock states, we fix them now.
                storm::dd::Add<DdType, ValueType> deadlockTransitions = abstractionInformation.getDdManager().template getAddZero<ValueType>();
                if (!deadlockStates.isZero()) {
                    deadlockTransitions = (deadlockStates && abstractionInformation.getAllPredicateIdentities() && abstractionInformation.encodePlayer1Choice(0, abstractionInformation.getPlayer1VariableCount()) && abstractionInformation.encodePlayer2Choice(0, gameBdd.second) && abstractionInformation.encodeProbabilisticChoice(0, abstractionInformation.getProbabilisticBranchingVariableCount())).template toAdd<ValueType>();
                }

                // Construct the transition matrix by cutting away the transitions of unreachable states.
                storm::dd::Add<DdType> transitionMatrix = (gameBdd.first && reachableStates).template toAdd<ValueType>() * commandUpdateProbabilitiesAdd + deadlockTransitions;
            
                std::set<storm::expressions::Variable> usedPlayer2Variables(abstractionInformation.getPlayer2Variables().begin(), abstractionInformation.getPlayer2Variables().begin() + gameBdd.second);
                
                std::set<storm::expressions::Variable> allNondeterminismVariables = usedPlayer2Variables;
                allNondeterminismVariables.insert(abstractionInformation.getPlayer1Variables().begin(), abstractionInformation.getPlayer1Variables().end());

                return std::make_unique<MenuGame<DdType, ValueType>>(abstractionInformation.getDdManagerAsSharedPointer(), reachableStates, initialStates, abstractionInformation.getDdManager().getBddZero(), transitionMatrix, bottomStates, abstractionInformation.getSourceVariables(), abstractionInformation.getSuccessorVariables(), abstractionInformation.getSourceSuccessorVariablePairs(), std::set<storm::expressions::Variable>(abstractionInformation.getPlayer1Variables().begin(), abstractionInformation.getPlayer1Variables().end()), usedPlayer2Variables, allNondeterminismVariables, std::set<storm::expressions::Variable>(abstractionInformation.getProbabilisticBranchingVariables().begin(), abstractionInformation.getProbabilisticBranchingVariables().end()), abstractionInformation.getPredicateToBddMap());
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            storm::dd::Bdd<DdType> AbstractProgram<DdType, ValueType>::getReachableStates(storm::dd::Bdd<DdType> const& initialStates, storm::dd::Bdd<DdType> const& transitionRelation) {
                storm::dd::Bdd<DdType> frontier = initialStates;

                storm::dd::Bdd<DdType> reachableStates = initialStates;
                uint_fast64_t reachabilityIteration = 0;
                while (!frontier.isZero()) {
                    ++reachabilityIteration;
                    frontier = frontier.andExists(transitionRelation, abstractionInformation.getSourceVariables());
                    frontier = frontier.swapVariables(abstractionInformation.getSourceSuccessorVariablePairs());
                    frontier &= !reachableStates;
                    reachableStates |= frontier;
                    STORM_LOG_TRACE("Iteration " << reachabilityIteration << " of reachability analysis.");
                }
                
                return reachableStates;
            }
            
            // Explicitly instantiate the class.
            template class AbstractProgram<storm::dd::DdType::CUDD, double>;
            template class AbstractProgram<storm::dd::DdType::Sylvan, double>;
            
        }
    }
}