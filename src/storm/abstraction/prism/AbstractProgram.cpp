#include "storm/abstraction/prism/AbstractProgram.h"

#include "storm/abstraction/BottomStateResult.h"
#include "storm/abstraction/GameBddResult.h"

#include "storm/storage/BitVector.h"

#include "storm/storage/prism/Program.h"

#include "storm/storage/dd/DdManager.h"
#include "storm/storage/dd/Add.h"

#include "storm/models/symbolic/StandardRewardModel.h"

#include "storm/utility/dd.h"
#include "storm/utility/macros.h"
#include "storm/utility/solver.h"
#include "storm/exceptions/WrongFormatException.h"
#include "storm/exceptions/InvalidArgumentException.h"

#include "storm-config.h"
#include "storm/adapters/CarlAdapter.h"

namespace storm {
    namespace abstraction {
        namespace prism {
            
#undef LOCAL_DEBUG
            
            template <storm::dd::DdType DdType, typename ValueType>
            AbstractProgram<DdType, ValueType>::AbstractProgram(storm::prism::Program const& program,
                                                                std::shared_ptr<storm::utility::solver::SmtSolverFactory> const& smtSolverFactory,
                                                                bool addAllGuards)
            : program(program), smtSolverFactory(smtSolverFactory), abstractionInformation(program.getManager()), modules(), initialStateAbstractor(abstractionInformation, program.getAllExpressionVariables(), {program.getInitialStatesExpression()}, this->smtSolverFactory), addedAllGuards(addAllGuards), currentGame(nullptr), refinementPerformed(false) {
                
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
                
                // For each module of the concrete program, we create an abstract counterpart.
                for (auto const& module : program.getModules()) {
                    this->modules.emplace_back(module, abstractionInformation, this->smtSolverFactory, addAllGuards);
                }
                
                // Retrieve the command-update probability ADD, so we can multiply it with the abstraction BDD later.
                commandUpdateProbabilitiesAdd = modules.front().getCommandUpdateProbabilitiesAdd();
                
                // Now that we have created all other DD variables, we create the DD variables for the predicates.
                std::vector<storm::expressions::Expression> initialPredicates;
                if (addAllGuards) {
                    for (auto const& guard : allGuards) {
                        initialPredicates.push_back(guard);
                    }
                }
                
                // Finally, refine using the all predicates and build game as a by-product.
                this->refine(initialPredicates);
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            void AbstractProgram<DdType, ValueType>::refine(std::vector<storm::expressions::Expression> const& predicates) {
                // Add the predicates to the global list of predicates and gather their indices.
                std::vector<uint_fast64_t> newPredicateIndices;
                for (auto const& predicate : predicates) {
                    STORM_LOG_THROW(predicate.hasBooleanType(), storm::exceptions::InvalidArgumentException, "Expecting a predicate of type bool.");
                    newPredicateIndices.push_back(abstractionInformation.addPredicate(predicate));
                }
                
                // Refine all abstract modules.
                for (auto& module : modules) {
                    module.refine(newPredicateIndices);
                }
                
                // Refine initial state abstractor.
                initialStateAbstractor.refine(newPredicateIndices);
                
                // Update the flag that stores whether a refinement was performed.
                refinementPerformed = refinementPerformed || !newPredicateIndices.empty();
            }
                                    
            template <storm::dd::DdType DdType, typename ValueType>
            MenuGame<DdType, ValueType> AbstractProgram<DdType, ValueType>::abstract() {
                if (refinementPerformed) {
                    currentGame = buildGame();
                    refinementPerformed = true;
                }
                return *currentGame;
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            AbstractionInformation<DdType> const& AbstractProgram<DdType, ValueType>::getAbstractionInformation() const {
                return abstractionInformation;
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            storm::expressions::Expression const& AbstractProgram<DdType, ValueType>::getGuard(uint64_t player1Choice) const {
                return modules.front().getGuard(player1Choice);
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            std::map<storm::expressions::Variable, storm::expressions::Expression> AbstractProgram<DdType, ValueType>::getVariableUpdates(uint64_t player1Choice, uint64_t auxiliaryChoice) const {
                return modules.front().getVariableUpdates(player1Choice, auxiliaryChoice);
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
                storm::dd::Add<DdType, ValueType> transitionMatrix = (game.bdd && reachableStates).template toAdd<ValueType>();
                transitionMatrix *= commandUpdateProbabilitiesAdd;
                transitionMatrix += deadlockTransitions;
                
                // Extend the current game information with the 'non-bottom' tag before potentially adding bottom state transitions.
                transitionMatrix *= (abstractionInformation.getBottomStateBdd(true, true) && abstractionInformation.getBottomStateBdd(false, true)).template toAdd<ValueType>();
                reachableStates &= abstractionInformation.getBottomStateBdd(true, true);
                initialStates &= abstractionInformation.getBottomStateBdd(true, true);
                
                // If there are bottom transitions, exnted the transition matrix and reachable states now.
                if (hasBottomStates) {
                    transitionMatrix += bottomStateResult.transitions.template toAdd<ValueType>();
                    reachableStates |= bottomStateResult.states;
                }
                
                std::set<storm::expressions::Variable> usedPlayer2Variables(abstractionInformation.getPlayer2Variables().begin(), abstractionInformation.getPlayer2Variables().begin() + game.numberOfPlayer2Variables);
                
                std::set<storm::expressions::Variable> allNondeterminismVariables = usedPlayer2Variables;
                allNondeterminismVariables.insert(abstractionInformation.getPlayer1Variables().begin(), abstractionInformation.getPlayer1Variables().end());
                
                std::set<storm::expressions::Variable> allSourceVariables(abstractionInformation.getSourceVariables());
                allSourceVariables.insert(abstractionInformation.getBottomStateVariable(true));
                std::set<storm::expressions::Variable> allSuccessorVariables(abstractionInformation.getSuccessorVariables());
                allSuccessorVariables.insert(abstractionInformation.getBottomStateVariable(false));
                
                return std::make_unique<MenuGame<DdType, ValueType>>(abstractionInformation.getDdManagerAsSharedPointer(), reachableStates, initialStates, abstractionInformation.getDdManager().getBddZero(), transitionMatrix, bottomStateResult.states, allSourceVariables, allSuccessorVariables, abstractionInformation.getExtendedSourceSuccessorVariablePairs(), std::set<storm::expressions::Variable>(abstractionInformation.getPlayer1Variables().begin(), abstractionInformation.getPlayer1Variables().end()), usedPlayer2Variables, allNondeterminismVariables, auxVariables, abstractionInformation.getPredicateToBddMap());
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            void AbstractProgram<DdType, ValueType>::exportToDot(std::string const& filename, storm::dd::Bdd<DdType> const& highlightStatesBdd, storm::dd::Bdd<DdType> const& filter) const {
                std::ofstream out(filename);
                
                storm::dd::Add<DdType, ValueType> filteredTransitions = filter.template toAdd<ValueType>() * currentGame->getTransitionMatrix();
                storm::dd::Bdd<DdType> filteredTransitionsBdd = filteredTransitions.toBdd().existsAbstract(currentGame->getNondeterminismVariables());
                storm::dd::Bdd<DdType> filteredReachableStates = storm::utility::dd::computeReachableStates(currentGame->getInitialStates(), filteredTransitionsBdd, currentGame->getRowVariables(), currentGame->getColumnVariables());
                filteredTransitions *= filteredReachableStates.template toAdd<ValueType>();
                
                // Determine all initial states so we can color them blue.
                std::unordered_set<std::string> initialStates;
                storm::dd::Add<DdType, ValueType> initialStatesAsAdd = currentGame->getInitialStates().template toAdd<ValueType>();
                for (auto stateValue : initialStatesAsAdd) {
                    std::stringstream stateName;
                    for (auto const& var : currentGame->getRowVariables()) {
                        if (stateValue.first.getBooleanValue(var)) {
                            stateName << "1";
                        } else {
                            stateName << "0";
                        }
                    }
                    initialStates.insert(stateName.str());
                }
                
                // Determine all highlight states so we can color them red.
                std::unordered_set<std::string> highlightStates;
                storm::dd::Add<DdType, ValueType> highlightStatesAdd = highlightStatesBdd.template toAdd<ValueType>();
                for (auto stateValue : highlightStatesAdd) {
                    std::stringstream stateName;
                    for (auto const& var : currentGame->getRowVariables()) {
                        if (stateValue.first.getBooleanValue(var)) {
                            stateName << "1";
                        } else {
                            stateName << "0";
                        }
                    }
                    highlightStates.insert(stateName.str());
                }
                
                out << "digraph game {" << std::endl;
                
                // Create the player 1 nodes.
                storm::dd::Add<DdType, ValueType> statesAsAdd = filteredReachableStates.template toAdd<ValueType>();
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
                    std::string stateNameAsString = stateName.str();
                    out << stateNameAsString;
                    out << " [ label=\"";
                    if (stateValue.first.getBooleanValue(abstractionInformation.getBottomStateVariable(true))) {
                        out << "*\", margin=0, width=0, height=0, shape=\"none\"";
                    } else {
                        out << stateName.str() << "\", margin=0, width=0, height=0, shape=\"oval\"";
                    }
                    bool isInitial = initialStates.find(stateNameAsString) != initialStates.end();
                    bool isHighlight = highlightStates.find(stateNameAsString) != highlightStates.end();
                    if (isInitial && isHighlight) {
                        out << ", style=\"filled\", fillcolor=\"yellow\"";
                    } else if (isInitial) {
                        out << ", style=\"filled\", fillcolor=\"blue\"";
                    } else if (isHighlight) {
                        out << ", style=\"filled\", fillcolor=\"red\"";
                    }
                    out << " ];" << std::endl;
                }
                
                // Create the nodes of the second player.
                storm::dd::Add<DdType, ValueType> player2States = filteredTransitions.toBdd().existsAbstract(currentGame->getColumnVariables()).existsAbstract(currentGame->getPlayer2Variables()).template toAdd<ValueType>();
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
                storm::dd::Add<DdType, ValueType> playerPStates = filteredTransitions.toBdd().existsAbstract(currentGame->getColumnVariables()).template toAdd<ValueType>();
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
                
                for (auto stateValue : filteredTransitions) {
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
#ifdef STORM_HAVE_CARL
            template class AbstractProgram<storm::dd::DdType::Sylvan, storm::RationalFunction>;
#endif
        }
    }
}
