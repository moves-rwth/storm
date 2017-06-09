#ifndef STORM_COUNTEREXAMPLES_SMTMINIMALCOMMANDSETGENERATOR_MDP_H_
#define STORM_COUNTEREXAMPLES_SMTMINIMALCOMMANDSETGENERATOR_MDP_H_

#include <queue>
#include <chrono>

#include "storm/solver/Z3SmtSolver.h"

#include "storm/counterexamples/PrismHighLevelCounterexample.h"

#include "storm/storage/prism/Program.h"
#include "storm/storage/expressions/Expression.h"
#include "storm/storage/sparse/PrismChoiceOrigins.h"
#include "storm/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "storm/modelchecker/prctl/helper/SparseMdpPrctlHelper.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/CoreSettings.h"

#include "storm/utility/counterexamples.h"
#include "storm/utility/cli.h"

namespace storm {
    namespace counterexamples {
        
        /*!
         * This class provides functionality to generate a minimal counterexample to a probabilistic reachability
         * property in terms of used commands.
         */
        template <class T>
        class SMTMinimalCommandSetGenerator {
#ifdef STORM_HAVE_Z3
        private:
            struct RelevancyInformation {
                // The set of relevant states in the model.
                storm::storage::BitVector relevantStates;
                
                // The set of relevant commands.
                boost::container::flat_set<uint_fast64_t> relevantCommands;
                
                // A set of commands that is definitely known to be taken in the final solution.
                boost::container::flat_set<uint_fast64_t> knownCommands;
                
                // A list of relevant choices for each relevant state.
                std::map<uint_fast64_t, std::list<uint_fast64_t>> relevantChoicesForRelevantStates;
            };
            
            struct VariableInformation {
                // The manager responsible for the constraints we are building.
                std::shared_ptr<storm::expressions::ExpressionManager> manager;
                
                // The variables associated with the relevant commands.
                std::vector<storm::expressions::Variable> commandVariables;
                
                // A mapping from relevant commands to their indices in the variable vector.
                std::map<uint_fast64_t, uint_fast64_t> commandToIndexMap;

                // A set of original auxiliary variables needed for the Fu-Malik procedure.
                std::vector<storm::expressions::Variable> originalAuxiliaryVariables;
                
                // A set of auxiliary variables that may be modified by the MaxSAT procedure.
                std::vector<storm::expressions::Variable> auxiliaryVariables;
                
                // A vector of variables that can be used to constrain the number of variables that are set to true.
                std::vector<storm::expressions::Variable> adderVariables;
                
                // A flag whether or not there are variables reserved for encoding reachability of a target state.
                bool hasReachabilityVariables;
                
                // Starting from here, all structures hold information only if hasReachabilityVariables is true;
                
                // A mapping from each pair of adjacent relevant states to their index in the corresponding variable vector.
                std::map<std::pair<uint_fast64_t, uint_fast64_t>, uint_fast64_t> statePairToIndexMap;
                
                // A vector of variables associated with each pair of relevant states (s, s') such that s' is
                // a successor of s.
                std::vector<storm::expressions::Variable> statePairVariables;
                
                // A mapping from relevant states to the index with the corresponding order variable in the state order variable vector.
                std::map<uint_fast64_t, uint_fast64_t> relevantStatesToOrderVariableIndexMap;
                
                // A vector of variables that holds all state order variables.
                std::vector<storm::expressions::Variable> stateOrderVariables;
            };
            
            /*!
             * Computes the set of relevant commands in the model. Relevant commands are choice origins such that there exists
             * a scheduler that satisfies phi until psi with a nonzero probability.
             *
             * @param mdp The MDP to search for relevant commands.
             * @param phiStates A bit vector representing all states that satisfy phi.
             * @param psiStates A bit vector representing all states that satisfy psi.
             * @return A structure containing the relevant commands as well as states.
             */
            static RelevancyInformation determineRelevantStatesAndCommands(storm::models::sparse::Mdp<T> const& mdp, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates) {

                // Create result.
                RelevancyInformation relevancyInformation;
                
                // Compute all relevant states, i.e. states for which there exists a scheduler that has a non-zero
                // probabilitiy of satisfying phi until psi.
                storm::storage::SparseMatrix<T> backwardTransitions = mdp.getBackwardTransitions();
                relevancyInformation.relevantStates = storm::utility::graph::performProbGreater0E(backwardTransitions, phiStates, psiStates);
                relevancyInformation.relevantStates &= ~psiStates;

                STORM_LOG_DEBUG("Found " << relevancyInformation.relevantStates.getNumberOfSetBits() << " relevant states.");
                STORM_LOG_DEBUG(relevancyInformation.relevantStates);

                // Retrieve some references for convenient access.
                storm::storage::SparseMatrix<T> const& transitionMatrix = mdp.getTransitionMatrix();
                std::vector<uint_fast64_t> const& nondeterministicChoiceIndices = mdp.getNondeterministicChoiceIndices();
                storm::storage::sparse::PrismChoiceOrigins const& choiceOrigins = mdp.getChoiceOrigins()->asPrismChoiceOrigins();

                // Now traverse all choices of all relevant states and check whether there is a successor target state.
                // If so, the associated commands become relevant. Also, if a choice of relevant state has at least one
                // relevant successor, the choice becomes relevant.
                for (auto state : relevancyInformation.relevantStates) {
                    relevancyInformation.relevantChoicesForRelevantStates.emplace(state, std::list<uint_fast64_t>());
                    
                    for (uint_fast64_t row = nondeterministicChoiceIndices[state]; row < nondeterministicChoiceIndices[state + 1]; ++row) {
                        bool currentChoiceRelevant = false;

                        for (auto const& entry : transitionMatrix.getRow(row)) {
                            // If there is a relevant successor, we need to add the commands of the current choice.
                            if (relevancyInformation.relevantStates.get(entry.getColumn()) || psiStates.get(entry.getColumn())) {
                                for (auto const& command : choiceOrigins.getCommandSet(row)) {
                                    relevancyInformation.relevantCommands.insert(command);
                                }
                                if (!currentChoiceRelevant) {
                                    currentChoiceRelevant = true;
                                    relevancyInformation.relevantChoicesForRelevantStates[state].push_back(row);
                                }
                            }
                        }
                    }
                }
                
                // Compute the set of commands that are known to be taken in any case.
                relevancyInformation.knownCommands = storm::utility::counterexamples::getGuaranteedCommandSet(mdp, psiStates, relevancyInformation.relevantCommands);
                if (!relevancyInformation.knownCommands.empty()) {
                    boost::container::flat_set<uint_fast64_t> remainingCommands;
                    std::set_difference(relevancyInformation.relevantCommands.begin(), relevancyInformation.relevantCommands.end(), relevancyInformation.knownCommands.begin(), relevancyInformation.knownCommands.end(), std::inserter(remainingCommands, remainingCommands.end()));
                    relevancyInformation.relevantCommands = remainingCommands;
                }
                
                std::cout << "Found " << relevancyInformation.relevantCommands.size() << " relevant and " << relevancyInformation.knownCommands.size() << " known commands." << std::endl;

                STORM_LOG_DEBUG("Found " << relevancyInformation.relevantCommands.size() << " relevant and " << relevancyInformation.knownCommands.size() << " known commands.");
                return relevancyInformation;
            }
            
            /*!
             * Creates all necessary variables.
             *
             * @param manager The manager in which to create the variables.
             * @param relevantCommands A set of relevant commands for which to create the expressions.
             * @return A mapping from relevant commands to their corresponding expressions.
             */
            static VariableInformation createVariables(std::shared_ptr<storm::expressions::ExpressionManager> const& manager, storm::models::sparse::Mdp<T> const& mdp, storm::storage::BitVector const& psiStates, RelevancyInformation const& relevancyInformation, bool createReachabilityVariables) {
                VariableInformation variableInformation;
                variableInformation.manager = manager;
                
                // Create stringstream to build expression names.
                std::stringstream variableName;
                
                // Create the variables for the relevant commands.
                for (auto command : relevancyInformation.relevantCommands) {
                    variableInformation.commandToIndexMap[command] = variableInformation.commandVariables.size();
                    
                    // Clear contents of the stream to construct new expression name.
                    variableName.clear();
                    variableName.str("");
                    variableName << "c" << command;
                    
                    variableInformation.commandVariables.push_back(manager->declareBooleanVariable(variableName.str()));
                    
                    // Clear contents of the stream to construct new expression name.
                    variableName.clear();
                    variableName.str("");
                    variableName << "h" << command;
                    
                    variableInformation.originalAuxiliaryVariables.push_back(manager->declareBooleanVariable(variableName.str()));
                }
                
                // A mapping from each pair of adjacent relevant states to their index in the corresponding variable vector.
                std::map<std::pair<uint_fast64_t, uint_fast64_t>, uint_fast64_t> statePairToIndexMap;
                
                // A vector of variables associated with each pair of relevant states (s, s') such that s' is
                // a successor of s.
                std::vector<storm::expressions::Variable> statePairVariables;
                
                // Create variables needed for encoding reachability of a target state if requested.
                if (createReachabilityVariables) {
                    variableInformation.hasReachabilityVariables = true;
                    
                    storm::storage::SparseMatrix<T> const& transitionMatrix = mdp.getTransitionMatrix();
                    
                    for (auto state : relevancyInformation.relevantStates) {
                        variableInformation.relevantStatesToOrderVariableIndexMap[state] = variableInformation.stateOrderVariables.size();
                        
                        // Clear contents of the stream to construct new expression name.
                        variableName.clear();
                        variableName.str("");
                        variableName << "o" << state;
                        
                        variableInformation.stateOrderVariables.push_back(manager->declareRationalVariable(variableName.str()));
                        
                        for (auto const& relevantChoice : relevancyInformation.relevantChoicesForRelevantStates.at(state)) {
                            for (auto const& entry : transitionMatrix.getRow(relevantChoice)) {
                                
                                // If the successor state is neither the state itself nor an irrelevant state, we need to add a variable for the transition.
                                if (state != entry.getColumn() && (relevancyInformation.relevantStates.get(entry.getColumn()) || psiStates.get(entry.getColumn()))) {
                                    
                                    // Make sure that there is not already one variable for the state pair. This may happen because of several nondeterministic choices
                                    // targeting the same state.
                                    if (variableInformation.statePairToIndexMap.find(std::make_pair(state, entry.getColumn())) != variableInformation.statePairToIndexMap.end()) {
                                        continue;
                                    }
                                    
                                    // At this point we know that the state-pair does not have an associated variable.
                                    variableInformation.statePairToIndexMap[std::make_pair(state, entry.getColumn())] = variableInformation.statePairVariables.size();
                                    
                                    // Clear contents of the stream to construct new expression name.
                                    variableName.clear();
                                    variableName.str("");
                                    variableName << "t" << state << "_" << entry.getColumn();
                                    
                                    variableInformation.statePairVariables.push_back(manager->declareBooleanVariable(variableName.str()));
                                }
                            }
                        }
                    }
                    
                    for (auto psiState : psiStates) {
                        variableInformation.relevantStatesToOrderVariableIndexMap[psiState] = variableInformation.stateOrderVariables.size();
                        
                        // Clear contents of the stream to construct new expression name.
                        variableName.clear();
                        variableName.str("");
                        variableName << "o" << psiState;
                        
                        variableInformation.stateOrderVariables.push_back(manager->declareRationalVariable(variableName.str()));
                    }
                }
                
                return variableInformation;
            }

            /*!
             * Asserts the constraints that are initially needed for the Fu-Malik procedure.
             *
             * @param program The program for which to build the constraints.
             * @param mdp The MDP that results from the given program.
             * @param context The Z3 context in which to build the expressions.
             * @param solver The solver in which to assert the constraints.
             * @param variableInformation A structure with information about the variables for the commands.
             */
            static void assertFuMalikInitialConstraints(storm::prism::Program const& program, storm::models::sparse::Mdp<T> const& mdp, storm::storage::BitVector const& psiStates, z3::context& context, z3::solver& solver, VariableInformation const& variableInformation, RelevancyInformation const& relevancyInformation) {
                // Assert that at least one of the commands must be taken.
                z3::expr formula = variableInformation.commandVariables.at(0);
                for (uint_fast64_t index = 1; index < variableInformation.commandVariables.size(); ++index) {
                    formula = formula || variableInformation.commandVariables.at(index);
                }
                solver.add(formula);
            }
            
            /*!
             * Asserts cuts that are derived from the explicit representation of the model and rule out a lot of
             * suboptimal solutions.
             *
             * @param mdp The MDP for which to compute the cuts.
             * @param context The Z3 context in which to build the expressions.
             * @param solver The solver to use for the satisfiability evaluation.
             */
            static void assertExplicitCuts(storm::models::sparse::Mdp<T> const& mdp, storm::storage::BitVector const& psiStates, VariableInformation const& variableInformation, RelevancyInformation const& relevancyInformation, storm::solver::SmtSolver& solver) {
                // Walk through the MDP and
                // * identify commands enabled in initial states
                // * identify commands that can directly precede a given action
                // * identify commands that directly reach a target state
                // * identify commands that can directly follow a given action
                
                boost::container::flat_set<uint_fast64_t> initialCommands;
                std::set<boost::container::flat_set<uint_fast64_t>> initialCombinations;
                std::map<uint_fast64_t, boost::container::flat_set<uint_fast64_t>> precedingCommands;
                boost::container::flat_set<uint_fast64_t> targetCommands;
                boost::container::flat_set<boost::container::flat_set<uint_fast64_t>> targetCombinations;
                std::map<boost::container::flat_set<uint_fast64_t>, std::set<boost::container::flat_set<uint_fast64_t>>> followingCommands;
                std::map<uint_fast64_t, std::set<boost::container::flat_set<uint_fast64_t>>> synchronizingCommands;
                
                // Get some data from the MDP for convenient access.
                storm::storage::SparseMatrix<T> const& transitionMatrix = mdp.getTransitionMatrix();
                storm::storage::BitVector const& initialStates = mdp.getInitialStates();
                storm::storage::SparseMatrix<T> backwardTransitions = mdp.getBackwardTransitions();
                storm::storage::sparse::PrismChoiceOrigins const& choiceOrigins = mdp.getChoiceOrigins()->asPrismChoiceOrigins();
                
                for (auto currentState : relevancyInformation.relevantStates) {
                    for (auto currentChoice : relevancyInformation.relevantChoicesForRelevantStates.at(currentState)) {
                        boost::container::flat_set<uint_fast64_t> const& commandsOfCurrentChoice = choiceOrigins.getCommandSet(currentChoice);
                        
                        // If the choice is a synchronization choice, we need to record it.
                        if (commandsOfCurrentChoice.size() > 1) {
                            for (auto command : commandsOfCurrentChoice) {
                                synchronizingCommands[command].emplace(commandsOfCurrentChoice);
                            }
                        }
                        
                        // If the state is initial, we need to add all the choice commands to the initial command set.
                        if (initialStates.get(currentState)) {
                            initialCommands.insert(commandsOfCurrentChoice.begin(), commandsOfCurrentChoice.end());
                            initialCombinations.insert(commandsOfCurrentChoice);
                        }
                        
                        // Iterate over successors and add relevant choices of relevant successors to the following command set.
                        bool canReachTargetState = false;
                        for (auto const& entry : transitionMatrix.getRow(currentChoice)) {
                            if (relevancyInformation.relevantStates.get(entry.getColumn())) {
                                for (auto relevantChoice : relevancyInformation.relevantChoicesForRelevantStates.at(entry.getColumn())) {
                                    followingCommands[commandsOfCurrentChoice].insert(choiceOrigins.getCommandSet(relevantChoice));
                                }
                            } else if (psiStates.get(entry.getColumn())) {
                                canReachTargetState = true;
                            }
                        }
                        
                        // If the choice can reach a target state directly, we add all the commands to the target command set.
                        if (canReachTargetState) {
                            targetCommands.insert(commandsOfCurrentChoice.begin(), commandsOfCurrentChoice.end());
                            targetCombinations.insert(commandsOfCurrentChoice);
                        }
                    }
                    
                    // Iterate over predecessors and add all choices that target the current state to the preceding
                    // command set of all commands of all relevant choices of the current state.
                    for (auto const& predecessorEntry : backwardTransitions.getRow(currentState)) {
                        if (relevancyInformation.relevantStates.get(predecessorEntry.getColumn())) {
                            for (auto predecessorChoice : relevancyInformation.relevantChoicesForRelevantStates.at(predecessorEntry.getColumn())) {
                                bool choiceTargetsCurrentState = false;
                                for (auto const& successorEntry : transitionMatrix.getRow(predecessorChoice)) {
                                    if (successorEntry.getColumn() == currentState) {
                                        choiceTargetsCurrentState = true;
                                    }
                                }
                                
                                if (choiceTargetsCurrentState) {
                                    for (auto currentChoice : relevancyInformation.relevantChoicesForRelevantStates.at(currentState)) {
                                        for (auto commandToAdd : choiceOrigins.getCommandSet(predecessorChoice)) {
                                            for (auto commandForWhichToAdd : choiceOrigins.getCommandSet(currentChoice)) {
                                                precedingCommands[commandForWhichToAdd].insert(commandToAdd);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                
                STORM_LOG_DEBUG("Successfully gathered data for explicit cuts.");
                
                STORM_LOG_DEBUG("Asserting initial combination is taken.");
                {
                    std::vector<storm::expressions::Expression> formulae;
                    
                    // Start by asserting that we take at least one initial command. We may do so only if there is no initial
                    // combination that is already known. Otherwise this condition would be too strong.
                    bool initialCombinationKnown = false;
                    for (auto const& combination : initialCombinations) {
                        boost::container::flat_set<uint_fast64_t> tmpSet;
                        std::set_difference(combination.begin(), combination.end(), relevancyInformation.knownCommands.begin(), relevancyInformation.knownCommands.end(), std::inserter(tmpSet, tmpSet.end()));
                        if (tmpSet.size() == 0) {
                            initialCombinationKnown = true;
                            break;
                        } else {
                            storm::expressions::Expression conj = variableInformation.manager->boolean(true);
                            for (auto command : tmpSet) {
                                conj = conj && variableInformation.commandVariables.at(variableInformation.commandToIndexMap.at(command));
                            }
                            formulae.push_back(conj);
                        }
                    }
                    if (!initialCombinationKnown) {
                        assertDisjunction(solver, formulae, *variableInformation.manager);
                    }
                }
                
                STORM_LOG_DEBUG("Asserting target combination is taken.");
                {
                    std::vector<storm::expressions::Expression> formulae;

                    // Likewise, if no target combination is known, we may assert that there is at least one.
                    bool targetCombinationKnown = false;
                    for (auto const& combination : targetCombinations) {
                        boost::container::flat_set<uint_fast64_t> tmpSet;
                        std::set_difference(combination.begin(), combination.end(), relevancyInformation.knownCommands.begin(), relevancyInformation.knownCommands.end(), std::inserter(tmpSet, tmpSet.end()));
                        if (tmpSet.size() == 0) {
                            targetCombinationKnown = true;
                            break;
                        } else {
                            storm::expressions::Expression conj = variableInformation.manager->boolean(true);
                            for (auto command : tmpSet) {
                                conj = conj && variableInformation.commandVariables.at(variableInformation.commandToIndexMap.at(command));
                            }
                            formulae.push_back(conj);
                        }
                    }
                    if (!targetCombinationKnown) {
                        assertDisjunction(solver, formulae, *variableInformation.manager);
                    }
                }
                
                // Compute the sets of commands such that the transitions originating from this set possess at least one known successor.
                boost::container::flat_set<boost::container::flat_set<uint_fast64_t>> hasKnownSuccessor;
                for (auto const& commandSetFollowingSetsPair : followingCommands) {
                    for (auto const& set : commandSetFollowingSetsPair.second) {
                        if (std::includes(relevancyInformation.knownCommands.begin(), relevancyInformation.knownCommands.end(), set.begin(), set.end())) {
                            hasKnownSuccessor.insert(set);
                            break;
                        }
                    }
                }
                
                STORM_LOG_DEBUG("Asserting taken commands are followed by another command if they are not a target command.");
                // Now assert that for each non-target command, we take a following command.
                for (auto const& commandSetFollowingSetsPair : followingCommands) {
                    std::vector<storm::expressions::Expression> formulae;
                    
                    // Only build a constraint if the combination does not lead to a target state and
                    // no successor set is already known.
                    if (targetCombinations.find(commandSetFollowingSetsPair.first) == targetCombinations.end() && hasKnownSuccessor.find(commandSetFollowingSetsPair.first) == hasKnownSuccessor.end()) {
                    
                        // Compute the set of unknown commands on the left-hand side of the implication.
                        boost::container::flat_set<uint_fast64_t> unknownLhsCommands;
                        std::set_difference(commandSetFollowingSetsPair.first.begin(), commandSetFollowingSetsPair.first.end(), relevancyInformation.knownCommands.begin(), relevancyInformation.knownCommands.end(), std::inserter(unknownLhsCommands, unknownLhsCommands.end()));
                        for (auto command : unknownLhsCommands) {
                            formulae.push_back(!variableInformation.commandVariables.at(variableInformation.commandToIndexMap.at(command)));
                        }
                        
                        for (auto const& followingSet : commandSetFollowingSetsPair.second) {
                            boost::container::flat_set<uint_fast64_t> tmpSet;
                            
                            // Check which commands of the current following set are not known. This set must be non-empty, because
                            // otherwise a successor combination would already be known and control cannot reach this point.
                            std::set_difference(followingSet.begin(), followingSet.end(), relevancyInformation.knownCommands.begin(), relevancyInformation.knownCommands.end(), std::inserter(tmpSet, tmpSet.end()));
                            
                            // Construct an expression that enables all unknown commands of the current following set.
                            storm::expressions::Expression conj = variableInformation.manager->boolean(true);
                            for (auto command : tmpSet) {
                                conj = conj && variableInformation.commandVariables.at(variableInformation.commandToIndexMap.at(command));
                            }
                            formulae.push_back(conj);
                        }
                        
                        if (commandSetFollowingSetsPair.first.size() > 1) {
                            // Taking all commands of a combination does not necessarily mean that a following command set needs to be taken.
                            // This is because it could be that the commands are taken to enable other synchronizations. Therefore, we need
                            // to add an additional clause that says that the right-hand side of the implication is also true if all commands
                            // of the current choice have enabled synchronization options.
                            storm::expressions::Expression finalDisjunct = variableInformation.manager->boolean(false);
                            for (auto command : commandSetFollowingSetsPair.first) {
                                storm::expressions::Expression alternativeExpressionForCommand = variableInformation.manager->boolean(false);
                                std::set<boost::container::flat_set<uint_fast64_t>> const& synchsForCommand = synchronizingCommands.at(command);
                                
                                for (auto const& synchSet : synchsForCommand) {
                                    storm::expressions::Expression alternativeExpression = variableInformation.manager->boolean(true);
                                    
                                    // If the current synchSet is the same as left-hand side of the implication, we need to skip it.
                                    if (synchSet == commandSetFollowingSetsPair.first) continue;
                                    
                                    // Now that we have the commands that are unknown and "missing", we still need to check whether this other
                                    // synchronizing set already has a known successor or leads directly to a target state.
                                    if (hasKnownSuccessor.find(synchSet) == hasKnownSuccessor.end() && targetCombinations.find(synchSet) == targetCombinations.end()) {
                                        // If not, we can assert that we take one of its possible successors.
                                        boost::container::flat_set<uint_fast64_t> unknownSynchs;
                                        std::set_difference(synchSet.begin(), synchSet.end(), relevancyInformation.knownCommands.begin(), relevancyInformation.knownCommands.end(), std::inserter(unknownSynchs, unknownSynchs.end()));
                                        unknownSynchs.erase(command);
                                        
                                        for (auto command : unknownSynchs) {
                                            alternativeExpression = alternativeExpression && variableInformation.commandVariables.at(variableInformation.commandToIndexMap.at(command));
                                        }
                                    
                                        storm::expressions::Expression disjunctionOverSuccessors = variableInformation.manager->boolean(false);
                                        for (auto successorSet : followingCommands.at(synchSet)) {
                                            storm::expressions::Expression conjunctionOverCommands = variableInformation.manager->boolean(true);
                                            for (auto command : successorSet) {
                                                if (relevancyInformation.knownCommands.find(command) == relevancyInformation.knownCommands.end()) {
                                                    conjunctionOverCommands = conjunctionOverCommands && variableInformation.commandVariables.at(variableInformation.commandToIndexMap.at(command));
                                                }
                                            }
                                            disjunctionOverSuccessors = disjunctionOverSuccessors || conjunctionOverCommands;
                                        }
                                        
                                        alternativeExpression = alternativeExpression && disjunctionOverSuccessors;
                                    }
                                    
                                    alternativeExpressionForCommand = alternativeExpressionForCommand || alternativeExpression;
                                }
                                
                                finalDisjunct = finalDisjunct && alternativeExpressionForCommand;
                            }
                            
                            formulae.push_back(finalDisjunct);
                        }

                        if (formulae.size() > 0) {
                            assertDisjunction(solver, formulae, *variableInformation.manager);
                        }
                    }
                }
                
                STORM_LOG_DEBUG("Asserting synchronization cuts.");
                // Finally, assert that if we take one of the synchronizing commands, we also take one of the combinations
                // the command appears in.
                for (auto const& commandSynchronizingSetsPair : synchronizingCommands) {
                    std::vector<storm::expressions::Expression> formulae;

                    if (relevancyInformation.knownCommands.find(commandSynchronizingSetsPair.first) == relevancyInformation.knownCommands.end()) {
                        formulae.push_back(!variableInformation.commandVariables.at(variableInformation.commandToIndexMap.at(commandSynchronizingSetsPair.first)));
                    }
                    
                    // We need to be careful, because there may be one synchronisation set out of which all commands are
                    // known, which means we must not assert anything.
                    bool allImplicantsKnownForOneSet = false;
                    for (auto const& synchronizingSet : commandSynchronizingSetsPair.second) {
                        storm::expressions::Expression currentCombination = variableInformation.manager->boolean(true);
                        bool allImplicantsKnownForCurrentSet = true;
                        for (auto command : synchronizingSet) {
                            if (relevancyInformation.knownCommands.find(command) == relevancyInformation.knownCommands.end() && command != commandSynchronizingSetsPair.first) {
                                currentCombination = currentCombination && variableInformation.commandVariables.at(variableInformation.commandToIndexMap.at(command));
                            }
                        }
                        formulae.push_back(currentCombination);
                        
                        // If all implicants of the current set are known, we do not need to further build the constraint.
                        if (allImplicantsKnownForCurrentSet) {
                            allImplicantsKnownForOneSet = true;
                            break;
                        }
                    }
                    
                    if (!allImplicantsKnownForOneSet) {
                        assertDisjunction(solver, formulae, *variableInformation.manager);
                    }
                }
            }

            /*!
             * Asserts cuts that are derived from the symbolic representation of the model and rule out a lot of
             * suboptimal solutions.
             *
             * @param program The symbolic representation of the model in terms of a program.
             * @param solver The solver to use for the satisfiability evaluation.
             */
            static void assertSymbolicCuts(storm::prism::Program& program, storm::models::sparse::Mdp<T> const& mdp, VariableInformation const& variableInformation, RelevancyInformation const& relevancyInformation, storm::solver::SmtSolver& solver) {
                // A container storing the command sets that may precede a given command set.
                std::map<boost::container::flat_set<uint_fast64_t>, std::set<boost::container::flat_set<uint_fast64_t>>> precedingCommandSets;

                // A container that maps commands to their reachable synchronization sets.
                std::map<uint_fast64_t, std::set<boost::container::flat_set<uint_fast64_t>>> synchronizingCommands;

                // Get some data from the MDP for convenient access.
                storm::storage::SparseMatrix<T> const& transitionMatrix = mdp.getTransitionMatrix();
                storm::storage::SparseMatrix<T> backwardTransitions = mdp.getBackwardTransitions();
                storm::storage::sparse::PrismChoiceOrigins const& choiceOrigins = mdp.getChoiceOrigins()->asPrismChoiceOrigins();
                
                // Compute the set of commands that may precede a given action.
                for (auto currentState : relevancyInformation.relevantStates) {
                    for (auto currentChoice : relevancyInformation.relevantChoicesForRelevantStates.at(currentState)) {
                        boost::container::flat_set<uint_fast64_t> const& commandsOfCurrentChoice = choiceOrigins.getCommandSet(currentChoice);
                        
                        // If the choice is a synchronization choice, we need to record it.
                        if (commandsOfCurrentChoice.size() > 1) {
                            for (auto command : commandsOfCurrentChoice) {
                                synchronizingCommands[command].emplace(commandsOfCurrentChoice);
                            }
                        }

                        // Iterate over predecessors and add all choices that target the current state to the preceding
                        // command set of all commands of all relevant choices of the current state.
                        for (auto const& predecessorEntry : backwardTransitions.getRow(currentState)) {
                            if (relevancyInformation.relevantStates.get(predecessorEntry.getColumn())) {
                                for (auto predecessorChoice : relevancyInformation.relevantChoicesForRelevantStates.at(predecessorEntry.getColumn())) {
                                    bool choiceTargetsCurrentState = false;
                                    for (auto const& successorEntry : transitionMatrix.getRow(predecessorChoice)) {
                                        if (successorEntry.getColumn() == currentState) {
                                            choiceTargetsCurrentState = true;
                                        }
                                    }
                                    
                                    if (choiceTargetsCurrentState) {
                                        precedingCommandSets[commandsOfCurrentChoice].insert(choiceOrigins.getCommandSet(predecessorChoice));
                                    }
                                }
                            }
                        }
                    }
                }
                
                // Create a new solver over the same variables as the given program to use it for determining the symbolic
                // cuts.
                std::unique_ptr<storm::solver::SmtSolver> localSolver(new storm::solver::Z3SmtSolver(program.getManager()));
                storm::expressions::ExpressionManager const& localManager = program.getManager();
//
//                // Create a context and register all variables of the program with their correct type.
//                z3::context localContext;
//                z3::solver localSolver(localContext);
//                std::map<std::string, z3::expr> solverVariables;
//                for (auto const& booleanVariable : program.getGlobalBooleanVariables()) {
//                    solverVariables.emplace(booleanVariable.getName(), localContext.bool_const(booleanVariable.getName().c_str()));
//                }
//                for (auto const& integerVariable : program.getGlobalIntegerVariables()) {
//                    solverVariables.emplace(integerVariable.getName(), localContext.int_const(integerVariable.getName().c_str()));
//                }
//                
//                for (auto const& module : program.getModules()) {
//                    for (auto const& booleanVariable : module.getBooleanVariables()) {
//                        solverVariables.emplace(booleanVariable.getName(), localContext.bool_const(booleanVariable.getName().c_str()));
//                    }
//                    for (auto const& integerVariable : module.getIntegerVariables()) {
//                        solverVariables.emplace(integerVariable.getName(), localContext.int_const(integerVariable.getName().c_str()));
//                    }
//                }
//                
//                storm::adapters::Z3ExpressionAdapter expressionAdapter(localContext, false, solverVariables);

                // Then add the constraints for bounds of the integer variables..
                for (auto const& integerVariable : program.getGlobalIntegerVariables()) {
                    localSolver->add(integerVariable.getExpressionVariable() >= integerVariable.getLowerBoundExpression());
                    localSolver->add(integerVariable.getExpressionVariable() <= integerVariable.getUpperBoundExpression());
                }
                for (auto const& module : program.getModules()) {
                    for (auto const& integerVariable : module.getIntegerVariables()) {
                        localSolver->add(integerVariable.getExpressionVariable() >= integerVariable.getLowerBoundExpression());
                        localSolver->add(integerVariable.getExpressionVariable() <= integerVariable.getUpperBoundExpression());
                    }
                }
                
                // Construct an expression that exactly characterizes the initial state.
                storm::expressions::Expression initialStateExpression = program.getInitialStatesExpression();
                
                // Store the found implications in a container similar to the preceding command sets.
                std::map<boost::container::flat_set<uint_fast64_t>, std::set<boost::container::flat_set<uint_fast64_t>>> backwardImplications;
                
                // Now check for possible backward cuts.
                for (auto const& commandSetAndPrecedingCommandSetsPair : precedingCommandSets) {
                
                    // Find out the commands for the currently considered command set.
                    std::vector<std::reference_wrapper<storm::prism::Command const>> currentCommandVector;
                    for (uint_fast64_t moduleIndex = 0; moduleIndex < program.getNumberOfModules(); ++moduleIndex) {
                        storm::prism::Module const& module = program.getModule(moduleIndex);
                        
                        for (uint_fast64_t commandIndex = 0; commandIndex < module.getNumberOfCommands(); ++commandIndex) {
                            storm::prism::Command const& command = module.getCommand(commandIndex);

                            // If the current command is one of the commands we need to consider, store a reference to it in the container.
                            if (commandSetAndPrecedingCommandSetsPair.first.find(command.getGlobalIndex()) != commandSetAndPrecedingCommandSetsPair.first.end()) {
                                currentCommandVector.push_back(command);
                            }
                        }
                    }
                    
                    // Save the state of the solver so we can easily backtrack.
                    localSolver->push();
                    
                    // Check if the command set is enabled in the initial state.
                    for (auto const& command : currentCommandVector) {
                        localSolver->add(command.get().getGuardExpression());
                    }
                    localSolver->add(initialStateExpression);
                    
                    storm::solver::SmtSolver::CheckResult checkResult = localSolver->check();
                    localSolver->pop();
                    localSolver->push();

                    // If the solver reports unsat, then we know that the current selection is not enabled in the initial state.
                    if (checkResult == storm::solver::SmtSolver::CheckResult::Unsat) {
                        STORM_LOG_DEBUG("Selection not enabled in initial state.");
                        storm::expressions::Expression guardConjunction;
                        if (currentCommandVector.size() == 1) {
                            guardConjunction = currentCommandVector.begin()->get().getGuardExpression();
                        } else if (currentCommandVector.size() > 1) {
                            std::vector<std::reference_wrapper<storm::prism::Command const>>::const_iterator setIterator = currentCommandVector.begin();
                            storm::expressions::Expression first = setIterator->get().getGuardExpression();
                            ++setIterator;
                            storm::expressions::Expression second = setIterator->get().getGuardExpression();
                            guardConjunction = first && second;
                            ++setIterator;
                            
                            while (setIterator != currentCommandVector.end()) {
                                guardConjunction = guardConjunction && setIterator->get().getGuardExpression();
                                ++setIterator;
                            }
                        } else {
                            throw storm::exceptions::InvalidStateException() << "Choice command set is empty.";
                            STORM_LOG_DEBUG("Choice command set is empty.");
                        }
                        
                        STORM_LOG_DEBUG("About to assert disjunction of negated guards.");
                        storm::expressions::Expression guardExpression = localManager.boolean(false);
                        bool firstAssignment = true;
                        for (auto const& command : currentCommandVector) {
                            if (firstAssignment) {
                                guardExpression = !command.get().getGuardExpression();
                            } else {
                                guardExpression = guardExpression || !command.get().getGuardExpression();
                            }
                        }
                        localSolver->add(guardExpression);
                        STORM_LOG_DEBUG("Asserted disjunction of negated guards.");
                        
                        // Now check the possible preceding command sets for the essential ones.
                        for (auto const& precedingCommandSet : commandSetAndPrecedingCommandSetsPair.second) {
                            // Create a restore point so we can easily pop-off all weakest precondition expressions.
                            localSolver->push();
                            
                            // Find out the commands for the currently considered preceding command set.
                            std::vector<std::reference_wrapper<storm::prism::Command const>> currentPrecedingCommandVector;
                            for (uint_fast64_t moduleIndex = 0; moduleIndex < program.getNumberOfModules(); ++moduleIndex) {
                                storm::prism::Module const& module = program.getModule(moduleIndex);
                                
                                for (uint_fast64_t commandIndex = 0; commandIndex < module.getNumberOfCommands(); ++commandIndex) {
                                    storm::prism::Command const& command = module.getCommand(commandIndex);
                                    
                                    // If the current command is one of the commands we need to consider, store a reference to it in the container.
                                    if (precedingCommandSet.find(command.getGlobalIndex()) != precedingCommandSet.end()) {
                                        currentPrecedingCommandVector.push_back(command);
                                    }
                                }
                            }
                            
                            // Assert all the guards of the preceding command set.
                            for (auto const& command : currentPrecedingCommandVector) {
                                localSolver->add(command.get().getGuardExpression());
                            }
                            
                            std::vector<std::vector<storm::prism::Update>::const_iterator> iteratorVector;
                            for (auto const& command : currentPrecedingCommandVector) {
                                iteratorVector.push_back(command.get().getUpdates().begin());
                            }
                            
                            // Iterate over all possible combinations of updates of the preceding command set.
                            std::vector<storm::expressions::Expression> formulae;
                            bool done = false;
                            while (!done) {
                                std::map<storm::expressions::Variable, storm::expressions::Expression> currentUpdateCombinationMap;
                                for (auto const& updateIterator : iteratorVector) {
                                    for (auto const& assignment : updateIterator->getAssignments()) {
                                        currentUpdateCombinationMap.emplace(assignment.getVariable(), assignment.getExpression());
                                    }
                                }
                                
                                STORM_LOG_DEBUG("About to assert a weakest precondition.");
                                storm::expressions::Expression wp = guardConjunction.substitute(currentUpdateCombinationMap);
                                formulae.push_back(wp);
                                STORM_LOG_DEBUG("Asserted weakest precondition.");
                                
                                // Now try to move iterators to the next position if possible. If we could properly move it, we can directly
                                // move on to the next combination of updates. If we have to reset it to the start, we
                                uint_fast64_t k = iteratorVector.size();
                                for (; k > 0; --k) {
                                    ++iteratorVector[k - 1];
                                    if (iteratorVector[k - 1] == currentPrecedingCommandVector[k - 1].get().getUpdates().end()) {
                                        iteratorVector[k - 1] = currentPrecedingCommandVector[k - 1].get().getUpdates().begin();
                                    } else {
                                        break;
                                    }
                                }
                                
                                // If we had to reset all iterator to the start, we are done.
                                if (k == 0) {
                                    done = true;
                                }
                            }

                            // Now assert the disjunction of all weakest preconditions of all considered update combinations.
                            assertDisjunction(*localSolver, formulae, localManager);
                            
                            STORM_LOG_DEBUG("Asserted disjunction of all weakest preconditions.");
                            
                            if (localSolver->check() == storm::solver::SmtSolver::CheckResult::Sat) {
                                backwardImplications[commandSetAndPrecedingCommandSetsPair.first].insert(precedingCommandSet);
                            }
                            
                            localSolver->pop();
                        }
                        
                        // Popping the disjunction of negated guards from the solver stack.
                        localSolver->pop();
                    }
                }
                
                // Compute the sets of commands such that the transitions originating from this set possess at least one known successor.
                boost::container::flat_set<boost::container::flat_set<uint_fast64_t>> hasKnownPredecessor;
                for (auto const& commandSetImplicationsPair : backwardImplications) {
                    for (auto const& set : commandSetImplicationsPair.second) {
                        if (std::includes(relevancyInformation.knownCommands.begin(), relevancyInformation.knownCommands.end(), set.begin(), set.end())) {
                            hasKnownPredecessor.insert(set);
                            break;
                        }
                    }
                }
                
                STORM_LOG_DEBUG("Asserting taken commands are preceded by another command if they are not an initial command.");
                // Now assert that for each non-target command, we take a following command.
                for (auto const& commandSetImplicationsPair : backwardImplications) {
                    std::vector<storm::expressions::Expression> formulae;
                    
                    // Only build a constraint if the combination no predecessor set is already known.
                    if (hasKnownPredecessor.find(commandSetImplicationsPair.first) == hasKnownPredecessor.end()) {
                        
                        // Compute the set of unknown commands on the left-hand side of the implication.
                        boost::container::flat_set<uint_fast64_t> unknownLhsCommands;
                        std::set_difference(commandSetImplicationsPair.first.begin(), commandSetImplicationsPair.first.end(), relevancyInformation.knownCommands.begin(), relevancyInformation.knownCommands.end(), std::inserter(unknownLhsCommands, unknownLhsCommands.end()));
                        for (auto command : unknownLhsCommands) {
                            formulae.push_back(!variableInformation.commandVariables.at(variableInformation.commandToIndexMap.at(command)));
                        }
                        
                        for (auto const& precedingSet : commandSetImplicationsPair.second) {
                            boost::container::flat_set<uint_fast64_t> tmpSet;
                            
                            // Check which commands of the current following set are not known. This set must be non-empty, because
                            // otherwise a predecessor combination would already be known and control cannot reach this point.
                            std::set_difference(precedingSet.begin(), precedingSet.end(), relevancyInformation.knownCommands.begin(), relevancyInformation.knownCommands.end(), std::inserter(tmpSet, tmpSet.end()));
                            
                            // Construct an expression that enables all unknown commands of the current following set.
                            storm::expressions::Expression conj = variableInformation.manager->boolean(true);
                            for (auto command : tmpSet) {
                                conj = conj && variableInformation.commandVariables.at(variableInformation.commandToIndexMap.at(command));
                            }
                            formulae.push_back(conj);
                        }
                        
                        if (commandSetImplicationsPair.first.size() > 1) {
                            // Taking all commands of a combination does not necessarily mean that a predecessor command set needs to be taken.
                            // This is because it could be that the commands are taken to enable other synchronizations. Therefore, we need
                            // to add an additional clause that says that the right-hand side of the implication is also true if all commands
                            // of the current choice have enabled synchronization options.
                            storm::expressions::Expression finalDisjunct = variableInformation.manager->boolean(false);
                            for (auto command : commandSetImplicationsPair.first) {
                                storm::expressions::Expression alternativeExpressionForCommand = variableInformation.manager->boolean(false);
                                std::set<boost::container::flat_set<uint_fast64_t>> const& synchsForCommand = synchronizingCommands.at(command);
                                
                                for (auto const& synchSet : synchsForCommand) {
                                    storm::expressions::Expression alternativeExpression = variableInformation.manager->boolean(true);

                                    // If the current synchSet is the same as left-hand side of the implication, we need to skip it.
                                    if (synchSet == commandSetImplicationsPair.first) continue;

                                    // Now that we have the commands that are unknown and "missing", we still need to check whether this other
                                    // synchronizing set already has a known predecessor.
                                    if (hasKnownPredecessor.find(synchSet) == hasKnownPredecessor.end()) {
                                        // If not, we can assert that we take one of its possible predecessors.
                                        boost::container::flat_set<uint_fast64_t> unknownSynchs;
                                        std::set_difference(synchSet.begin(), synchSet.end(), relevancyInformation.knownCommands.begin(), relevancyInformation.knownCommands.end(), std::inserter(unknownSynchs, unknownSynchs.end()));
                                        unknownSynchs.erase(command);

                                        for (auto command : unknownSynchs) {
                                            alternativeExpression = alternativeExpression && variableInformation.commandVariables.at(variableInformation.commandToIndexMap.at(command));
                                        }
                                        
                                        storm::expressions::Expression disjunctionOverPredecessors = variableInformation.manager->boolean(false);
                                        auto precedingCommandSetsIterator = precedingCommandSets.find(synchSet);
                                        if (precedingCommandSetsIterator != precedingCommandSets.end()) {
                                            for (auto precedingSet : precedingCommandSetsIterator->second) {
                                                storm::expressions::Expression conjunctionOverCommands = variableInformation.manager->boolean(true);
                                                for (auto command : precedingSet) {
                                                    if (relevancyInformation.knownCommands.find(command) == relevancyInformation.knownCommands.end()) {
                                                        conjunctionOverCommands = conjunctionOverCommands && variableInformation.commandVariables.at(variableInformation.commandToIndexMap.at(command));
                                                    }
                                                }
                                                disjunctionOverPredecessors = disjunctionOverPredecessors || conjunctionOverCommands;
                                            }
                                        }
                                        
                                        alternativeExpression = alternativeExpression && disjunctionOverPredecessors;
                                    }
                                    
                                    alternativeExpressionForCommand = alternativeExpressionForCommand || alternativeExpression;
                                }
                                
                                finalDisjunct = finalDisjunct && alternativeExpressionForCommand;
                            }

                            formulae.push_back(finalDisjunct);
                        }
                        
                        if (formulae.size() > 0) {
                            assertDisjunction(solver, formulae, *variableInformation.manager);
                        }
                    }
                }
            }
            
            /*!
             * Asserts constraints necessary to encode the reachability of at least one target state from the initial states.
             */
            static void assertReachabilityCuts(storm::models::sparse::Mdp<T> const& mdp, storm::storage::BitVector const& psiStates, VariableInformation const& variableInformation, RelevancyInformation const& relevancyInformation, storm::solver::SmtSolver& solver) {
                
                if (!variableInformation.hasReachabilityVariables) {
                    throw storm::exceptions::InvalidStateException() << "Impossible to assert reachability cuts without the necessary variables.";
                }
                
                // Get some data from the MDP for convenient access.
                storm::storage::SparseMatrix<T> const& transitionMatrix = mdp.getTransitionMatrix();
                storm::storage::SparseMatrix<T> backwardTransitions = mdp.getBackwardTransitions();
                storm::storage::sparse::PrismChoiceOrigins const& choiceOrigins = mdp.getChoiceOrigins()->asPrismChoiceOrigins();

                // First, we add the formulas that encode
                // (1) if an incoming transition is chosen, an outgoing one is chosen as well (for non-initial states)
                // (2) an outgoing transition out of the initial states is taken.
                storm::expressions::Expression initialStateExpression = variableInformation.manager->boolean(false);
                for (auto relevantState : relevancyInformation.relevantStates) {
                    if (!mdp.getInitialStates().get(relevantState)) {
                        // Assert the constraints (1).
                        boost::container::flat_set<uint_fast64_t> relevantPredecessors;
                        for (auto const& predecessorEntry : backwardTransitions.getRow(relevantState)) {
                            if (relevantState != predecessorEntry.getColumn() && relevancyInformation.relevantStates.get(predecessorEntry.getColumn())) {
                                relevantPredecessors.insert(predecessorEntry.getColumn());
                            }
                        }
                        
                        boost::container::flat_set<uint_fast64_t> relevantSuccessors;
                        for (auto const& relevantChoice : relevancyInformation.relevantChoicesForRelevantStates.at(relevantState)) {
                            for (auto const& successorEntry : transitionMatrix.getRow(relevantChoice)) {
                                if (relevantState != successorEntry.getColumn() && (relevancyInformation.relevantStates.get(successorEntry.getColumn()) || psiStates.get(successorEntry.getColumn()))) {
                                    relevantSuccessors.insert(successorEntry.getColumn());
                                }
                            }
                        }
                        
                        storm::expressions::Expression expression = variableInformation.manager->boolean(true);
                        for (auto predecessor : relevantPredecessors) {
                            expression = expression && !variableInformation.statePairVariables.at(variableInformation.statePairToIndexMap.at(std::make_pair(predecessor, relevantState)));
                        }
                        for (auto successor : relevantSuccessors) {
                            expression = expression || variableInformation.statePairVariables.at(variableInformation.statePairToIndexMap.at(std::make_pair(relevantState, successor)));
                        }
                        
                        solver.add(expression);
                    } else {
                        // Assert the constraints (2).
                        boost::container::flat_set<uint_fast64_t> relevantSuccessors;
                        for (auto const& relevantChoice : relevancyInformation.relevantChoicesForRelevantStates.at(relevantState)) {
                            for (auto const& successorEntry : transitionMatrix.getRow(relevantChoice)) {
                                if (relevantState != successorEntry.getColumn() && (relevancyInformation.relevantStates.get(successorEntry.getColumn()) || psiStates.get(successorEntry.getColumn()))) {
                                    relevantSuccessors.insert(successorEntry.getColumn());
                                }
                            }
                        }
                        
                        for (auto successor : relevantSuccessors) {
                            initialStateExpression = initialStateExpression || variableInformation.statePairVariables.at(variableInformation.statePairToIndexMap.at(std::make_pair(relevantState, successor)));
                        }
                    }
                }
                solver.add(initialStateExpression);
                
                // Finally, add constraints that
                // (1) if a transition is selected, a valid labeling is selected as well.
                // (2) enforce that if a transition from s to s' is selected, the ordering variables become strictly larger.
                for (auto const& statePairIndexPair : variableInformation.statePairToIndexMap) {
                    uint_fast64_t sourceState = statePairIndexPair.first.first;
                    uint_fast64_t targetState = statePairIndexPair.first.second;
                    
                    // Assert constraint for (1).
                    boost::container::flat_set<uint_fast64_t> choicesForStatePair;
                    for (auto const& relevantChoice : relevancyInformation.relevantChoicesForRelevantStates.at(sourceState)) {
                        for (auto const& successorEntry : transitionMatrix.getRow(relevantChoice)) {
                            if (successorEntry.getColumn() == targetState) {
                                choicesForStatePair.insert(relevantChoice);
                            }
                        }
                    }
                    storm::expressions::Expression commandExpression = !variableInformation.statePairVariables.at(statePairIndexPair.second);
                    for (auto choice : choicesForStatePair) {
                        storm::expressions::Expression choiceExpression = variableInformation.manager->boolean(true);
                        for (auto element : choiceOrigins.getCommandSet(choice)) {
                            if (relevancyInformation.knownCommands.find(element) == relevancyInformation.knownCommands.end()) {
                                choiceExpression = choiceExpression && variableInformation.commandVariables.at(variableInformation.commandToIndexMap.at(element));
                            }
                        }
                        commandExpression = commandExpression || choiceExpression;
                    }
                    solver.add(commandExpression);

                    // Assert constraint for (2).
                    storm::expressions::Expression orderExpression = !variableInformation.statePairVariables.at(statePairIndexPair.second) || variableInformation.stateOrderVariables.at(variableInformation.relevantStatesToOrderVariableIndexMap.at(sourceState)).getExpression() < variableInformation.stateOrderVariables.at(variableInformation.relevantStatesToOrderVariableIndexMap.at(targetState)).getExpression();
                    solver.add(orderExpression);
                }
            }
        
            /*!
             * Asserts that the disjunction of the given formulae holds. If the content of the disjunction is empty,
             * this corresponds to asserting false.
             *
             * @param solver The solver to use for the satisfiability evaluation.
             * @param formulaVector A vector of expressions that shall form the disjunction.
             * @param manager The expression manager to use.
             */
            static void assertDisjunction(storm::solver::SmtSolver& solver, std::vector<storm::expressions::Expression> const& formulaVector, storm::expressions::ExpressionManager const& manager) {
                storm::expressions::Expression disjunction = manager.boolean(false);
                for (auto expr : formulaVector) {
                    disjunction = disjunction || expr;
                }
                solver.add(disjunction);
            }
            
            /*!
             * Asserts that the conjunction of the given formulae holds. If the content of the conjunction is empty,
             * this corresponds to asserting true.
             *
             * @param context The Z3 context in which to build the expressions.
             * @param solver The solver to use for the satisfiability evaluation.
             * @param formulaVector A vector of expressions that shall form the conjunction.
             */
            static void assertConjunction(z3::context& context, z3::solver& solver, std::vector<z3::expr> const& formulaVector) {
                z3::expr conjunction = context.bool_val(true);
                for (auto expr : formulaVector) {
                    conjunction = conjunction && expr;
                }
                solver.add(conjunction);
            }
            
            /*!
             * Creates a full-adder for the two inputs and returns the resulting bit as well as the carry bit.
             *
             * @param in1 The first input to the adder.
             * @param in2 The second input to the adder.
             * @param carryIn The carry bit input to the adder.
             * @return A pair whose first component represents the carry bit and whose second component represents the
             * result bit.
             */
            static std::pair<storm::expressions::Expression, storm::expressions::Expression> createFullAdder(storm::expressions::Expression in1, storm::expressions::Expression in2, storm::expressions::Expression carryIn) {
                storm::expressions::Expression resultBit = (in1 && !in2 && !carryIn) || (!in1 && in2 && !carryIn) || (!in1 && !in2 && carryIn) || (in1 && in2 && carryIn);
                storm::expressions::Expression carryBit = in1 && in2 || in1 && carryIn || in2 && carryIn;
                
                return std::make_pair(carryBit, resultBit);
            }
            
            /*!
             * Creates an adder for the two inputs of equal size. The resulting vector represents the different bits of
             * the sum (and is thus one bit longer than the two inputs).
             *
             * @param variableInformation The variable information structure.
             * @param in1 The first input to the adder.
             * @param in2 The second input to the adder.
             * @return A vector representing the bits of the sum of the two inputs.
             */
            static std::vector<storm::expressions::Expression> createAdder(VariableInformation const& variableInformation, std::vector<storm::expressions::Expression> const& in1, std::vector<storm::expressions::Expression> const& in2) {
                // Sanity check for sizes of input.
                if (in1.size() != in2.size() || in1.size() == 0) {
                    STORM_LOG_ERROR("Illegal input to adder (" << in1.size() << ", " << in2.size() << ").");
                    throw storm::exceptions::InvalidArgumentException() << "Illegal input to adder.";
                }
                
                // Prepare result.
                std::vector<storm::expressions::Expression> result;
                result.reserve(in1.size() + 1);
                
                // Add all bits individually and pass on carry bit appropriately.
                storm::expressions::Expression carryBit = variableInformation.manager->boolean(false);
                for (uint_fast64_t currentBit = 0; currentBit < in1.size(); ++currentBit) {
                    std::pair<storm::expressions::Expression, storm::expressions::Expression> localResult = createFullAdder(in1[currentBit], in2[currentBit], carryBit);
                    
                    result.push_back(localResult.second);
                    carryBit = localResult.first;
                }
                result.push_back(carryBit);
                
                return result;
            }
            
            /*!
             * Given a number of input numbers, creates a number of output numbers that corresponds to the sum of two
             * consecutive numbers of the input. If the number if input numbers is odd, the last number is simply added
             * to the output.
             *
             * @param variableInformation The variable information structure.
             * @param in A vector or binary encoded numbers.
             * @return A vector of numbers that each correspond to the sum of two consecutive elements of the input.
             */
            static std::vector<std::vector<storm::expressions::Expression>> createAdderPairs(VariableInformation const& variableInformation, std::vector<std::vector<storm::expressions::Expression>> const& in) {
                std::vector<std::vector<storm::expressions::Expression>> result;
                
                result.reserve(in.size() / 2 + in.size() % 2);
                
                for (uint_fast64_t index = 0; index < in.size() / 2; ++index) {
                    result.push_back(createAdder(variableInformation, in[2 * index], in[2 * index + 1]));
                }
                
                if (in.size() % 2 != 0) {
                    result.push_back(in.back());
                    result.back().push_back(variableInformation.manager->boolean(false));
                }
                
                return result;
            }
            
            /*!
             * Creates a counter circuit that returns the number of literals out of the given vector that are set to true.
             *
             * @param variableInformation The variable information structure.
             * @param literals The literals for which to create the adder circuit.
             * @return A bit vector representing the number of literals that are set to true.
             */
            static std::vector<storm::expressions::Expression> createCounterCircuit(VariableInformation const& variableInformation, std::vector<storm::expressions::Variable> const& literals) {
                STORM_LOG_DEBUG("Creating counter circuit for " << literals.size() << " literals.");

                if (literals.empty()) {
                    return std::vector<storm::expressions::Expression>();
                }
                
                // Create the auxiliary vector.
                std::vector<std::vector<storm::expressions::Expression>> aux;
                for (uint_fast64_t index = 0; index < literals.size(); ++index) {
                    aux.emplace_back();
                    aux.back().push_back(literals[index]);
                }
                
                while (aux.size() > 1) {
                    aux = createAdderPairs(variableInformation, aux);
                }
                
                return aux.front();
            }
            
            /*!
             * Determines whether the bit at the given index is set in the given value.
             *
             * @param value The value to test.
             * @param index The index of the bit to test.
             * @return True iff the bit at the given index is set in the given value.
             */
            static bool bitIsSet(uint64_t value, uint64_t index) {
                uint64_t mask = 1 << (index & 63);
                return (value & mask) != 0;
            }
            
            /*!
             * Asserts a constraint in the given solver that expresses that the value encoded by the given input variables
             * may at most represent the number k. The constraint is associated with a relaxation variable, that is
             * returned by this function and may be used to deactivate the constraint.
             *
             * @param solver The solver to use for the satisfiability evaluation.
             * @param variableInformation The struct that holds the variable information.
             * @param k The bound for the binary-encoded value.
             * @return The relaxation variable associated with the constraint.
             */
            static storm::expressions::Variable assertLessOrEqualKRelaxed(storm::solver::SmtSolver& solver, VariableInformation const& variableInformation, uint64_t k) {
                STORM_LOG_DEBUG("Asserting solution has size less or equal " << k << ".");
                
                std::vector<storm::expressions::Variable> const& input = variableInformation.adderVariables;
                
                // If there are no input variables, the value is always 0 <= k, so there is nothing to assert.
                if (input.empty()) {
                    std::stringstream variableName;
                    variableName << "relaxed" << k;
                    return variableInformation.manager->declareBooleanVariable(variableName.str());
                }
                
                storm::expressions::Expression result;
                if (bitIsSet(k, 0)) {
                    result = variableInformation.manager->boolean(true);
                } else {
                    result = !input.at(0);
                }
                for (uint_fast64_t index = 1; index < input.size(); ++index) {
                    storm::expressions::Expression i1;
                    storm::expressions::Expression i2;
                    
                    if (bitIsSet(k, index)) {
                        i1 = !input.at(index);
                        i2 = result;
                    } else {
                        i1 = variableInformation.manager->boolean(false);
                        i2 = variableInformation.manager->boolean(false);
                    }
                    result = i1 || i2 || (!input.at(index) && result);
                }
                
                std::stringstream variableName;
                variableName << "relaxed" << k;
                storm::expressions::Variable relaxingVariable = variableInformation.manager->declareBooleanVariable(variableName.str());
                result = result || relaxingVariable;
                
                solver.add(result);
                
                return relaxingVariable;
            }
            
            /*!
             * Asserts that the input vector encodes a decimal smaller or equal to one.
             *
             * @param context The Z3 context in which to build the expressions.
             * @param solver The solver to use for the satisfiability evaluation.
             * @param input The binary encoded input number.
             */
            static void assertLessOrEqualOne(z3::context& context, z3::solver& solver, std::vector<z3::expr> input) {
                std::transform(input.begin(), input.end(), input.begin(), [](z3::expr e) -> z3::expr { return !e; });
                assertConjunction(context, solver, input);
            }
            
            /*!
             * Asserts that at most one of given literals may be true at any time.
             *
             * @param context The Z3 context in which to build the expressions.
             * @param solver The solver to use for the satisfiability evaluation.
             * @param blockingVariables A vector of variables out of which only one may be true.
             */
            static void assertAtMostOne(z3::context& context, z3::solver& solver, std::vector<z3::expr> const& literals) {
                std::vector<z3::expr> counter = createCounterCircuit(context, literals);
                assertLessOrEqualOne(context, solver, counter);
            }
            
            /*!
             * Performs one Fu-Malik-Maxsat step.
             *
             * @param context The Z3 context in which to build the expressions.
             * @param solver The solver to use for the satisfiability evaluation.
             * @param variableInformation A structure with information about the variables for the commands.
             * @return True iff the constraint system was satisfiable.
             */
            static bool fuMalikMaxsatStep(z3::context& context, z3::solver& solver, std::vector<z3::expr>& auxiliaryVariables, std::vector<z3::expr>& softConstraints, uint_fast64_t& nextFreeVariableIndex) {
                z3::expr_vector assumptions(context);
                for (auto const& auxiliaryVariable : auxiliaryVariables) {
                    assumptions.push_back(!auxiliaryVariable);
                }
                                
                // Check whether the assumptions are satisfiable.
                STORM_LOG_DEBUG("Invoking satisfiability checking.");
                z3::check_result result = solver.check(assumptions);
                STORM_LOG_DEBUG("Done invoking satisfiability checking.");
                
                if (result == z3::sat) {
                    return true;
                } else {
                    STORM_LOG_DEBUG("Computing unsat core.");
                    z3::expr_vector unsatCore = solver.unsat_core();
                    STORM_LOG_DEBUG("Computed unsat core.");
                    
                    std::vector<z3::expr> blockingVariables;
                    blockingVariables.reserve(unsatCore.size());
                    
                    // Create stringstream to build expression names.
                    std::stringstream variableName;
                    
                    for (uint_fast64_t softConstraintIndex = 0; softConstraintIndex < softConstraints.size(); ++softConstraintIndex) {
                        for (uint_fast64_t coreIndex = 0; coreIndex < unsatCore.size(); ++coreIndex) {
                            bool isContainedInCore = false;
                            if (softConstraints[softConstraintIndex] == unsatCore[coreIndex]) {
                                isContainedInCore = true;
                            }
                            
                            if (isContainedInCore) {
                                variableName.clear();
                                variableName.str("");
                                variableName << "b" << nextFreeVariableIndex;
                                blockingVariables.push_back(context.bool_const(variableName.str().c_str()));
                                
                                variableName.clear();
                                variableName.str("");
                                variableName << "a" << nextFreeVariableIndex;
                                ++nextFreeVariableIndex;
                                auxiliaryVariables[softConstraintIndex] = context.bool_const(variableName.str().c_str());
                                
                                softConstraints[softConstraintIndex] = softConstraints[softConstraintIndex] || blockingVariables.back();
                                
                                solver.add(softConstraints[softConstraintIndex] || auxiliaryVariables[softConstraintIndex]);
                            }
                        }
                    }
                    
                    assertAtMostOne(context, solver, blockingVariables);
                }
                
                return false;
            }
            
            /*!
             * Rules out the given command set for the given solver.
             *
             * @param context The Z3 context in which to build the expressions.
             * @param solver The solver to use for the satisfiability evaluation.
             * @param commandSet The command set to rule out as a solution.
             * @param variableInformation A structure with information about the variables for the commands.
             */
            static void ruleOutSolution(z3::context& context, z3::solver& solver, boost::container::flat_set<uint_fast64_t> const& commandSet, VariableInformation const& variableInformation) {
                z3::expr blockSolutionExpression = context.bool_val(false);
                for (auto commandIndexPair : variableInformation.commandToIndexMap) {
                    if (commandSet.find(commandIndexPair.first) != commandSet.end()) {
                        blockSolutionExpression = blockSolutionExpression || variableInformation.commandVariables[commandIndexPair.second];
                    }
                }
                
                solver.add(blockSolutionExpression);
            }

            /*!
             * Determines the set of commands that was chosen by the given model.
             *
             * @param model The model from which to extract the information.
             * @param variableInformation A structure with information about the variables of the solver.
             */
            static boost::container::flat_set<uint_fast64_t> getUsedCommandSet(storm::solver::SmtSolver::ModelReference const& model, VariableInformation const& variableInformation) {
                boost::container::flat_set<uint_fast64_t> result;
                for (auto const& commandIndexPair : variableInformation.commandToIndexMap) {
                    bool commandIncluded = model.getBooleanValue(variableInformation.commandVariables.at(commandIndexPair.second));
                    
                    if (commandIncluded) {
                        result.insert(commandIndexPair.first);
                    }
                }
                return result;
            }
            
            /*!
             * Asserts an adder structure in the given solver that counts the number of variables that are set to true
             * out of the given variables.
             *
             * @param solver The solver for which to add the adder.
             * @param variableInformation A structure with information about the variables of the solver.
             */
            static std::vector<storm::expressions::Variable> assertAdder(storm::solver::SmtSolver& solver, VariableInformation const& variableInformation) {
                std::stringstream variableName;
                std::vector<storm::expressions::Variable> result;
                
                std::vector<storm::expressions::Expression> adderVariables = createCounterCircuit(variableInformation, variableInformation.commandVariables);
                for (uint_fast64_t i = 0; i < adderVariables.size(); ++i) {
                    variableName.str("");
                    variableName.clear();
                    variableName << "adder" << i;
                    result.push_back(variableInformation.manager->declareBooleanVariable(variableName.str()));
                    solver.add(storm::expressions::implies(adderVariables[i], result.back()));
                }
                
                return result;
            }
            
            /*!
             * Finds the smallest set of commands such that the constraint system of the solver is still satisfiable.
             *
             * @param solver The solver to use for the satisfiability evaluation.
             * @param variableInformation A structure with information about the variables of the solver.
             * @param currentBound The currently known lower bound for the number of commands that need to be enabled
             * in order to satisfy the constraint system.
             * @return The smallest set of commands such that the constraint system of the solver is satisfiable.
             */
            static boost::container::flat_set<uint_fast64_t> findSmallestCommandSet(storm::solver::SmtSolver& solver, VariableInformation& variableInformation, uint_fast64_t& currentBound) {
                // Check if we can find a solution with the current bound.
                storm::expressions::Expression assumption = !variableInformation.auxiliaryVariables.back();

                // As long as the constraints are unsatisfiable, we need to relax the last at-most-k constraint and
                // try with an increased bound.
                while (solver.checkWithAssumptions({assumption}) == storm::solver::SmtSolver::CheckResult::Unsat) {
                    STORM_LOG_DEBUG("Constraint system is unsatisfiable with at most " << currentBound << " taken commands; increasing bound.");
                    solver.add(variableInformation.auxiliaryVariables.back());
                    variableInformation.auxiliaryVariables.push_back(assertLessOrEqualKRelaxed(solver, variableInformation, ++currentBound));
                    assumption = !variableInformation.auxiliaryVariables.back();
                }
                
                // At this point we know that the constraint system was satisfiable, so compute the induced command
                // set and return it.
                return getUsedCommandSet(*solver.getModel(), variableInformation);
            }
            
            /*!
             * Analyzes the given sub-MDP that has a maximal reachability of zero (i.e. no psi states are reachable) and tries to construct assertions that aim to make at least one psi state reachable.
             *
             * @param solver The solver to use for the satisfiability evaluation.
             * @param subMdp The sub-MDP resulting from restricting the original MDP to the given command set.
             * @param originalMdp The original MDP.
             * @param phiStates A bit vector characterizing all phi states in the model.
             * @param psiState A bit vector characterizing all psi states in the model.
             * @param commandSet The currently chosen set of commands.
             * @param variableInformation A structure with information about the variables of the solver.
             */
            static void analyzeZeroProbabilitySolution(storm::solver::SmtSolver& solver, storm::models::sparse::Mdp<T> const& subMdp, storm::models::sparse::Mdp<T> const& originalMdp, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, boost::container::flat_set<uint_fast64_t> const& commandSet, VariableInformation& variableInformation, RelevancyInformation const& relevancyInformation) {
                storm::storage::BitVector reachableStates(subMdp.getNumberOfStates());
                
                STORM_LOG_DEBUG("Analyzing solution with zero probability.");
                
                // Initialize the stack for the DFS.
                bool targetStateIsReachable = false;
                std::vector<uint_fast64_t> stack;
                stack.reserve(subMdp.getNumberOfStates());
                for (auto initialState : subMdp.getInitialStates()) {
                    stack.push_back(initialState);
                    reachableStates.set(initialState, true);
                }
                
                storm::storage::SparseMatrix<T> const& transitionMatrix = subMdp.getTransitionMatrix();
                std::vector<uint_fast64_t> const& nondeterministicChoiceIndices = subMdp.getNondeterministicChoiceIndices();
                storm::storage::sparse::PrismChoiceOrigins const& subChoiceOrigins = subMdp.getChoiceOrigins()->asPrismChoiceOrigins();
                
                // Now determine which states and commands are actually reachable.

                boost::container::flat_set<uint_fast64_t> reachableCommands;
                while (!stack.empty()) {
                    uint_fast64_t currentState = stack.back();
                    stack.pop_back();

                    for (uint_fast64_t currentChoice = nondeterministicChoiceIndices[currentState]; currentChoice < nondeterministicChoiceIndices[currentState + 1]; ++currentChoice) {
                        bool choiceTargetsRelevantState = false;
                        
                        for (auto const& successorEntry : transitionMatrix.getRow(currentChoice)) {
                            if (relevancyInformation.relevantStates.get(successorEntry.getColumn()) && currentState != successorEntry.getColumn()) {
                                choiceTargetsRelevantState = true;
                                if (!reachableStates.get(successorEntry.getColumn())) {
                                    reachableStates.set(successorEntry.getColumn());
                                    stack.push_back(successorEntry.getColumn());
                                }
                            } else if (psiStates.get(successorEntry.getColumn())) {
                                targetStateIsReachable = true;
                            }
                        }
                        
                        if (choiceTargetsRelevantState) {
                            for (auto command : subChoiceOrigins.getCommandSet(currentChoice)) {
                                reachableCommands.insert(command);
                            }
                        }
                    }
                }
                
                STORM_LOG_DEBUG("Successfully performed reachability analysis.");
                
                if (targetStateIsReachable) {
                    STORM_LOG_ERROR("Target must be unreachable for this analysis.");
                    throw storm::exceptions::InvalidStateException() << "Target must be unreachable for this analysis.";
                }
                
                storm::storage::BitVector unreachableRelevantStates = ~reachableStates & relevancyInformation.relevantStates;
                storm::storage::BitVector statesThatCanReachTargetStates = storm::utility::graph::performProbGreater0E(subMdp.getBackwardTransitions(), phiStates, psiStates);
                
                boost::container::flat_set<uint_fast64_t> locallyRelevantCommands;
                std::set_difference(relevancyInformation.relevantCommands.begin(), relevancyInformation.relevantCommands.end(), commandSet.begin(), commandSet.end(), std::inserter(locallyRelevantCommands, locallyRelevantCommands.begin()));
                
                storm::storage::sparse::PrismChoiceOrigins const& originalChoiceOrigins = originalMdp.getChoiceOrigins()->asPrismChoiceOrigins();
                
                std::vector<boost::container::flat_set<uint_fast64_t>> guaranteedCommandSets = storm::utility::counterexamples::getGuaranteedCommandSets(originalMdp, statesThatCanReachTargetStates, locallyRelevantCommands);
                STORM_LOG_DEBUG("Found " << reachableCommands.size() << " reachable commands and " << reachableStates.getNumberOfSetBits() << " reachable states.");
                
                // Search for states on the border of the reachable state space, i.e. states that are still reachable
                // and possess a (disabled) option to leave the reachable part of the state space.
                std::set<boost::container::flat_set<uint_fast64_t>> cutCommands;
                for (auto state : reachableStates) {
                    for (auto currentChoice : relevancyInformation.relevantChoicesForRelevantStates.at(state)) {
                        if (!std::includes(commandSet.begin(), commandSet.end(), originalChoiceOrigins.getCommandSet(currentChoice).begin(), originalChoiceOrigins.getCommandSet(currentChoice).end())) {
                            bool isBorderChoice = false;

                            // Determine whether the state has the option to leave the reachable state space and go to the unreachable relevant states.
                            for (auto const& successorEntry : originalMdp.getTransitionMatrix().getRow(currentChoice)) {
                                if (unreachableRelevantStates.get(successorEntry.getColumn())) {
                                    isBorderChoice = true;
                                }
                            }
                            
                            if (isBorderChoice) {
                                boost::container::flat_set<uint_fast64_t> currentCommandSet;
                                for (auto command : originalChoiceOrigins.getCommandSet(currentChoice)) {
                                    if (commandSet.find(command) == commandSet.end()) {
                                        currentCommandSet.insert(command);
                                    }
                                }
                                std::set_difference(guaranteedCommandSets[state].begin(), guaranteedCommandSets[state].end(), commandSet.begin(), commandSet.end(), std::inserter(currentCommandSet, currentCommandSet.end()));
                                
                                cutCommands.insert(currentCommandSet);
                            }
                        }
                    }
                }
                
                // Given the results of the previous analysis, we construct the implications.
                std::vector<storm::expressions::Expression> formulae;
                boost::container::flat_set<uint_fast64_t> unknownReachableCommands;
                std::set_difference(reachableCommands.begin(), reachableCommands.end(), relevancyInformation.knownCommands.begin(), relevancyInformation.knownCommands.end(), std::inserter(unknownReachableCommands, unknownReachableCommands.end()));
                for (auto command : unknownReachableCommands) {
                    formulae.push_back(!variableInformation.commandVariables.at(variableInformation.commandToIndexMap.at(command)));
                }
                for (auto const& cutCommandSet : cutCommands) {
                    storm::expressions::Expression cube = variableInformation.manager->boolean(true);
                    for (auto cutCommand : cutCommandSet) {
                        cube = cube && variableInformation.commandVariables.at(variableInformation.commandToIndexMap.at(cutCommand));
                    }
                    
                    formulae.push_back(cube);
                }
                
                STORM_LOG_DEBUG("Asserting reachability implications.");
                assertDisjunction(solver, formulae, *variableInformation.manager);
            }
            
            /*!
             * Analyzes the given sub-MDP that has a non-zero maximal reachability and tries to construct assertions that aim to guide the solver to solutions
             * with an improved probability value.
             *
             * @param solver The solver to use for the satisfiability evaluation.
             * @param subMdp The sub-MDP resulting from restricting the original MDP to the given command set.
             * @param originalMdp The original MDP.
             * @param phiStates A bit vector characterizing all phi states in the model.
             * @param psiState A bit vector characterizing all psi states in the model.
             * @param commandSet The currently chosen set of commands.
             * @param variableInformation A structure with information about the variables of the solver.
             */
            static void analyzeInsufficientProbabilitySolution(storm::solver::SmtSolver& solver, storm::models::sparse::Mdp<T> const& subMdp, storm::models::sparse::Mdp<T> const& originalMdp, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, boost::container::flat_set<uint_fast64_t> const& commandSet, VariableInformation& variableInformation, RelevancyInformation const& relevancyInformation) {

                STORM_LOG_DEBUG("Analyzing solution with insufficient probability.");

                storm::storage::BitVector reachableStates(subMdp.getNumberOfStates());
                
                // Initialize the stack for the DFS.
                std::vector<uint_fast64_t> stack;
                stack.reserve(subMdp.getNumberOfStates());
                for (auto initialState : subMdp.getInitialStates()) {
                    stack.push_back(initialState);
                    reachableStates.set(initialState, true);
                }
                
                storm::storage::SparseMatrix<T> const& transitionMatrix = subMdp.getTransitionMatrix();
                std::vector<uint_fast64_t> const& nondeterministicChoiceIndices = subMdp.getNondeterministicChoiceIndices();
                storm::storage::sparse::PrismChoiceOrigins const& subChoiceOrigins = subMdp.getChoiceOrigins()->asPrismChoiceOrigins();
                
                // Now determine which states and commands are actually reachable.
                boost::container::flat_set<uint_fast64_t> reachableCommands;
                while (!stack.empty()) {
                    uint_fast64_t currentState = stack.back();
                    stack.pop_back();
                    
                    for (uint_fast64_t currentChoice = nondeterministicChoiceIndices[currentState]; currentChoice < nondeterministicChoiceIndices[currentState + 1]; ++currentChoice) {
                        bool choiceTargetsRelevantState = false;
                        
                        for (auto const& successorEntry : transitionMatrix.getRow(currentChoice)) {
                            if (relevancyInformation.relevantStates.get(successorEntry.getColumn()) && currentState != successorEntry.getColumn()) {
                                choiceTargetsRelevantState = true;
                                if (!reachableStates.get(successorEntry.getColumn())) {
                                    reachableStates.set(successorEntry.getColumn(), true);
                                    stack.push_back(successorEntry.getColumn());
                                }
                            } //else if (psiStates.get(successorEntry.getColumn())) {
                                //targetStateIsReachable = true;
                            //}
                        }
                        
                        if (choiceTargetsRelevantState) {
                            for (auto command : subChoiceOrigins.getCommandSet(currentChoice)) {
                                reachableCommands.insert(command);
                            }
                        }
                    }
                }
                STORM_LOG_DEBUG("Successfully determined reachable state space.");
                
                storm::storage::BitVector unreachableRelevantStates = ~reachableStates & relevancyInformation.relevantStates;
                storm::storage::BitVector statesThatCanReachTargetStates = storm::utility::graph::performProbGreater0E(subMdp.getBackwardTransitions(), phiStates, psiStates);
                
                boost::container::flat_set<uint_fast64_t> locallyRelevantCommands;
                std::set_difference(relevancyInformation.relevantCommands.begin(), relevancyInformation.relevantCommands.end(), commandSet.begin(), commandSet.end(), std::inserter(locallyRelevantCommands, locallyRelevantCommands.begin()));
                
                storm::storage::sparse::PrismChoiceOrigins const& originalChoiceOrigins = originalMdp.getChoiceOrigins()->asPrismChoiceOrigins();
                
                std::vector<boost::container::flat_set<uint_fast64_t>> guaranteedCommandSets = storm::utility::counterexamples::getGuaranteedCommandSets(originalMdp, statesThatCanReachTargetStates, locallyRelevantCommands);
                
                // Search for states for which we could enable another option and possibly improve the reachability probability.
                std::set<boost::container::flat_set<uint_fast64_t>> cutCommands;
                for (auto state : reachableStates) {
                    for (auto currentChoice : relevancyInformation.relevantChoicesForRelevantStates.at(state)) {
                        if (!std::includes(commandSet.begin(), commandSet.end(), originalChoiceOrigins.getCommandSet(currentChoice).begin(), originalChoiceOrigins.getCommandSet(currentChoice).end())) {
                            boost::container::flat_set<uint_fast64_t> currentCommandSet;
                            for (auto command : originalChoiceOrigins.getCommandSet(currentChoice)) {
                                if (commandSet.find(command) == commandSet.end()) {
                                    currentCommandSet.insert(command);
                                }
                            }
                            std::set_difference(guaranteedCommandSets[state].begin(), guaranteedCommandSets[state].end(), commandSet.begin(), commandSet.end(), std::inserter(currentCommandSet, currentCommandSet.end()));
                            
                            cutCommands.insert(currentCommandSet);
                        }
                    }
                }
                
                // Given the results of the previous analysis, we construct the implications
                std::vector<storm::expressions::Expression> formulae;
                boost::container::flat_set<uint_fast64_t> unknownReachableCommands;
                std::set_difference(reachableCommands.begin(), reachableCommands.end(), relevancyInformation.knownCommands.begin(), relevancyInformation.knownCommands.end(), std::inserter(unknownReachableCommands, unknownReachableCommands.end()));
                for (auto command : unknownReachableCommands) {
                    formulae.push_back(!variableInformation.commandVariables.at(variableInformation.commandToIndexMap.at(command)));
                }
                for (auto const& cutCommandSet : cutCommands) {
                    storm::expressions::Expression cube = variableInformation.manager->boolean(true);
                    for (auto cutCommand : cutCommandSet) {
                        cube = cube && variableInformation.commandVariables.at(variableInformation.commandToIndexMap.at(cutCommand));
                    }
                    
                    formulae.push_back(cube);
                }
                
                STORM_LOG_DEBUG("Asserting reachability implications.");
                assertDisjunction(solver, formulae, *variableInformation.manager);
            }
#endif
            
        public:
            
            /*!
             * Returns the submdp obtained from removing all choices that do not originate from the specified commandset.
             */
            static storm::models::sparse::Mdp<T> restrictMdpToCommandSet(storm::models::sparse::Mdp<T> const& mdp, boost::container::flat_set<uint_fast64_t> const& enabledCommands) {
                STORM_LOG_THROW(mdp.hasChoiceOrigins(), storm::exceptions::InvalidArgumentException, "Restriction to command set is impossible for model without choice origins.");
                STORM_LOG_THROW(mdp.getChoiceOrigins()->isPrismChoiceOrigins(), storm::exceptions::InvalidArgumentException, "Restriction to command set is impossible for model without prism choice origins.");

                storm::storage::sparse::PrismChoiceOrigins const& prismChoiceOrigins = mdp.getChoiceOrigins()->asPrismChoiceOrigins();

                storm::storage::SparseMatrixBuilder<T> transitionMatrixBuilder(0, mdp.getTransitionMatrix().getColumnCount(), 0, true, true, mdp.getTransitionMatrix().getRowGroupCount());
                std::vector<uint_fast64_t> subMdpChoiceIndexMapping;

                // Check for each choice of each state, whether the choice commands are fully contained in the given command set.
                uint_fast64_t currentRow = 0;
                for(uint_fast64_t state = 0; state < mdp.getNumberOfStates(); ++state) {
                    bool stateHasValidChoice = false;
                    for (uint_fast64_t choice = mdp.getTransitionMatrix().getRowGroupIndices()[state]; choice < mdp.getTransitionMatrix().getRowGroupIndices()[state + 1]; ++choice) {
                        bool choiceValid = std::includes(enabledCommands.begin(), enabledCommands.end(), prismChoiceOrigins.getCommandSet(choice).begin(), prismChoiceOrigins.getCommandSet(choice).end());

                        // If the choice is valid, copy over all its elements.
                        if (choiceValid) {
                            if (!stateHasValidChoice) {
                                transitionMatrixBuilder.newRowGroup(currentRow);
                            }
                            stateHasValidChoice = true;
                            for (auto const& entry : mdp.getTransitionMatrix().getRow(choice)) {
                                transitionMatrixBuilder.addNextValue(currentRow, entry.getColumn(), entry.getValue());
                            }
                            subMdpChoiceIndexMapping.push_back(choice);
                            ++currentRow;
                        }
                    }

                    // If no choice of the current state may be taken, we insert a self-loop to the state instead.
                    if (!stateHasValidChoice) {
                        transitionMatrixBuilder.newRowGroup(currentRow);
                        transitionMatrixBuilder.addNextValue(currentRow, state, storm::utility::one<T>());
                        subMdpChoiceIndexMapping.push_back(std::numeric_limits<uint_fast64_t>::max());
                        ++currentRow;
                    }
                }
                
                storm::storage::sparse::ModelComponents<T> resultComponents(transitionMatrixBuilder.build());
                resultComponents.stateLabeling = mdp.getStateLabeling();
                resultComponents.choiceOrigins = prismChoiceOrigins.selectChoices(subMdpChoiceIndexMapping);
                return storm::models::sparse::Mdp<T>(std::move(resultComponents));
            }

            /*!
             * Computes the minimal command set that is needed in the given MDP to exceed the given probability threshold for satisfying phi until psi.
             *
             * @param program The program that was used to build the MDP.
             * @param mdp The MDP in which to find the minimal command set.
             * @param phiStates A bit vector characterizing all phi states in the model.
             * @param psiStates A bit vector characterizing all psi states in the model.
             * @param probabilityThreshold The probability value that must be achieved or exceeded.
             * @param strictBound A flag indicating whether the probability must be achieved (in which case the flag must be set) or strictly exceeded
             * (if the flag is set to false).
             * @param checkThresholdFeasible If set, it is verified that the model can actually achieve/exceed the given probability value. If this check
             * is made and fails, an exception is thrown.
             */
            static boost::container::flat_set<uint_fast64_t> getMinimalCommandSet(storm::prism::Program program, storm::models::sparse::Mdp<T> const& mdp, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, double probabilityThreshold, bool strictBound, bool checkThresholdFeasible = false, bool includeReachabilityEncoding = false) {
#ifdef STORM_HAVE_Z3
                // Set up all clocks used for time measurement.
                auto totalClock = std::chrono::high_resolution_clock::now();
                auto localClock = std::chrono::high_resolution_clock::now();
                decltype(std::chrono::high_resolution_clock::now() - totalClock) totalTime(0);
                
                auto setupTimeClock = std::chrono::high_resolution_clock::now();
                decltype(std::chrono::high_resolution_clock::now() - setupTimeClock) totalSetupTime(0);
                
                auto solverClock = std::chrono::high_resolution_clock::now();
                decltype(std::chrono::high_resolution_clock::now() - solverClock) totalSolverTime(0);
                
                auto modelCheckingClock = std::chrono::high_resolution_clock::now();
                decltype(std::chrono::high_resolution_clock::now() - modelCheckingClock) totalModelCheckingTime(0);

                auto analysisClock = std::chrono::high_resolution_clock::now();
                decltype(std::chrono::high_resolution_clock::now() - analysisClock) totalAnalysisTime(0);

                STORM_LOG_THROW(mdp.hasChoiceOrigins() && mdp.getChoiceOrigins()->isPrismChoiceOrigins(), storm::exceptions::InvalidArgumentException, "Restriction to Prism command set is impossible for model without Prism choice origins.");
                
                // (1) Check whether its possible to exceed the threshold if checkThresholdFeasible is set.
                double maximalReachabilityProbability = 0;
                if (checkThresholdFeasible) {
                    storm::modelchecker::helper::SparseMdpPrctlHelper<T> modelCheckerHelper;
                    STORM_LOG_DEBUG("Invoking model checker.");
                    std::vector<T> result = std::move(modelCheckerHelper.computeUntilProbabilities(false, mdp.getTransitionMatrix(), mdp.getBackwardTransitions(), phiStates, psiStates, false, false, storm::solver::GeneralMinMaxLinearEquationSolverFactory<T>()).values);
                    for (auto state : mdp.getInitialStates()) {
                        maximalReachabilityProbability = std::max(maximalReachabilityProbability, result[state]);
                    }
                    STORM_LOG_THROW((strictBound && maximalReachabilityProbability >= probabilityThreshold) || (!strictBound && maximalReachabilityProbability > probabilityThreshold), storm::exceptions::InvalidArgumentException, "Given probability threshold " << probabilityThreshold << " can not be " << (strictBound ? "achieved" : "exceeded") << " in model with maximal reachability probability of " << maximalReachabilityProbability << ".");
                    std::cout << std::endl << "Maximal reachability in model is " << maximalReachabilityProbability << "." << std::endl << std::endl;
                }
                
                // (2) Identify all states and commands that are relevant, because only these need to be considered later.
                RelevancyInformation relevancyInformation = determineRelevantStatesAndCommands(mdp, phiStates, psiStates);
                
                // (3) Create a solver.
                std::shared_ptr<storm::expressions::ExpressionManager> manager(new storm::expressions::ExpressionManager());
                std::unique_ptr<storm::solver::SmtSolver> solver(new storm::solver::Z3SmtSolver(*manager));
                
                // (4) Create the variables for the relevant commands.
                VariableInformation variableInformation = createVariables(manager, mdp, psiStates, relevancyInformation, includeReachabilityEncoding);
                STORM_LOG_DEBUG("Created variables.");

                // (5) Now assert an adder whose result variables can later be used to constrain the nummber of command
                // variables that were set to true. Initially, we are looking for a solution that has no command enabled
                // and subsequently relax that.
                variableInformation.adderVariables = assertAdder(*solver, variableInformation);
                variableInformation.auxiliaryVariables.push_back(assertLessOrEqualKRelaxed(*solver, variableInformation, 0));
                
                // (6) Add constraints that cut off a lot of suboptimal solutions.
                STORM_LOG_DEBUG("Asserting cuts.");
                assertExplicitCuts(mdp, psiStates, variableInformation, relevancyInformation, *solver);
                STORM_LOG_DEBUG("Asserted explicit cuts.");
                assertSymbolicCuts(program, mdp, variableInformation, relevancyInformation, *solver);
                STORM_LOG_DEBUG("Asserted symbolic cuts.");
                if (includeReachabilityEncoding) {
                    assertReachabilityCuts(mdp, psiStates, variableInformation, relevancyInformation, *solver);
                    STORM_LOG_DEBUG("Asserted reachability cuts.");
                }
                
                // As we are done with the setup at this point, stop the clock for the setup time.
                totalSetupTime = std::chrono::high_resolution_clock::now() - setupTimeClock;
                
                // (7) Find the smallest set of commands that satisfies all constraints. If the probability of
                // satisfying phi until psi exceeds the given threshold, the set of commands is minimal and can be returned.
                // Otherwise, the current solution has to be ruled out and the next smallest solution is retrieved from
                // the solver.
                
                // Set up some variables for the iterations.
                boost::container::flat_set<uint_fast64_t> commandSet(relevancyInformation.relevantCommands);
                bool done = false;
                uint_fast64_t iterations = 0;
                uint_fast64_t currentBound = 0;
                maximalReachabilityProbability = 0;
                uint_fast64_t zeroProbabilityCount = 0;
                do {
                    STORM_LOG_DEBUG("Computing minimal command set.");
                    solverClock = std::chrono::high_resolution_clock::now();
                    commandSet = findSmallestCommandSet(*solver, variableInformation, currentBound);
                    totalSolverTime += std::chrono::high_resolution_clock::now() - solverClock;
                    STORM_LOG_DEBUG("Computed minimal command set of size " << (commandSet.size() + relevancyInformation.knownCommands.size()) << ".");
                    
                    // Restrict the given MDP to the current set of commands and compute the reachability probability.
                    modelCheckingClock = std::chrono::high_resolution_clock::now();
                    commandSet.insert(relevancyInformation.knownCommands.begin(), relevancyInformation.knownCommands.end());
                    storm::models::sparse::Mdp<T> const& subMdp = restrictMdpToCommandSet(mdp, commandSet);
                    STORM_LOG_THROW(subMdp.hasChoiceOrigins() && subMdp.getChoiceOrigins()->isPrismChoiceOrigins(), storm::exceptions::InvalidArgumentException, "Expected prism choice origins for submodel.");
                
                    storm::modelchecker::helper::SparseMdpPrctlHelper<T> modelCheckerHelper;
                    STORM_LOG_DEBUG("Invoking model checker.");
                    std::vector<T> result = std::move(modelCheckerHelper.computeUntilProbabilities(false, subMdp.getTransitionMatrix(), subMdp.getBackwardTransitions(), phiStates, psiStates, false, false, storm::solver::GeneralMinMaxLinearEquationSolverFactory<T>()).values);
                    STORM_LOG_DEBUG("Computed model checking results.");
                    totalModelCheckingTime += std::chrono::high_resolution_clock::now() - modelCheckingClock;

                    // Now determine the maximal reachability probability by checking all initial states.
                    maximalReachabilityProbability = 0;
                    for (auto state : mdp.getInitialStates()) {
                        maximalReachabilityProbability = std::max(maximalReachabilityProbability, result[state]);
                    }
                    
                    // Depending on whether the threshold was successfully achieved or not, we proceed by either analyzing the bad solution or stopping the iteration process.
                    analysisClock = std::chrono::high_resolution_clock::now();
                    if ((strictBound && maximalReachabilityProbability < probabilityThreshold) || (!strictBound && maximalReachabilityProbability <= probabilityThreshold)) {
                        if (maximalReachabilityProbability == 0) {
                            ++zeroProbabilityCount;
                            
                            // If there was no target state reachable, analyze the solution and guide the solver into the right direction.
                            analyzeZeroProbabilitySolution(*solver, subMdp, mdp, phiStates, psiStates, commandSet, variableInformation, relevancyInformation);
                        } else {
                            // If the reachability probability was greater than zero (i.e. there is a reachable target state), but the probability was insufficient to exceed
                            // the given threshold, we analyze the solution and try to guide the solver into the right direction.
                            analyzeInsufficientProbabilitySolution(*solver, subMdp, mdp, phiStates, psiStates, commandSet, variableInformation, relevancyInformation);
                        }
                    } else {
                        done = true;
                    }
                    totalAnalysisTime += (std::chrono::high_resolution_clock::now() - analysisClock);
                    ++iterations;
                    
                    if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - localClock).count() >= 5) {
                        std::cout << "Checked " << iterations << " models in " << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - totalClock).count() << "s (out of which " << zeroProbabilityCount << " could not reach the target states). Current command set size is " << commandSet.size() << "." << std::endl;
                        localClock = std::chrono::high_resolution_clock::now();
                    }
                } while (!done);

                // Compute and emit the time measurements if the corresponding flag was set.
                totalTime = std::chrono::high_resolution_clock::now() - totalClock;
                if (storm::settings::getModule<storm::settings::modules::CoreSettings>().isShowStatisticsSet()) {
                    std::cout << std::endl;
                    std::cout << "Time breakdown:" << std::endl;
                    std::cout << "    * time for setup: " << std::chrono::duration_cast<std::chrono::milliseconds>(totalSetupTime).count() << "ms" << std::endl;
                    std::cout << "    * time for solving: " << std::chrono::duration_cast<std::chrono::milliseconds>(totalSolverTime).count() << "ms" << std::endl;
                    std::cout << "    * time for checking: " << std::chrono::duration_cast<std::chrono::milliseconds>(totalModelCheckingTime).count() << "ms" << std::endl;
                    std::cout << "    * time for analysis: " << std::chrono::duration_cast<std::chrono::milliseconds>(totalAnalysisTime).count() << "ms" << std::endl;
                    std::cout << "------------------------------------------" << std::endl;
                    std::cout << "    * total time: " << std::chrono::duration_cast<std::chrono::milliseconds>(totalTime).count() << "ms" << std::endl;
                    std::cout << std::endl;
                    std::cout << "Other:" << std::endl;
                    std::cout << "    * number of models checked: " << iterations << std::endl;
                    std::cout << "    * number of models that could not reach a target state: " << zeroProbabilityCount << " (" << 100 * static_cast<double>(zeroProbabilityCount)/iterations << "%)" << std::endl << std::endl;
                }

                
                return commandSet;
#else
                throw storm::exceptions::NotImplementedException() << "This functionality is unavailable since storm has been compiled without support for Z3.";
#endif
            }
            
            static std::shared_ptr<PrismHighLevelCounterexample> computeCounterexample(storm::prism::Program program, storm::models::sparse::Mdp<T> const& mdp, std::shared_ptr<storm::logic::Formula const> const& formula) {
#ifdef STORM_HAVE_Z3
                std::cout << std::endl << "Generating minimal command set counterexample for formula " << *formula << std::endl;
                
                STORM_LOG_THROW(formula->isProbabilityOperatorFormula(), storm::exceptions::InvalidPropertyException, "Counterexample generation does not support this kind of formula. Expecting a probability operator as the outermost formula element.");
                storm::logic::ProbabilityOperatorFormula const& probabilityOperator = formula->asProbabilityOperatorFormula();
                STORM_LOG_THROW(probabilityOperator.hasBound(), storm::exceptions::InvalidPropertyException, "Counterexample generation only supports bounded formulas.");
                storm::logic::ComparisonType comparisonType = probabilityOperator.getComparisonType();
                STORM_LOG_THROW(comparisonType == storm::logic::ComparisonType::Less || comparisonType == storm::logic::ComparisonType::LessEqual, storm::exceptions::InvalidPropertyException, "Counterexample generation only supports formulas with an upper probability bound.");
                STORM_LOG_THROW(probabilityOperator.getSubformula().isUntilFormula() || probabilityOperator.getSubformula().isEventuallyFormula(), storm::exceptions::InvalidPropertyException, "Path formula is required to be of the form 'phi U psi' for counterexample generation.");
                
                bool strictBound = comparisonType == storm::logic::ComparisonType::Less;
                double threshold = probabilityOperator.getThresholdAs<double>();
                
                storm::storage::BitVector phiStates;
                storm::storage::BitVector psiStates;
                storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<T>> modelchecker(mdp);
                
                if (probabilityOperator.getSubformula().isUntilFormula()) {
                    storm::logic::UntilFormula const& untilFormula = probabilityOperator.getSubformula().asUntilFormula();
                    
                    std::unique_ptr<storm::modelchecker::CheckResult> leftResult = modelchecker.check(untilFormula.getLeftSubformula());
                    std::unique_ptr<storm::modelchecker::CheckResult> rightResult = modelchecker.check(untilFormula.getRightSubformula());
                    
                    storm::modelchecker::ExplicitQualitativeCheckResult const& leftQualitativeResult = leftResult->asExplicitQualitativeCheckResult();
                    storm::modelchecker::ExplicitQualitativeCheckResult const& rightQualitativeResult = rightResult->asExplicitQualitativeCheckResult();
                    
                    phiStates = leftQualitativeResult.getTruthValuesVector();
                    psiStates = rightQualitativeResult.getTruthValuesVector();
                } else if (probabilityOperator.getSubformula().isEventuallyFormula()) {
                    storm::logic::EventuallyFormula const& eventuallyFormula = probabilityOperator.getSubformula().asEventuallyFormula();
                    
                    std::unique_ptr<storm::modelchecker::CheckResult> subResult = modelchecker.check(eventuallyFormula.getSubformula());
                    
                    storm::modelchecker::ExplicitQualitativeCheckResult const& subQualitativeResult = subResult->asExplicitQualitativeCheckResult();
                    
                    phiStates = storm::storage::BitVector(mdp.getNumberOfStates(), true);
                    psiStates = subQualitativeResult.getTruthValuesVector();
                }
                
                // Delegate the actual computation work to the function of equal name.
                auto startTime = std::chrono::high_resolution_clock::now();
                auto commandSet = getMinimalCommandSet(program, mdp, phiStates, psiStates, threshold, strictBound, true, storm::settings::getModule<storm::settings::modules::CounterexampleGeneratorSettings>().isEncodeReachabilitySet());
                auto endTime = std::chrono::high_resolution_clock::now();
                std::cout << std::endl << "Computed minimal command set of size " << commandSet.size() << " in " << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << "ms." << std::endl;
                
                return std::make_shared<PrismHighLevelCounterexample>(program.restrictCommands(commandSet));
#else
                throw storm::exceptions::NotImplementedException() << "This functionality is unavailable since storm has been compiled without support for Z3.";
#endif
            }
            
        };
        
    } // namespace counterexamples
} // namespace storm

#endif /* STORM_COUNTEREXAMPLES_SMTMINIMALCOMMANDSETGENERATOR_MDP_H_ */
