#include "src/abstraction/prism/AbstractProgram.h"

#include "src/abstraction/BottomStateResult.h"

#include "src/storage/prism/Program.h"

#include "src/storage/dd/DdManager.h"
#include "src/storage/dd/Add.h"

#include "src/models/symbolic/StandardRewardModel.h"

#include "src/utility/dd.h"
#include "src/utility/macros.h"
#include "src/utility/solver.h"
#include "src/exceptions/WrongFormatException.h"
#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace abstraction {
        namespace prism {
            
            template <storm::dd::DdType DdType, typename ValueType>
            AbstractProgram<DdType, ValueType>::AbstractProgram(storm::prism::Program const& program,
                                                                std::vector<storm::expressions::Expression> const& initialPredicates,
                                                                std::shared_ptr<storm::utility::solver::SmtSolverFactory> const& smtSolverFactory,
                                                                bool addAllGuards)
            : program(program), smtSolverFactory(smtSolverFactory), abstractionInformation(program.getManager()), modules(), initialStateAbstractor(abstractionInformation, program.getAllExpressionVariables(), {program.getInitialConstruct().getInitialStatesExpression()}, this->smtSolverFactory), addedAllGuards(addAllGuards), currentGame(nullptr) {
                
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
                    this->modules.emplace_back(module, abstractionInformation, this->smtSolverFactory, addAllGuards);
                }
                
                // Refine the initial state abstractors using the initial predicates.
                initialStateAbstractor.refine(allPredicateIndices);
                
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
                                
                // Finally, we rebuild the game.
                currentGame = buildGame();
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            void AbstractProgram<DdType, ValueType>::refine(storm::dd::Bdd<DdType> const& pivotState, storm::dd::Bdd<DdType> const& player1Choice, storm::dd::Bdd<DdType> const& lowerChoice, storm::dd::Bdd<DdType> const& upperChoice) {
//                std::cout << "refining in program!" << std::endl;
//                lowerChoice.template toAdd<ValueType>().exportToDot("lchoice.dot");
//                upperChoice.template toAdd<ValueType>().exportToDot("uchoice.dot");
//                player1Choice.template toAdd<ValueType>().exportToDot("pl1_choice.dot");
//                pivotState.template toAdd<ValueType>().exportToDot("pivotstate.dot");
                
                // Decode the index of the command chosen by player 1.
                storm::dd::Add<DdType, ValueType> player1ChoiceAsAdd = player1Choice.template toAdd<ValueType>();
                auto pl1It = player1ChoiceAsAdd.begin();
                uint_fast64_t commandIndex = abstractionInformation.decodePlayer1Choice((*pl1It).first, abstractionInformation.getPlayer1VariableCount());
                
                bool bottomSuccessor = !((abstractionInformation.getBottomStateBdd(false, false) && lowerChoice) || (abstractionInformation.getBottomStateBdd(false, false) && upperChoice)).isZero();
                if (bottomSuccessor) {
                    storm::abstraction::prism::AbstractCommand<DdType, ValueType>& abstractCommand = modules.front().getCommands()[commandIndex];
                    abstractCommand.notifyGuardIsPredicate();
                    storm::expressions::Expression newPredicate = abstractCommand.getConcreteCommand().getGuardExpression();
                    STORM_LOG_DEBUG("Derived new predicate: " << newPredicate);
                    this->refine({newPredicate});
                } else {
                    exit(-1);
                }
                
                storm::dd::Add<DdType, ValueType> lowerChoiceAsAdd = lowerChoice.template toAdd<ValueType>();
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
                GameBddResult<DdType> game = modules.front().getAbstractBdd();

                // Construct a set of all unnecessary variables, so we can abstract from it.
                std::set<storm::expressions::Variable> variablesToAbstract(abstractionInformation.getPlayer1VariableSet(abstractionInformation.getPlayer1VariableCount()));
                auto player2Variables = abstractionInformation.getPlayer2VariableSet(game.numberOfPlayer2Variables);
                variablesToAbstract.insert(player2Variables.begin(), player2Variables.end());
                auto auxVariables = abstractionInformation.getAuxVariableSet(0, abstractionInformation.getAuxVariableCount());
                variablesToAbstract.insert(auxVariables.begin(), auxVariables.end());
                
                // Do a reachability analysis on the raw transition relation.
                storm::dd::Bdd<DdType> transitionRelation = game.bdd.existsAbstract(variablesToAbstract);
                storm::dd::Bdd<DdType> initialStates = initialStateAbstractor.getAbstractStates();
                storm::dd::Bdd<DdType> reachableStates = storm::utility::dd::computeReachableStates(initialStates, transitionRelation, abstractionInformation.getSourceVariables(), abstractionInformation.getSuccessorVariables());
                
                // Find the deadlock states in the model. Note that this does not find the 'deadlocks' in bottom states,
                // as the bottom states are not contained in the reachable states.
                storm::dd::Bdd<DdType> deadlockStates = transitionRelation.existsAbstract(abstractionInformation.getSuccessorVariables());
                deadlockStates = reachableStates && !deadlockStates;
                
                // If there are deadlock states, we fix them now.
                storm::dd::Add<DdType, ValueType> deadlockTransitions = abstractionInformation.getDdManager().template getAddZero<ValueType>();
                if (!deadlockStates.isZero()) {
                    deadlockTransitions = (deadlockStates && abstractionInformation.getAllPredicateIdentities() && abstractionInformation.encodePlayer1Choice(0, abstractionInformation.getPlayer1VariableCount()) && abstractionInformation.encodePlayer2Choice(0, game.numberOfPlayer2Variables) && abstractionInformation.encodeAux(0, 0, abstractionInformation.getAuxVariableCount())).template toAdd<ValueType>();
                }

                // Compute bottom states and the appropriate transitions if necessary.
                BottomStateResult<DdType> bottomStateResult(abstractionInformation.getDdManager().getBddZero(), abstractionInformation.getDdManager().getBddZero());
                bool hasBottomStates = false;
                if (!addedAllGuards) {
                    bottomStateResult = modules.front().getBottomStateTransitions(reachableStates, game.numberOfPlayer2Variables);
                    hasBottomStates = !bottomStateResult.states.isZero();
                }
                
                // Construct the transition matrix by cutting away the transitions of unreachable states.
                storm::dd::Add<DdType> transitionMatrix = (game.bdd && reachableStates).template toAdd<ValueType>() * commandUpdateProbabilitiesAdd + deadlockTransitions;
                
                // If there are bottom states, we need to mark all other states as non-bottom now.
                if (hasBottomStates) {
                    transitionMatrix *= (abstractionInformation.getBottomStateBdd(true, true) && abstractionInformation.getBottomStateBdd(false, true)).template toAdd<ValueType>();
                    transitionMatrix += bottomStateResult.transitions.template toAdd<ValueType>();
                    reachableStates &= abstractionInformation.getBottomStateBdd(true, true);
                    initialStates &= abstractionInformation.getBottomStateBdd(true, true);
                    reachableStates |= bottomStateResult.states;
                }
            
                std::set<storm::expressions::Variable> usedPlayer2Variables(abstractionInformation.getPlayer2Variables().begin(), abstractionInformation.getPlayer2Variables().begin() + game.numberOfPlayer2Variables);
                
                std::set<storm::expressions::Variable> allNondeterminismVariables = usedPlayer2Variables;
                allNondeterminismVariables.insert(abstractionInformation.getPlayer1Variables().begin(), abstractionInformation.getPlayer1Variables().end());

                std::set<storm::expressions::Variable> allSourceVariables(abstractionInformation.getSourceVariables());
                if (hasBottomStates) {
                    allSourceVariables.insert(abstractionInformation.getBottomStateVariable(true));
                }
                std::set<storm::expressions::Variable> allSuccessorVariables(abstractionInformation.getSuccessorVariables());
                if (hasBottomStates) {
                    allSuccessorVariables.insert(abstractionInformation.getBottomStateVariable(false));
                }
                
                return std::make_unique<MenuGame<DdType, ValueType>>(abstractionInformation.getDdManagerAsSharedPointer(), reachableStates, initialStates, abstractionInformation.getDdManager().getBddZero(), transitionMatrix, bottomStateResult.states, allSourceVariables, allSuccessorVariables, hasBottomStates ? abstractionInformation.getExtendedSourceSuccessorVariablePairs() : abstractionInformation.getSourceSuccessorVariablePairs(), std::set<storm::expressions::Variable>(abstractionInformation.getPlayer1Variables().begin(), abstractionInformation.getPlayer1Variables().end()), usedPlayer2Variables, allNondeterminismVariables, auxVariables, abstractionInformation.getPredicateToBddMap());
            }
                        
            template <storm::dd::DdType DdType, typename ValueType>
            void AbstractProgram<DdType, ValueType>::exportToDot(std::string const& filename) const {
                std::ofstream out(filename);
                out << "digraph game {" << std::endl;
                
                // Create the player 1 nodes.
                storm::dd::Add<DdType, ValueType> statesAsAdd = currentGame->getReachableStates().template toAdd<ValueType>();
                for (auto stateValue : statesAsAdd) {
                    out << "\tpl1_";
                    std::stringstream stateName;
                    for (auto const& var : currentGame->getRowVariables()) {
                        if (stateValue.first.getBooleanValue(var)) {
                            stateName << "1";
                        } else {
                            stateName << "0";
                        }
                    }
                    out << stateName.str();
                    out << " [ label=\"";
                    if (stateValue.first.getBooleanValue(abstractionInformation.getBottomStateVariable(true))) {
                        out << "*\", margin=0, width=0, height=0, shape=\"none";
                    } else {
                        out << stateName.str() << "\", margin=0, width=0, height=0, shape=\"oval";
                    }
                    out << "\" ];" << std::endl;
                }
                
                // Create the nodes of the second player.
                storm::dd::Add<DdType, ValueType> player2States = currentGame->getTransitionMatrix().toBdd().existsAbstract(currentGame->getColumnVariables()).existsAbstract(currentGame->getPlayer2Variables()).template toAdd<ValueType>();
                for (auto stateValue : player2States) {
                    out << "\tpl2_";
                    std::stringstream stateName;
                    for (auto const& var : currentGame->getRowVariables()) {
                        if (stateValue.first.getBooleanValue(var)) {
                            stateName << "1";
                        } else {
                            stateName << "0";
                        }
                    }
                    uint_fast64_t index = abstractionInformation.decodePlayer1Choice(stateValue.first, abstractionInformation.getPlayer1VariableCount());
                    out << stateName.str() << "_" << index;
                    out << " [ shape=\"square\", width=0, height=0, margin=0, label=\"" << index << "\" ];" << std::endl;
                    out << "\tpl1_" << stateName.str() << " -> " << "pl2_" << stateName.str() << "_" << index << " [ label=\"" << index << "\" ];" << std::endl;
                }
                
                // Create the nodes of the probabilistic player.
                storm::dd::Add<DdType, ValueType> playerPStates = currentGame->getTransitionMatrix().toBdd().existsAbstract(currentGame->getColumnVariables()).template toAdd<ValueType>();
                for (auto stateValue : playerPStates) {
                    out << "\tplp_";
                    std::stringstream stateName;
                    for (auto const& var : currentGame->getRowVariables()) {
                        if (stateValue.first.getBooleanValue(var)) {
                            stateName << "1";
                        } else {
                            stateName << "0";
                        }
                    }
                    uint_fast64_t index = abstractionInformation.decodePlayer1Choice(stateValue.first, abstractionInformation.getPlayer1VariableCount());
                    stateName << "_" << index;
                    index = abstractionInformation.decodePlayer2Choice(stateValue.first, currentGame->getPlayer2Variables().size());
                    out << stateName.str() << "_" << index;
                    out << " [ shape=\"point\", label=\"\" ];" << std::endl;
                    out << "\tpl2_" << stateName.str() << " -> " << "plp_" << stateName.str() << "_" << index << " [ label=\"" << index << "\" ];" << std::endl;
                }
                
                for (auto stateValue : currentGame->getTransitionMatrix()) {
                    std::stringstream sourceStateName;
                    std::stringstream successorStateName;
                    for (auto const& var : currentGame->getRowVariables()) {
                        if (stateValue.first.getBooleanValue(var)) {
                            sourceStateName << "1";
                        } else {
                            sourceStateName << "0";
                        }
                    }
                    for (auto const& var : currentGame->getColumnVariables()) {
                        if (stateValue.first.getBooleanValue(var)) {
                            successorStateName << "1";
                        } else {
                            successorStateName << "0";
                        }
                    }
                    uint_fast64_t pl1Index = abstractionInformation.decodePlayer1Choice(stateValue.first, abstractionInformation.getPlayer1VariableCount());
                    uint_fast64_t pl2Index = abstractionInformation.decodePlayer2Choice(stateValue.first, currentGame->getPlayer2Variables().size());
                    out << "\tplp_" << sourceStateName.str() << "_" << pl1Index << "_" << pl2Index << " -> pl1_" << successorStateName.str() << " [ label=\"" << stateValue.second << "\"];" << std::endl;
                }
                
                out << "}" << std::endl;
            }
            
            // Explicitly instantiate the class.
            template class AbstractProgram<storm::dd::DdType::CUDD, double>;
            template class AbstractProgram<storm::dd::DdType::Sylvan, double>;
            
        }
    }
}