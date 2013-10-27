/*
 * SMTMinimalCommandSetGenerator.h
 *
 *  Created on: 01.10.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_COUNTEREXAMPLES_SMTMINIMALCOMMANDSETGENERATOR_MDP_H_
#define STORM_COUNTEREXAMPLES_SMTMINIMALCOMMANDSETGENERATOR_MDP_H_

#include <queue>
#include <chrono>

// To detect whether the usage of Z3 is possible, this include is neccessary.
#include "storm-config.h"

// If we have Z3 available, we have to include the C++ header.
#ifdef STORM_HAVE_Z3
#include "z3++.h"
#include "src/adapters/Z3ExpressionAdapter.h"
#endif

#include "src/adapters/ExplicitModelAdapter.h"
#include "src/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "src/solver/GmmxxNondeterministicLinearEquationSolver.h"

#include "src/utility/counterexamples.h"
#include "src/utility/IRUtility.h"

namespace storm {
    namespace counterexamples {
        
        /*!
         * This class provides functionality to generate a minimal counterexample to a probabilistic reachability
         * property in terms of used labels.
         */
        template <class T>
        class SMTMinimalCommandSetGenerator {
#ifdef STORM_HAVE_Z3
        private:
            struct RelevancyInformation {
                // The set of relevant states in the model.
                storm::storage::BitVector relevantStates;
                
                // The set of relevant labels.
                storm::storage::VectorSet<uint_fast64_t> relevantLabels;
                
                // A set of labels that is definitely known to be taken in the final solution.
                storm::storage::VectorSet<uint_fast64_t> knownLabels;
                
                // A list of relevant choices for each relevant state.
                std::map<uint_fast64_t, std::list<uint_fast64_t>> relevantChoicesForRelevantStates;
            };
            
            struct VariableInformation {
                // The variables associated with the relevant labels.
                std::vector<z3::expr> labelVariables;
                
                // A mapping from relevant labels to their indices in the variable vector.
                std::map<uint_fast64_t, uint_fast64_t> labelToIndexMap;

                // A set of original auxiliary variables needed for the Fu-Malik procedure.
                std::vector<z3::expr> originalAuxiliaryVariables;
                
                // A set of auxiliary variables that may be modified by the MaxSAT procedure.
                std::vector<z3::expr> auxiliaryVariables;
                
                // A vector of variables that can be used to constrain the number of variables that are set to true.
                std::vector<z3::expr> adderVariables;
            };
            
            /*!
             * Computes the set of relevant labels in the model. Relevant labels are choice labels such that there exists
             * a scheduler that satisfies phi until psi with a nonzero probability.
             *
             * @param labeledMdp The MDP to search for relevant labels.
             * @param phiStates A bit vector representing all states that satisfy phi.
             * @param psiStates A bit vector representing all states that satisfy psi.
             * @return A structure containing the relevant labels as well as states.
             */
            static RelevancyInformation determineRelevantStatesAndLabels(storm::models::Mdp<T> const& labeledMdp, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates) {
                // Create result.
                RelevancyInformation relevancyInformation;
                
                // Compute all relevant states, i.e. states for which there exists a scheduler that has a non-zero
                // probabilitiy of satisfying phi until psi.
                storm::storage::SparseMatrix<bool> backwardTransitions = labeledMdp.getBackwardTransitions();
                relevancyInformation.relevantStates = storm::utility::graph::performProbGreater0E(labeledMdp, backwardTransitions, phiStates, psiStates);
                relevancyInformation.relevantStates &= ~psiStates;

                LOG4CPLUS_DEBUG(logger, "Found " << relevancyInformation.relevantStates.getNumberOfSetBits() << " relevant states.");
                LOG4CPLUS_DEBUG(logger, relevancyInformation.relevantStates.toString());

                // Retrieve some references for convenient access.
                storm::storage::SparseMatrix<T> const& transitionMatrix = labeledMdp.getTransitionMatrix();
                std::vector<uint_fast64_t> const& nondeterministicChoiceIndices = labeledMdp.getNondeterministicChoiceIndices();
                std::vector<storm::storage::VectorSet<uint_fast64_t>> const& choiceLabeling = labeledMdp.getChoiceLabeling();

                // Now traverse all choices of all relevant states and check whether there is a successor target state.
                // If so, the associated labels become relevant. Also, if a choice of relevant state has at least one
                // relevant successor, the choice becomes relevant.
                for (auto state : relevancyInformation.relevantStates) {
                    relevancyInformation.relevantChoicesForRelevantStates.emplace(state, std::list<uint_fast64_t>());
                    
                    for (uint_fast64_t row = nondeterministicChoiceIndices[state]; row < nondeterministicChoiceIndices[state + 1]; ++row) {
                        bool currentChoiceRelevant = false;

                        for (typename storm::storage::SparseMatrix<T>::ConstIndexIterator successorIt = transitionMatrix.constColumnIteratorBegin(row); successorIt != transitionMatrix.constColumnIteratorEnd(row); ++successorIt) {
                            // If there is a relevant successor, we need to add the labels of the current choice.
                            if (relevancyInformation.relevantStates.get(*successorIt) || psiStates.get(*successorIt)) {
                                for (auto const& label : choiceLabeling[row]) {
                                    relevancyInformation.relevantLabels.insert(label);
                                }
                                if (!currentChoiceRelevant) {
                                    currentChoiceRelevant = true;
                                    relevancyInformation.relevantChoicesForRelevantStates[state].push_back(row);
                                }
                            }
                        }
                    }
                }
                
                // Compute the set of labels that are known to be taken in any case.
                relevancyInformation.knownLabels = storm::utility::counterexamples::getGuaranteedLabelSet(labeledMdp, psiStates, relevancyInformation.relevantLabels);
                if (!relevancyInformation.knownLabels.empty()) {
                    storm::storage::VectorSet<uint_fast64_t> remainingLabels;
                    std::set_difference(relevancyInformation.relevantLabels.begin(), relevancyInformation.relevantLabels.end(), relevancyInformation.knownLabels.begin(), relevancyInformation.knownLabels.end(), std::inserter(remainingLabels, remainingLabels.end()));
                    relevancyInformation.relevantLabels = remainingLabels;
                }
                
                std::cout << "Found " << relevancyInformation.relevantLabels.size() << " relevant and " << relevancyInformation.knownLabels.size() << " known labels." << std::endl;

                LOG4CPLUS_DEBUG(logger, "Found " << relevancyInformation.relevantLabels.size() << " relevant and " << relevancyInformation.knownLabels.size() << " known labels.");
                return relevancyInformation;
            }
            
            /*!
             * Creates all necessary base expressions for the relevant labels.
             *
             * @param context The Z3 context in which to create the expressions.
             * @param relevantCommands A set of relevant labels for which to create the expressions.
             * @return A mapping from relevant labels to their corresponding expressions.
             */
            static VariableInformation createExpressionsForRelevantLabels(z3::context& context, storm::storage::VectorSet<uint_fast64_t> const& relevantLabels) {
                VariableInformation variableInformation;
                
                // Create stringstream to build expression names.
                std::stringstream variableName;
                
                for (auto label : relevantLabels) {                    
                    variableInformation.labelToIndexMap[label] = variableInformation.labelVariables.size();
                    
                    // Clear contents of the stream to construct new expression name.
                    variableName.clear();
                    variableName.str("");
                    variableName << "c" << label;
                    
                    variableInformation.labelVariables.push_back(context.bool_const(variableName.str().c_str()));
                    
                    // Clear contents of the stream to construct new expression name.
                    variableName.clear();
                    variableName.str("");
                    variableName << "h" << label;
                    
                    variableInformation.originalAuxiliaryVariables.push_back(context.bool_const(variableName.str().c_str()));
                }
                
                return variableInformation;
            }

            /*!
             * Asserts the constraints that are initially needed for the Fu-Malik procedure.
             *
             * @param program The program for which to build the constraints.
             * @param labeledMdp The MDP that results from the given program.
             * @param context The Z3 context in which to build the expressions.
             * @param solver The solver in which to assert the constraints.
             * @param variableInformation A structure with information about the variables for the labels.
             */
            static void assertFuMalikInitialConstraints(storm::ir::Program const& program, storm::models::Mdp<T> const& labeledMdp, storm::storage::BitVector const& psiStates, z3::context& context, z3::solver& solver, VariableInformation const& variableInformation, RelevancyInformation const& relevancyInformation) {
                // Assert that at least one of the labels must be taken.
                z3::expr formula = variableInformation.labelVariables.at(0);
                for (uint_fast64_t index = 1; index < variableInformation.labelVariables.size(); ++index) {
                    formula = formula || variableInformation.labelVariables.at(index);
                }
                solver.add(formula);
            }
            
            /*!
             * Asserts cuts that are derived from the explicit representation of the model and rule out a lot of
             * suboptimal solutions.
             *
             * @param labeledMdp The labeled MDP for which to compute the cuts.
             * @param context The Z3 context in which to build the expressions.
             * @param solver The solver to use for the satisfiability evaluation.
             */
            static void assertExplicitCuts(storm::models::Mdp<T> const& labeledMdp, storm::storage::BitVector const& psiStates, VariableInformation const& variableInformation, RelevancyInformation const& relevancyInformation, z3::context& context, z3::solver& solver) {
                // Walk through the MDP and
                // * identify labels enabled in initial states
                // * identify labels that can directly precede a given action
                // * identify labels that directly reach a target state
                // * identify labels that can directly follow a given action
                
                storm::storage::VectorSet<uint_fast64_t> initialLabels;
                std::set<storm::storage::VectorSet<uint_fast64_t>> initialCombinations;
                std::map<uint_fast64_t, storm::storage::VectorSet<uint_fast64_t>> precedingLabels;
                storm::storage::VectorSet<uint_fast64_t> targetLabels;
                storm::storage::VectorSet<storm::storage::VectorSet<uint_fast64_t>> targetCombinations;
                std::map<storm::storage::VectorSet<uint_fast64_t>, std::set<storm::storage::VectorSet<uint_fast64_t>>> followingLabels;
                std::map<uint_fast64_t, std::set<storm::storage::VectorSet<uint_fast64_t>>> synchronizingLabels;
                
                // Get some data from the MDP for convenient access.
                storm::storage::SparseMatrix<T> const& transitionMatrix = labeledMdp.getTransitionMatrix();
                std::vector<uint_fast64_t> const& nondeterministicChoiceIndices = labeledMdp.getNondeterministicChoiceIndices();
                storm::storage::BitVector const& initialStates = labeledMdp.getInitialStates();
                std::vector<storm::storage::VectorSet<uint_fast64_t>> const& choiceLabeling = labeledMdp.getChoiceLabeling();
                storm::storage::SparseMatrix<bool> backwardTransitions = labeledMdp.getBackwardTransitions();
                
                for (auto currentState : relevancyInformation.relevantStates) {
                    for (auto currentChoice : relevancyInformation.relevantChoicesForRelevantStates.at(currentState)) {
                        
                        // If the choice is a synchronization choice, we need to record it.
                        if (choiceLabeling[currentChoice].size() > 1) {
                            for (auto label : choiceLabeling[currentChoice]) {
                                synchronizingLabels[label].emplace(choiceLabeling[currentChoice]);
                            }
                        }
                        
                        // If the state is initial, we need to add all the choice labels to the initial label set.
                        if (initialStates.get(currentState)) {
                            initialLabels.insert(choiceLabeling[currentChoice].begin(), choiceLabeling[currentChoice].end());
                            initialCombinations.insert(choiceLabeling[currentChoice]);
                        }
                        
                        // Iterate over successors and add relevant choices of relevant successors to the following label set.
                        bool canReachTargetState = false;
                        for (typename storm::storage::SparseMatrix<T>::ConstIndexIterator successorIt = transitionMatrix.constColumnIteratorBegin(currentChoice), successorIte = transitionMatrix.constColumnIteratorEnd(currentChoice); successorIt != successorIte; ++successorIt) {
                            if (relevancyInformation.relevantStates.get(*successorIt)) {
                                for (auto relevantChoice : relevancyInformation.relevantChoicesForRelevantStates.at(*successorIt)) {
                                    followingLabels[choiceLabeling[currentChoice]].insert(choiceLabeling[currentChoice]);
                                }
                            } else if (psiStates.get(*successorIt)) {
                                canReachTargetState = true;
                            }
                        }
                        
                        // If the choice can reach a target state directly, we add all the labels to the target label set.
                        if (canReachTargetState) {
                            targetLabels.insert(choiceLabeling[currentChoice].begin(), choiceLabeling[currentChoice].end());
                            targetCombinations.insert(choiceLabeling[currentChoice]);
                        }
                    }
                    
                    // Iterate over predecessors and add all choices that target the current state to the preceding
                    // label set of all labels of all relevant choices of the current state.
                    for (typename storm::storage::SparseMatrix<T>::ConstIndexIterator predecessorIt = backwardTransitions.constColumnIteratorBegin(currentState), predecessorIte = backwardTransitions.constColumnIteratorEnd(currentState); predecessorIt != predecessorIte; ++predecessorIt) {
                        if (relevancyInformation.relevantStates.get(*predecessorIt)) {
                            for (auto predecessorChoice : relevancyInformation.relevantChoicesForRelevantStates.at(*predecessorIt)) {
                                bool choiceTargetsCurrentState = false;
                                for (typename storm::storage::SparseMatrix<T>::ConstIndexIterator successorIt = transitionMatrix.constColumnIteratorBegin(predecessorChoice), successorIte = transitionMatrix.constColumnIteratorEnd(predecessorChoice); successorIt != successorIte; ++successorIt) {
                                    if (*successorIt == currentState) {
                                        choiceTargetsCurrentState = true;
                                    }
                                }
                                
                                if (choiceTargetsCurrentState) {
                                    for (auto currentChoice : relevancyInformation.relevantChoicesForRelevantStates.at(currentState)) {
                                        for (auto labelToAdd : choiceLabeling[predecessorChoice]) {
                                            for (auto labelForWhichToAdd : choiceLabeling[currentChoice]) {
                                                precedingLabels[labelForWhichToAdd].insert(labelToAdd);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                
                LOG4CPLUS_DEBUG(logger, "Successfully gathered data for explicit cuts.");
                
                LOG4CPLUS_DEBUG(logger, "Asserting initial combination is taken.");
                {
                    std::vector<z3::expr> formulae;
                    
                    // Start by asserting that we take at least one initial label. We may do so only if there is no initial
                    // combination that is already known. Otherwise this condition would be too strong.
                    bool initialCombinationKnown = false;
                    for (auto const& combination : initialCombinations) {
                        storm::storage::VectorSet<uint_fast64_t> tmpSet;
                        std::set_difference(combination.begin(), combination.end(), relevancyInformation.knownLabels.begin(), relevancyInformation.knownLabels.end(), std::inserter(tmpSet, tmpSet.end()));
                        if (tmpSet.size() == 0) {
                            initialCombinationKnown = true;
                            break;
                        } else {
                            z3::expr conj = context.bool_val(true);
                            for (auto label : tmpSet) {
                                conj = conj && variableInformation.labelVariables.at(variableInformation.labelToIndexMap.at(label));
                            }
                            formulae.push_back(conj);
                        }
                    }
                    if (!initialCombinationKnown) {
                        assertDisjunction(context, solver, formulae);
                    }
                }
                
                LOG4CPLUS_DEBUG(logger, "Asserting target combination is taken.");
                {
                    std::vector<z3::expr> formulae;

                    // Likewise, if no target combination is known, we may assert that there is at least one.
                    bool targetCombinationKnown = false;
                    for (auto const& combination : targetCombinations) {
                        storm::storage::VectorSet<uint_fast64_t> tmpSet;
                        std::set_difference(combination.begin(), combination.end(), relevancyInformation.knownLabels.begin(), relevancyInformation.knownLabels.end(), std::inserter(tmpSet, tmpSet.end()));
                        if (tmpSet.size() == 0) {
                            targetCombinationKnown = true;
                            break;
                        } else {
                            z3::expr conj = context.bool_val(true);
                            for (auto label : tmpSet) {
                                conj = conj && variableInformation.labelVariables.at(variableInformation.labelToIndexMap.at(label));
                            }
                            formulae.push_back(conj);
                        }
                    }
                    if (!targetCombinationKnown) {
                        assertDisjunction(context, solver, formulae);
                    }
                }
                
                // Compute the sets of labels such that the transitions labeled with this set possess at least one known successor.
                storm::storage::VectorSet<storm::storage::VectorSet<uint_fast64_t>> hasKnownSuccessor;
                for (auto const& labelSetFollowingSetsPair : followingLabels) {
                    for (auto const& set : labelSetFollowingSetsPair.second) {
                        if (set.subsetOf(relevancyInformation.knownLabels)) {
                            hasKnownSuccessor.insert(set);
                            break;
                        }
                    }
                }
                
                LOG4CPLUS_DEBUG(logger, "Asserting taken labels are followed by another label if they are not a target label.");
                // Now assert that for each non-target label, we take a following label.
                for (auto const& labelSetFollowingSetsPair : followingLabels) {
                    std::vector<z3::expr> formulae;
                    
                    // Only build a constraint if the combination does not lead to a target state and
                    // no successor set is already known.
                    if (!targetCombinations.contains(labelSetFollowingSetsPair.first) && !hasKnownSuccessor.contains(labelSetFollowingSetsPair.first)) {
                    
                        // Compute the set of unknown labels on the left-hand side of the implication.
                        storm::storage::VectorSet<uint_fast64_t> unknownLhsLabels;
                        std::set_difference(labelSetFollowingSetsPair.first.begin(), labelSetFollowingSetsPair.first.end(), relevancyInformation.knownLabels.begin(), relevancyInformation.knownLabels.end(), std::inserter(unknownLhsLabels, unknownLhsLabels.end()));
                        for (auto label : unknownLhsLabels) {
                            formulae.push_back(!variableInformation.labelVariables.at(variableInformation.labelToIndexMap.at(label)));
                        }
                        
                        for (auto const& followingSet : labelSetFollowingSetsPair.second) {
                            storm::storage::VectorSet<uint_fast64_t> tmpSet;
                            
                            // Check which labels of the current following set are not known. This set must be non-empty, because
                            // otherwise a successor combination would already be known and control cannot reach this point.
                            std::set_difference(followingSet.begin(), followingSet.end(), relevancyInformation.knownLabels.begin(), relevancyInformation.knownLabels.end(), std::inserter(tmpSet, tmpSet.end()));
                            
                            // Construct an expression that enables all unknown labels of the current following set.
                            z3::expr conj = context.bool_val(true);
                            for (auto label : tmpSet) {
                                conj = conj && variableInformation.labelVariables.at(variableInformation.labelToIndexMap.at(label));
                            }
                            formulae.push_back(conj);
                        }
                        
                        if (labelSetFollowingSetsPair.first.size() > 1) {
                            // Taking all commands of a combination does not necessarily mean that a following label set needs to be taken.
                            // This is because it could be that the commands are taken to enable other synchronizations. Therefore, we need
                            // to add an additional clause that says that the right-hand side of the implication is also true if all commands
                            // of the current choice have enabled synchronization options.
                            z3::expr finalDisjunct = context.bool_val(false);
                            for (auto label : labelSetFollowingSetsPair.first) {
                                z3::expr alternativeExpressionForLabel = context.bool_val(false);
                                std::set<storm::storage::VectorSet<uint_fast64_t>> const& synchsForCommand = synchronizingLabels.at(label);
                                
                                for (auto const& synchSet : synchsForCommand) {
                                    z3::expr alternativeExpression = context.bool_val(true);
                                    
                                    // If the current synchSet is the same as left-hand side of the implication, we need to skip it.
                                    if (synchSet == labelSetFollowingSetsPair.first) continue;
                                    
                                    // Now that we have the labels that are unknown and "missing", we still need to check whether this other
                                    // synchronizing set already has a known successor or leads directly to a target state.
                                    if (!hasKnownSuccessor.contains(synchSet) && !targetCombinations.contains(synchSet)) {
                                        // If not, we can assert that we take one of its possible successors.
                                        storm::storage::VectorSet<uint_fast64_t> unknownSynchs;
                                        std::set_difference(synchSet.begin(), synchSet.end(), relevancyInformation.knownLabels.begin(), relevancyInformation.knownLabels.end(), std::inserter(unknownSynchs, unknownSynchs.end()));
                                        unknownSynchs.erase(label);
                                        
                                        for (auto label : unknownSynchs) {
                                            alternativeExpression = alternativeExpression && variableInformation.labelVariables.at(variableInformation.labelToIndexMap.at(label));
                                        }
                                    
                                        z3::expr disjunctionOverSuccessors = context.bool_val(false);
                                        for (auto successorSet : followingLabels.at(synchSet)) {
                                            z3::expr conjunctionOverLabels = context.bool_val(true);
                                            for (auto label : successorSet) {
                                                if (!relevancyInformation.knownLabels.contains(label)) {
                                                    conjunctionOverLabels = conjunctionOverLabels && variableInformation.labelVariables.at(variableInformation.labelToIndexMap.at(label));
                                                }
                                            }
                                            disjunctionOverSuccessors = disjunctionOverSuccessors || conjunctionOverLabels;
                                        }
                                        
                                        alternativeExpression = alternativeExpression && disjunctionOverSuccessors;
                                    }
                                    
                                    alternativeExpressionForLabel = alternativeExpressionForLabel || alternativeExpression;
                                }
                                
                                finalDisjunct = finalDisjunct && alternativeExpressionForLabel;
                            }
                            
                            formulae.push_back(finalDisjunct);
                        }

                        if (formulae.size() > 0) {
                            assertDisjunction(context, solver, formulae);
                        }
                    }
                }
                
                LOG4CPLUS_DEBUG(logger, "Asserting synchronization cuts.");
                // Finally, assert that if we take one of the synchronizing labels, we also take one of the combinations
                // the label appears in.
                for (auto const& labelSynchronizingSetsPair : synchronizingLabels) {
                    std::vector<z3::expr> formulae;

                    if (!relevancyInformation.knownLabels.contains(labelSynchronizingSetsPair.first)) {
                        formulae.push_back(!variableInformation.labelVariables.at(variableInformation.labelToIndexMap.at(labelSynchronizingSetsPair.first)));
                    }
                    
                    // We need to be careful, because there may be one synchronisation set out of which all labels are
                    // known, which means we must not assert anything.
                    bool allImplicantsKnownForOneSet = false;
                    for (auto const& synchronizingSet : labelSynchronizingSetsPair.second) {
                        z3::expr currentCombination = context.bool_val(true);
                        bool allImplicantsKnownForCurrentSet = true;
                        for (auto label : synchronizingSet) {
                            if (!relevancyInformation.knownLabels.contains(label) && label != labelSynchronizingSetsPair.first) {
                                currentCombination = currentCombination && variableInformation.labelVariables.at(variableInformation.labelToIndexMap.at(label));
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
                        assertDisjunction(context, solver, formulae);
                    }
                }
            }

            /*!
             * Asserts cuts that are derived from the symbolic representation of the model and rule out a lot of
             * suboptimal solutions.
             *
             * @param program The symbolic representation of the model in terms of a program.
             * @param context The Z3 context in which to build the expressions.
             * @param solver The solver to use for the satisfiability evaluation.
             */
            static void assertSymbolicCuts(storm::ir::Program const& program, storm::models::Mdp<T> const& labeledMdp, VariableInformation const& variableInformation, RelevancyInformation const& relevancyInformation, z3::context& context, z3::solver& solver) {
                
                // A container storing the label sets that may precede a given label set.
                std::map<storm::storage::VectorSet<uint_fast64_t>, std::set<storm::storage::VectorSet<uint_fast64_t>>> precedingLabelSets;

                // A container that maps labels to their reachable synchronization sets.
                std::map<uint_fast64_t, std::set<storm::storage::VectorSet<uint_fast64_t>>> synchronizingLabels;

                // Get some data from the MDP for convenient access.
                storm::storage::SparseMatrix<T> const& transitionMatrix = labeledMdp.getTransitionMatrix();
                std::vector<storm::storage::VectorSet<uint_fast64_t>> const& choiceLabeling = labeledMdp.getChoiceLabeling();
                storm::storage::SparseMatrix<bool> backwardTransitions = labeledMdp.getBackwardTransitions();
                
                // Compute the set of labels that may precede a given action.
                for (auto currentState : relevancyInformation.relevantStates) {
                    for (auto currentChoice : relevancyInformation.relevantChoicesForRelevantStates.at(currentState)) {
                        
                        // If the choice is a synchronization choice, we need to record it.
                        if (choiceLabeling[currentChoice].size() > 1) {
                            for (auto label : choiceLabeling[currentChoice]) {
                                synchronizingLabels[label].emplace(choiceLabeling[currentChoice]);
                            }
                        }

                        // Iterate over predecessors and add all choices that target the current state to the preceding
                        // label set of all labels of all relevant choices of the current state.
                        for (typename storm::storage::SparseMatrix<T>::ConstIndexIterator predecessorIt = backwardTransitions.constColumnIteratorBegin(currentState), predecessorIte = backwardTransitions.constColumnIteratorEnd(currentState); predecessorIt != predecessorIte; ++predecessorIt) {
                            if (relevancyInformation.relevantStates.get(*predecessorIt)) {
                                for (auto predecessorChoice : relevancyInformation.relevantChoicesForRelevantStates.at(*predecessorIt)) {
                                    bool choiceTargetsCurrentState = false;
                                    for (typename storm::storage::SparseMatrix<T>::ConstIndexIterator successorIt = transitionMatrix.constColumnIteratorBegin(predecessorChoice), successorIte = transitionMatrix.constColumnIteratorEnd(predecessorChoice); successorIt != successorIte; ++successorIt) {
                                        if (*successorIt == currentState) {
                                            choiceTargetsCurrentState = true;
                                        }
                                    }
                                    
                                    if (choiceTargetsCurrentState) {
                                        precedingLabelSets[choiceLabeling.at(currentChoice)].insert(choiceLabeling.at(predecessorChoice));
                                    }
                                }
                            }
                        }
                    }
                }
                
                storm::utility::ir::VariableInformation programVariableInformation = storm::utility::ir::createVariableInformation(program);
                
                // Create a context and register all variables of the program with their correct type.
                z3::context localContext;
                std::map<std::string, z3::expr> solverVariables;
                for (auto const& booleanVariable : programVariableInformation.booleanVariables) {
                    solverVariables.emplace(booleanVariable.getName(), localContext.bool_const(booleanVariable.getName().c_str()));
                }
                for (auto const& integerVariable : programVariableInformation.integerVariables) {
                    solverVariables.emplace(integerVariable.getName(), localContext.int_const(integerVariable.getName().c_str()));
                }
                
                // Now create a corresponding local solver and assert all range bounds for the integer variables.
                z3::solver localSolver(localContext);
                storm::adapters::Z3ExpressionAdapter expressionAdapter(localContext, solverVariables);
                for (auto const& integerVariable : programVariableInformation.integerVariables) {
                    z3::expr lowerBound = expressionAdapter.translateExpression(integerVariable.getLowerBound());
                    lowerBound = solverVariables.at(integerVariable.getName()) >= lowerBound;
                    localSolver.add(lowerBound);
                    
                    z3::expr upperBound = expressionAdapter.translateExpression(integerVariable.getUpperBound());
                    upperBound = solverVariables.at(integerVariable.getName()) <= upperBound;
                    localSolver.add(upperBound);
                }
                
                // Construct an expression that exactly characterizes the initial state.
                std::unique_ptr<storm::utility::ir::StateType> initialState(storm::utility::ir::getInitialState(program, programVariableInformation));
                z3::expr initialStateExpression = localContext.bool_val(true);
                for (uint_fast64_t index = 0; index < programVariableInformation.booleanVariables.size(); ++index) {
                    if (std::get<0>(*initialState).at(programVariableInformation.booleanVariableToIndexMap.at(programVariableInformation.booleanVariables[index].getName()))) {
                        initialStateExpression = initialStateExpression && solverVariables.at(programVariableInformation.booleanVariables[index].getName());
                    } else {
                        initialStateExpression = initialStateExpression && !solverVariables.at(programVariableInformation.booleanVariables[index].getName());
                    }
                }
                for (uint_fast64_t index = 0; index < programVariableInformation.integerVariables.size(); ++index) {
                    storm::ir::IntegerVariable const& variable = programVariableInformation.integerVariables[index];
                    initialStateExpression = initialStateExpression && (solverVariables.at(variable.getName()) == localContext.int_val(std::get<1>(*initialState).at(programVariableInformation.integerVariableToIndexMap.at(variable.getName()))));
                }
                
                // Store the found implications in a container similar to the preceding label sets.
                std::map<storm::storage::VectorSet<uint_fast64_t>, std::set<storm::storage::VectorSet<uint_fast64_t>>> backwardImplications;
                
                // Now check for possible backward cuts.
                for (auto const& labelSetAndPrecedingLabelSetsPair : precedingLabelSets) {
                
                    // Find out the commands for the currently considered label set.
                    std::vector<std::reference_wrapper<storm::ir::Command const>> currentCommandVector;
                    for (uint_fast64_t moduleIndex = 0; moduleIndex < program.getNumberOfModules(); ++moduleIndex) {
                        storm::ir::Module const& module = program.getModule(moduleIndex);
                        
                        for (uint_fast64_t commandIndex = 0; commandIndex < module.getNumberOfCommands(); ++commandIndex) {
                            storm::ir::Command const& command = module.getCommand(commandIndex);

                            // If the current command is one of the commands we need to consider, store a reference to it in the container.
                            if (labelSetAndPrecedingLabelSetsPair.first.contains(command.getGlobalIndex())) {
                                currentCommandVector.push_back(command);
                            }
                        }
                    }
                    
                    // Save the state of the solver so we can easily backtrack.
                    localSolver.push();
                    
                    // Check if the command set is enabled in the initial state.
                    for (auto const& command : currentCommandVector) {
                        localSolver.add(expressionAdapter.translateExpression(command.get().getGuard()));
                    }
                    localSolver.add(initialStateExpression);
                    
                    z3::check_result checkResult = localSolver.check();
                    localSolver.pop();
                    localSolver.push();

                    // If the solver reports unsat, then we know that the current selection is not enabled in the initial state.
                    if (checkResult == z3::unsat) {
                        LOG4CPLUS_DEBUG(logger, "Selection not enabled in initial state.");
                        std::unique_ptr<storm::ir::expressions::BaseExpression> guardConjunction;
                        if (currentCommandVector.size() == 1) {
                            guardConjunction = currentCommandVector.begin()->get().getGuard()->clone();
                        } else if (currentCommandVector.size() > 1) {
                            std::vector<std::reference_wrapper<storm::ir::Command const>>::const_iterator setIterator = currentCommandVector.begin();
                            std::unique_ptr<storm::ir::expressions::BaseExpression> first = setIterator->get().getGuard()->clone();
                            ++setIterator;
                            std::unique_ptr<storm::ir::expressions::BaseExpression> second = setIterator->get().getGuard()->clone();
                            guardConjunction = std::unique_ptr<storm::ir::expressions::BaseExpression>(new storm::ir::expressions::BinaryBooleanFunctionExpression(std::move(first), std::move(second), storm::ir::expressions::BinaryBooleanFunctionExpression::AND));
                            ++setIterator;
                            
                            while (setIterator != currentCommandVector.end()) {
                                guardConjunction = std::unique_ptr<storm::ir::expressions::BaseExpression>(new storm::ir::expressions::BinaryBooleanFunctionExpression(std::move(guardConjunction), setIterator->get().getGuard()->clone(), storm::ir::expressions::BinaryBooleanFunctionExpression::AND));
                                ++setIterator;
                            }
                        } else {
                            throw storm::exceptions::InvalidStateException() << "Choice label set is empty.";
                            LOG4CPLUS_DEBUG(logger, "Choice label set is empty.");
                        }
                        
                        LOG4CPLUS_DEBUG(logger, "About to assert disjunction of negated guards.");
                        z3::expr guardExpression = localContext.bool_val(false);
                        bool firstAssignment = true;
                        for (auto const& command : currentCommandVector) {
                            if (firstAssignment) {
                                guardExpression = !expressionAdapter.translateExpression(command.get().getGuard());
                            } else {
                                guardExpression = guardExpression | !expressionAdapter.translateExpression(command.get().getGuard());
                            }
                        }
                        localSolver.add(guardExpression);
                        LOG4CPLUS_DEBUG(logger, "Asserted disjunction of negated guards.");
                        
                        // Now check the possible preceding label sets for the essential ones.
                        for (auto const& precedingLabelSet : labelSetAndPrecedingLabelSetsPair.second) {
                            // Create a restore point so we can easily pop-off all weakest precondition expressions.
                            localSolver.push();
                            
                            // Find out the commands for the currently considered preceding label set.
                            std::vector<std::reference_wrapper<storm::ir::Command const>> currentPrecedingCommandVector;
                            for (uint_fast64_t moduleIndex = 0; moduleIndex < program.getNumberOfModules(); ++moduleIndex) {
                                storm::ir::Module const& module = program.getModule(moduleIndex);
                                
                                for (uint_fast64_t commandIndex = 0; commandIndex < module.getNumberOfCommands(); ++commandIndex) {
                                    storm::ir::Command const& command = module.getCommand(commandIndex);
                                    
                                    // If the current command is one of the commands we need to consider, store a reference to it in the container.
                                    if (precedingLabelSet.contains(command.getGlobalIndex())) {
                                        currentPrecedingCommandVector.push_back(command);
                                    }
                                }
                            }
                            
                            // Assert all the guards of the preceding command set.
                            for (auto const& command : currentPrecedingCommandVector) {
                                localSolver.add(expressionAdapter.translateExpression(command.get().getGuard()));
                            }
                            
                            std::vector<std::vector<storm::ir::Update>::const_iterator> iteratorVector;
                            for (auto const& command : currentPrecedingCommandVector) {
                                iteratorVector.push_back(command.get().getUpdates().begin());
                            }
                            
                            // Iterate over all possible combinations of updates of the preceding command set.
                            std::vector<z3::expr> formulae;
                            bool done = false;
                            while (!done) {
                                std::vector<std::reference_wrapper<storm::ir::Update const>> currentUpdateCombination;
                                for (auto const& updateIterator : iteratorVector) {
                                    currentUpdateCombination.push_back(*updateIterator);
                                }
                                
                                LOG4CPLUS_DEBUG(logger, "About to assert a weakest precondition.");
                                std::unique_ptr<storm::ir::expressions::BaseExpression> wp = storm::utility::ir::getWeakestPrecondition(guardConjunction->clone(), currentUpdateCombination);
                                formulae.push_back(expressionAdapter.translateExpression(wp));
                                LOG4CPLUS_DEBUG(logger, "Asserted weakest precondition.");
                                
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
                            assertDisjunction(localContext, localSolver, formulae);
                            
                            LOG4CPLUS_DEBUG(logger, "Asserted disjunction of all weakest preconditions.");
                            
                            if (localSolver.check() == z3::sat) {
                                backwardImplications[labelSetAndPrecedingLabelSetsPair.first].insert(precedingLabelSet);
                            }
                            
                            localSolver.pop();
                        }
                        
                        // Popping the disjunction of negated guards from the solver stack.
                        localSolver.pop();
                    }
                }
                
                // Compute the sets of labels such that the transitions labeled with this set possess at least one known successor.
                storm::storage::VectorSet<storm::storage::VectorSet<uint_fast64_t>> hasKnownPredecessor;
                for (auto const& labelSetImplicationsPair : backwardImplications) {
                    for (auto const& set : labelSetImplicationsPair.second) {
                        if (set.subsetOf(relevancyInformation.knownLabels)) {
                            hasKnownPredecessor.insert(set);
                            break;
                        }
                    }
                }
                
                LOG4CPLUS_DEBUG(logger, "Asserting taken labels are preceded by another label if they are not an initial label.");
                // Now assert that for each non-target label, we take a following label.
                for (auto const& labelSetImplicationsPair : backwardImplications) {
                    std::vector<z3::expr> formulae;
                    
                    // Only build a constraint if the combination no predecessor set is already known.
                    if (!hasKnownPredecessor.contains(labelSetImplicationsPair.first)) {
                        
                        // Compute the set of unknown labels on the left-hand side of the implication.
                        storm::storage::VectorSet<uint_fast64_t> unknownLhsLabels;
                        std::set_difference(labelSetImplicationsPair.first.begin(), labelSetImplicationsPair.first.end(), relevancyInformation.knownLabels.begin(), relevancyInformation.knownLabels.end(), std::inserter(unknownLhsLabels, unknownLhsLabels.end()));
                        for (auto label : unknownLhsLabels) {
                            formulae.push_back(!variableInformation.labelVariables.at(variableInformation.labelToIndexMap.at(label)));
                        }
                        
                        for (auto const& precedingSet : labelSetImplicationsPair.second) {
                            storm::storage::VectorSet<uint_fast64_t> tmpSet;
                            
                            // Check which labels of the current following set are not known. This set must be non-empty, because
                            // otherwise a predecessor combination would already be known and control cannot reach this point.
                            std::set_difference(precedingSet.begin(), precedingSet.end(), relevancyInformation.knownLabels.begin(), relevancyInformation.knownLabels.end(), std::inserter(tmpSet, tmpSet.end()));
                            
                            // Construct an expression that enables all unknown labels of the current following set.
                            z3::expr conj = context.bool_val(true);
                            for (auto label : tmpSet) {
                                conj = conj && variableInformation.labelVariables.at(variableInformation.labelToIndexMap.at(label));
                            }
                            formulae.push_back(conj);
                        }
                        
                        if (labelSetImplicationsPair.first.size() > 1) {
                            // Taking all commands of a combination does not necessarily mean that a predecessor label set needs to be taken.
                            // This is because it could be that the commands are taken to enable other synchronizations. Therefore, we need
                            // to add an additional clause that says that the right-hand side of the implication is also true if all commands
                            // of the current choice have enabled synchronization options.
                            z3::expr finalDisjunct = context.bool_val(false);
                            for (auto label : labelSetImplicationsPair.first) {
                                z3::expr alternativeExpressionForLabel = context.bool_val(false);
                                std::set<storm::storage::VectorSet<uint_fast64_t>> const& synchsForCommand = synchronizingLabels.at(label);
                                
                                for (auto const& synchSet : synchsForCommand) {
                                    z3::expr alternativeExpression = context.bool_val(true);

                                    // If the current synchSet is the same as left-hand side of the implication, we need to skip it.
                                    if (synchSet == labelSetImplicationsPair.first) continue;

                                    // Now that we have the labels that are unknown and "missing", we still need to check whether this other
                                    // synchronizing set already has a known predecessor.
                                    if (!hasKnownPredecessor.contains(synchSet)) {
                                        // If not, we can assert that we take one of its possible predecessors.
                                        storm::storage::VectorSet<uint_fast64_t> unknownSynchs;
                                        std::set_difference(synchSet.begin(), synchSet.end(), relevancyInformation.knownLabels.begin(), relevancyInformation.knownLabels.end(), std::inserter(unknownSynchs, unknownSynchs.end()));
                                        unknownSynchs.erase(label);

                                        for (auto label : unknownSynchs) {
                                            alternativeExpression = alternativeExpression && variableInformation.labelVariables.at(variableInformation.labelToIndexMap.at(label));
                                        }
                                        
                                        z3::expr disjunctionOverPredecessors = context.bool_val(false);
                                        auto precedingLabelSetsIterator = precedingLabelSets.find(synchSet);
                                        if (precedingLabelSetsIterator != precedingLabelSets.end()) {
                                            for (auto precedingSet : precedingLabelSetsIterator->second) {
                                                z3::expr conjunctionOverLabels = context.bool_val(true);
                                                for (auto label : precedingSet) {
                                                    if (!relevancyInformation.knownLabels.contains(label)) {
                                                        conjunctionOverLabels = conjunctionOverLabels && variableInformation.labelVariables.at(variableInformation.labelToIndexMap.at(label));
                                                    }
                                                }
                                                disjunctionOverPredecessors = disjunctionOverPredecessors || conjunctionOverLabels;
                                            }
                                        }
                                        
                                        alternativeExpression = alternativeExpression && disjunctionOverPredecessors;
                                    }
                                    
                                    alternativeExpressionForLabel = alternativeExpressionForLabel || alternativeExpression;
                                }
                                
                                finalDisjunct = finalDisjunct && alternativeExpressionForLabel;
                            }

                            formulae.push_back(finalDisjunct);
                        }
                        
                        if (formulae.size() > 0) {
                            assertDisjunction(context, solver, formulae);
                        }
                    }
                }
            }
        
            /*!
             * Asserts that the disjunction of the given formulae holds. If the content of the disjunction is empty,
             * this corresponds to asserting false.
             *
             * @param context The Z3 context in which to build the expressions.
             * @param solver The solver to use for the satisfiability evaluation.
             * @param formulaVector A vector of expressions that shall form the disjunction.
             */
            static void assertDisjunction(z3::context& context, z3::solver& solver, std::vector<z3::expr> const& formulaVector) {
                z3::expr disjunction = context.bool_val(false);
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
            static std::pair<z3::expr, z3::expr> createFullAdder(z3::expr in1, z3::expr in2, z3::expr carryIn) {
                z3::expr resultBit = (in1 && !in2 && !carryIn) || (!in1 && in2 && !carryIn) || (!in1 && !in2 && carryIn) || (in1 && in2 && carryIn);
                z3::expr carryBit = in1 && in2 || in1 && carryIn || in2 && carryIn;
                
                return std::make_pair(carryBit, resultBit);
            }
            
            /*!
             * Creates an adder for the two inputs of equal size. The resulting vector represents the different bits of
             * the sum (and is thus one bit longer than the two inputs).
             *
             * @param context The Z3 context in which to build the expressions.
             * @param in1 The first input to the adder.
             * @param in2 The second input to the adder.
             * @return A vector representing the bits of the sum of the two inputs.
             */
            static std::vector<z3::expr> createAdder(z3::context& context, std::vector<z3::expr> const& in1, std::vector<z3::expr> const& in2) {
                // Sanity check for sizes of input.
                if (in1.size() != in2.size() || in1.size() == 0) {
                    LOG4CPLUS_ERROR(logger, "Illegal input to adder (" << in1.size() << ", " << in2.size() << ").");
                    throw storm::exceptions::InvalidArgumentException() << "Illegal input to adder.";
                }
                
                // Prepare result.
                std::vector<z3::expr> result;
                result.reserve(in1.size() + 1);
                
                // Add all bits individually and pass on carry bit appropriately.
                z3::expr carryBit = context.bool_val(false);
                for (uint_fast64_t currentBit = 0; currentBit < in1.size(); ++currentBit) {
                    std::pair<z3::expr, z3::expr> localResult = createFullAdder(in1[currentBit], in2[currentBit], carryBit);
                    
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
             * @param context The Z3 context in which to build the expressions.
             * @param in A vector or binary encoded numbers.
             * @return A vector of numbers that each correspond to the sum of two consecutive elements of the input.
             */
            static std::vector<std::vector<z3::expr>> createAdderPairs(z3::context& context, std::vector<std::vector<z3::expr>> const& in) {
                std::vector<std::vector<z3::expr>> result;
                
                result.reserve(in.size() / 2 + in.size() % 2);
                
                for (uint_fast64_t index = 0; index < in.size() / 2; ++index) {
                    result.push_back(createAdder(context, in[2 * index], in[2 * index + 1]));
                }
                
                if (in.size() % 2 != 0) {
                    result.push_back(in.back());
                    result.back().push_back(context.bool_val(false));
                }
                
                return result;
            }
            
            /*!
             * Creates a counter circuit that returns the number of literals out of the given vector that are set to true.
             *
             * @param context The Z3 context in which to build the expressions.
             * @param literals The literals for which to create the adder circuit.
             * @return A bit vector representing the number of literals that are set to true.
             */
            static std::vector<z3::expr> createCounterCircuit(z3::context& context, std::vector<z3::expr> const& literals) {
                LOG4CPLUS_DEBUG(logger, "Creating counter circuit for " << literals.size() << " literals.");

                // Create the auxiliary vector.
                std::vector<std::vector<z3::expr>> aux;
                for (uint_fast64_t index = 0; index < literals.size(); ++index) {
                    aux.emplace_back();
                    aux.back().push_back(literals[index]);
                }
                
                while (aux.size() > 1) {
                    aux = createAdderPairs(context, aux);
                }
                
                return aux[0];
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
             * @param context The Z3 context in which to build the expressions.
             * @param solver The solver to use for the satisfiability evaluation.
             * @param input The variables that encode the value to restrict.
             * @param k The bound for the binary-encoded value.
             * @return The relaxation variable associated with the constraint.
             */
             static z3::expr assertLessOrEqualKRelaxed(z3::context& context, z3::solver& solver, std::vector<z3::expr> const& input, uint64_t k) {
                LOG4CPLUS_DEBUG(logger, "Asserting solution has size less or equal " << k << ".");

                z3::expr result(context);
                if (bitIsSet(k, 0)) {
                    result = context.bool_val(true);
                } else {
                    result = !input.at(0);
                }
                for (uint_fast64_t index = 1; index < input.size(); ++index) {
                    z3::expr i1(context);
                    z3::expr i2(context);
                    
                    if (bitIsSet(k, index)) {
                        i1 = !input.at(index);
                        i2 = result;
                    } else {
                        i1 = context.bool_val(false);
                        i2 = context.bool_val(false);
                    }
                    result = i1 || i2 || (!input.at(index) && result);
                }
                
                std::stringstream variableName;
                variableName << "relaxed" << k;
                z3::expr relaxingVariable = context.bool_const(variableName.str().c_str());
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
             * @param variableInformation A structure with information about the variables for the labels.
             * @return True iff the constraint system was satisfiable.
             */
            static bool fuMalikMaxsatStep(z3::context& context, z3::solver& solver, std::vector<z3::expr>& auxiliaryVariables, std::vector<z3::expr>& softConstraints, uint_fast64_t& nextFreeVariableIndex) {
                z3::expr_vector assumptions(context);
                for (auto const& auxiliaryVariable : auxiliaryVariables) {
                    assumptions.push_back(!auxiliaryVariable);
                }
                                
                // Check whether the assumptions are satisfiable.
                LOG4CPLUS_DEBUG(logger, "Invoking satisfiability checking.");
                z3::check_result result = solver.check(assumptions);
                LOG4CPLUS_DEBUG(logger, "Done invoking satisfiability checking.");
                
                if (result == z3::sat) {
                    return true;
                } else {
                    LOG4CPLUS_DEBUG(logger, "Computing unsat core.");
                    z3::expr_vector unsatCore = solver.unsat_core();
                    LOG4CPLUS_DEBUG(logger, "Computed unsat core.");
                    
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
             * @param variableInformation A structure with information about the variables for the labels.
             */
            static void ruleOutSolution(z3::context& context, z3::solver& solver, storm::storage::VectorSet<uint_fast64_t> const& commandSet, VariableInformation const& variableInformation) {
                z3::expr blockSolutionExpression = context.bool_val(false);
                for (auto labelIndexPair : variableInformation.labelToIndexMap) {
                    if (commandSet.contains(labelIndexPair.first)) {
                        blockSolutionExpression = blockSolutionExpression || variableInformation.labelVariables[labelIndexPair.second];
                    }
                }
                
                solver.add(blockSolutionExpression);
            }

            /*!
             * Determines the set of labels that was chosen by the given model.
             *
             * @param context The Z3 context in which to build the expressions.
             * @param model The Z3 model from which to extract the information.
             * @param variableInformation A structure with information about the variables of the solver.
             */
            static storm::storage::VectorSet<uint_fast64_t> getUsedLabelSet(z3::context& context, z3::model const& model, VariableInformation const& variableInformation) {
                storm::storage::VectorSet<uint_fast64_t> result;
                for (auto const& labelIndexPair : variableInformation.labelToIndexMap) {
                    z3::expr auxValue = model.eval(variableInformation.labelVariables.at(labelIndexPair.second));
                    
                    // Check whether the auxiliary variable was set or not.
                    if (eq(auxValue, context.bool_val(true))) {
                        result.insert(labelIndexPair.first);
                    } else if (eq(auxValue, context.bool_val(false))) {
                        // Nothing to do in this case.
                    } else if (eq(auxValue, variableInformation.labelVariables.at(labelIndexPair.second))) {
                        // If the auxiliary variable is a don't care, then we don't take the corresponding command.
                    } else {
                        throw storm::exceptions::InvalidStateException() << "Could not retrieve value of boolean variable from illegal value.";
                    }
                }
                return result;
            }
            
            /*!
             * Asserts an adder structure in the given solver that counts the number of variables that are set to true
             * out of the given variables.
             *
             * @param context The Z3 context in which to build the expressions.
             * @param solver The solver for which to add the adder.
             * @param variableInformation A structure with information about the variables of the solver.
             */
            static std::vector<z3::expr> assertAdder(z3::context& context, z3::solver& solver, VariableInformation const& variableInformation) {
                std::stringstream variableName;
                std::vector<z3::expr> result;
                
                std::vector<z3::expr> adderVariables = createCounterCircuit(context, variableInformation.labelVariables);
                for (uint_fast64_t i = 0; i < adderVariables.size(); ++i) {
                    variableName.str("");
                    variableName.clear();
                    variableName << "adder" << i;
                    result.push_back(context.bool_const(variableName.str().c_str()));
                    solver.add(implies(adderVariables[i], result.back()));
                }
                
                return result;
            }
            
            /*!
             * Finds the smallest set of labels such that the constraint system of the solver is still satisfiable.
             *
             * @param context The Z3 context in which to build the expressions.
             * @param solver The solver to use for the satisfiability evaluation.
             * @param variableInformation A structure with information about the variables of the solver.
             * @param currentBound The currently known lower bound for the number of labels that need to be enabled
             * in order to satisfy the constraint system.
             * @return The smallest set of labels such that the constraint system of the solver is satisfiable.
             */
            static storm::storage::VectorSet<uint_fast64_t> findSmallestCommandSet(z3::context& context, z3::solver& solver, VariableInformation& variableInformation, uint_fast64_t& currentBound) {
                // Check if we can find a solution with the current bound.
                z3::expr assumption = !variableInformation.auxiliaryVariables.back();

                // As long as the constraints are unsatisfiable, we need to relax the last at-most-k constraint and
                // try with an increased bound.
                while (solver.check(1, &assumption) == z3::unsat) {
                    LOG4CPLUS_DEBUG(logger, "Constraint system is unsatisfiable with at most " << currentBound << " taken commands; increasing bound.");
                    solver.add(variableInformation.auxiliaryVariables.back());
                    variableInformation.auxiliaryVariables.push_back(assertLessOrEqualKRelaxed(context, solver, variableInformation.adderVariables, ++currentBound));
                    assumption = !variableInformation.auxiliaryVariables.back();
                }
                
                // At this point we know that the constraint system was satisfiable, so compute the induced label
                // set and return it.
                return getUsedLabelSet(context, solver.get_model(), variableInformation);
            }
            
            /*!
             * Analyzes the given sub-MDP that has a maximal reachability of zero (i.e. no psi states are reachable) and tries to construct assertions that aim to make at least one psi state reachable.
             *
             * @param context The Z3 context in which to build the expressions.
             * @param solver The solver to use for the satisfiability evaluation.
             * @param subMdp The sub-MDP resulting from restricting the original MDP to the given command set.
             * @param originalMdp The original MDP.
             * @param phiStates A bit vector characterizing all phi states in the model.
             * @param psiState A bit vector characterizing all psi states in the model.
             * @param commandSet The currently chosen set of commands.
             * @param variableInformation A structure with information about the variables of the solver.
             */
            static void analyzeZeroProbabilitySolution(z3::context& context, z3::solver& solver, storm::models::Mdp<T> const& subMdp, storm::models::Mdp<T> const& originalMdp, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, storm::storage::VectorSet<uint_fast64_t> const& commandSet, VariableInformation& variableInformation, RelevancyInformation const& relevancyInformation) {
                storm::storage::BitVector reachableStates(subMdp.getNumberOfStates());
                
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
                std::vector<storm::storage::VectorSet<uint_fast64_t>> const& subChoiceLabeling = subMdp.getChoiceLabeling();
                
                // Now determine which states and labels are actually reachable.
                storm::storage::VectorSet<uint_fast64_t> reachableLabels;
                while (!stack.empty()) {
                    uint_fast64_t currentState = stack.back();
                    stack.pop_back();

                    for (uint_fast64_t currentChoice = nondeterministicChoiceIndices[currentState]; currentChoice < nondeterministicChoiceIndices[currentState + 1]; ++currentChoice) {
                        bool choiceTargetsRelevantState = false;
                        
                        for (typename storm::storage::SparseMatrix<T>::ConstIndexIterator successorIt = transitionMatrix.constColumnIteratorBegin(currentChoice), successorIte = transitionMatrix.constColumnIteratorEnd(currentChoice); successorIt != successorIte; ++successorIt) {
                            if (relevancyInformation.relevantStates.get(*successorIt) && currentState != *successorIt) {
                                choiceTargetsRelevantState = true;
                                if (!reachableStates.get(*successorIt)) {
                                    reachableStates.set(*successorIt, true);
                                    stack.push_back(*successorIt);
                                }
                            } else if (psiStates.get(*successorIt)) {
                                targetStateIsReachable = true;
                            }
                        }
                        
                        if (choiceTargetsRelevantState) {
                            for (auto label : subChoiceLabeling[currentChoice]) {
                                reachableLabels.insert(label);
                            }
                        }
                    }
                }
                
                LOG4CPLUS_DEBUG(logger, "Successfully performed reachability analysis.");
                
                if (targetStateIsReachable) {
                    LOG4CPLUS_ERROR(logger, "Target must be unreachable for this analysis.");
                    throw storm::exceptions::InvalidStateException() << "Target must be unreachable for this analysis.";
                }
                
                storm::storage::BitVector unreachableRelevantStates = ~reachableStates & relevancyInformation.relevantStates;
                storm::storage::BitVector statesThatCanReachTargetStates = storm::utility::graph::performProbGreater0E(subMdp, subMdp.getBackwardTransitions(), phiStates, psiStates);
                std::vector<storm::storage::VectorSet<uint_fast64_t>> guaranteedLabelSets = storm::utility::counterexamples::getGuaranteedLabelSets(originalMdp, statesThatCanReachTargetStates, relevancyInformation.relevantLabels);
                
                LOG4CPLUS_DEBUG(logger, "Found " << reachableLabels.size() << " reachable labels and " << reachableStates.getNumberOfSetBits() << " reachable states.");
                
                // Search for states on the border of the reachable state space, i.e. states that are still reachable
                // and possess a (disabled) option to leave the reachable part of the state space.
                std::vector<storm::storage::VectorSet<uint_fast64_t>> const& choiceLabeling = originalMdp.getChoiceLabeling();
                std::set<storm::storage::VectorSet<uint_fast64_t>> cutLabels;
                for (auto state : reachableStates) {
                    for (auto currentChoice : relevancyInformation.relevantChoicesForRelevantStates.at(state)) {
                        if (!choiceLabeling[currentChoice].subsetOf(commandSet)) {
                            bool isBorderChoice = false;

                            // Determine whether the state has the option to leave the reachable state space and go to the unreachable relevant states.
                            for (typename storm::storage::SparseMatrix<T>::ConstIndexIterator successorIt = originalMdp.getTransitionMatrix().constColumnIteratorBegin(currentChoice), successorIte = originalMdp.getTransitionMatrix().constColumnIteratorEnd(currentChoice); successorIt != successorIte; ++successorIt) {
                                if (unreachableRelevantStates.get(*successorIt)) {
                                    isBorderChoice = true;
                                }
                            }
                            
                            if (isBorderChoice) {
                                storm::storage::VectorSet<uint_fast64_t> currentLabelSet;
                                for (auto label : choiceLabeling[currentChoice]) {
                                    if (!commandSet.contains(label)) {
                                        currentLabelSet.insert(label);
                                    }
                                }
                                std::set_difference(guaranteedLabelSets[state].begin(), guaranteedLabelSets[state].end(), commandSet.begin(), commandSet.end(), std::inserter(currentLabelSet, currentLabelSet.end()));
                                
                                cutLabels.insert(currentLabelSet);
                            }
                        }
                    }
                }
                
                // Given the results of the previous analysis, we construct the implications
                std::vector<z3::expr> formulae;
                storm::storage::VectorSet<uint_fast64_t> unknownReachableLabels;
                std::set_difference(reachableLabels.begin(), reachableLabels.end(), relevancyInformation.knownLabels.begin(), relevancyInformation.knownLabels.end(), std::inserter(unknownReachableLabels, unknownReachableLabels.end()));
                for (auto label : unknownReachableLabels) {
                    formulae.push_back(!variableInformation.labelVariables.at(variableInformation.labelToIndexMap.at(label)));
                }
                for (auto const& cutLabelSet : cutLabels) {
                    z3::expr cube = context.bool_val(true);
                    for (auto cutLabel : cutLabelSet) {
                        cube = cube && variableInformation.labelVariables.at(variableInformation.labelToIndexMap.at(cutLabel));
                    }
                    
                    formulae.push_back(cube);
                }
                
                LOG4CPLUS_DEBUG(logger, "Asserting reachability implications.");
                assertDisjunction(context, solver, formulae);
                
            }
            
            /*!
             * Analyzes the given sub-MDP that has a non-zero maximal reachability and tries to construct assertions that aim to guide the solver to solutions
             * with an improved probability value.
             *
             * @param context The Z3 context in which to build the expressions.
             * @param solver The solver to use for the satisfiability evaluation.
             * @param subMdp The sub-MDP resulting from restricting the original MDP to the given command set.
             * @param originalMdp The original MDP.
             * @param phiStates A bit vector characterizing all phi states in the model.
             * @param psiState A bit vector characterizing all psi states in the model.
             * @param commandSet The currently chosen set of commands.
             * @param variableInformation A structure with information about the variables of the solver.
             */
            static void analyzeInsufficientProbabilitySolution(z3::context& context, z3::solver& solver, storm::models::Mdp<T> const& subMdp, storm::models::Mdp<T> const& originalMdp, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, storm::storage::VectorSet<uint_fast64_t> const& commandSet, VariableInformation& variableInformation, RelevancyInformation const& relevancyInformation) {
                // ruleOutSolution(context, solver, commandSet, variableInformation);
                
                storm::storage::BitVector reachableStates(subMdp.getNumberOfStates());
                
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
                std::vector<storm::storage::VectorSet<uint_fast64_t>> const& subChoiceLabeling = subMdp.getChoiceLabeling();
                
                // Now determine which states and labels are actually reachable.
                storm::storage::VectorSet<uint_fast64_t> reachableLabels;
                while (!stack.empty()) {
                    uint_fast64_t currentState = stack.back();
                    stack.pop_back();
                    
                    for (uint_fast64_t currentChoice = nondeterministicChoiceIndices[currentState]; currentChoice < nondeterministicChoiceIndices[currentState + 1]; ++currentChoice) {
                        bool choiceTargetsRelevantState = false;
                        
                        for (typename storm::storage::SparseMatrix<T>::ConstIndexIterator successorIt = transitionMatrix.constColumnIteratorBegin(currentChoice), successorIte = transitionMatrix.constColumnIteratorEnd(currentChoice); successorIt != successorIte; ++successorIt) {
                            if (relevancyInformation.relevantStates.get(*successorIt) && currentState != *successorIt) {
                                choiceTargetsRelevantState = true;
                                if (!reachableStates.get(*successorIt)) {
                                    reachableStates.set(*successorIt, true);
                                    stack.push_back(*successorIt);
                                }
                            } else if (psiStates.get(*successorIt)) {
                                targetStateIsReachable = true;
                            }
                        }
                        
                        if (choiceTargetsRelevantState) {
                            for (auto label : subChoiceLabeling[currentChoice]) {
                                reachableLabels.insert(label);
                            }
                        }
                    }
                }
                LOG4CPLUS_DEBUG(logger, "Successfully determined reachable state space.");
                
                storm::storage::BitVector unreachableRelevantStates = ~reachableStates & relevancyInformation.relevantStates;
                storm::storage::BitVector statesThatCanReachTargetStates = storm::utility::graph::performProbGreater0E(subMdp, subMdp.getBackwardTransitions(), phiStates, psiStates);
                std::vector<storm::storage::VectorSet<uint_fast64_t>> guaranteedLabelSets = storm::utility::counterexamples::getGuaranteedLabelSets(originalMdp, statesThatCanReachTargetStates, relevancyInformation.relevantLabels);
                
                // Search for states for which we could enable another option and possibly improve the reachability probability.
                std::vector<storm::storage::VectorSet<uint_fast64_t>> const& choiceLabeling = originalMdp.getChoiceLabeling();
                std::set<storm::storage::VectorSet<uint_fast64_t>> cutLabels;
                for (auto state : reachableStates) {
                    for (auto currentChoice : relevancyInformation.relevantChoicesForRelevantStates.at(state)) {
                        if (!choiceLabeling[currentChoice].subsetOf(commandSet)) {
                            storm::storage::VectorSet<uint_fast64_t> currentLabelSet;
                            for (auto label : choiceLabeling[currentChoice]) {
                                if (!commandSet.contains(label)) {
                                    currentLabelSet.insert(label);
                                }
                            }
                            std::set_difference(guaranteedLabelSets[state].begin(), guaranteedLabelSets[state].end(), commandSet.begin(), commandSet.end(), std::inserter(currentLabelSet, currentLabelSet.end()));
                            
                            cutLabels.insert(currentLabelSet);

                        }
                    }
                }
                
                // Given the results of the previous analysis, we construct the implications
                std::vector<z3::expr> formulae;
                storm::storage::VectorSet<uint_fast64_t> unknownReachableLabels;
                std::set_difference(reachableLabels.begin(), reachableLabels.end(), relevancyInformation.knownLabels.begin(), relevancyInformation.knownLabels.end(), std::inserter(unknownReachableLabels, unknownReachableLabels.end()));
                for (auto label : unknownReachableLabels) {
                    formulae.push_back(!variableInformation.labelVariables.at(variableInformation.labelToIndexMap.at(label)));
                }
                for (auto const& cutLabelSet : cutLabels) {
                    z3::expr cube = context.bool_val(true);
                    for (auto cutLabel : cutLabelSet) {
                        cube = cube && variableInformation.labelVariables.at(variableInformation.labelToIndexMap.at(cutLabel));
                    }
                    
                    formulae.push_back(cube);
                }
                
                LOG4CPLUS_DEBUG(logger, "Asserting reachability implications.");
                assertDisjunction(context, solver, formulae);
            }
#endif
            
        public:
            
            /*!
             * Computes the minimal command set that is needed in the given MDP to exceed the given probability threshold for satisfying phi until psi.
             *
             * @param program The program that was used to build the MDP.
             * @param constantDefinitionString A string defining the undefined constants in the given program.
             * @param labeledMdp The MDP in which to find the minimal command set.
             * @param phiStates A bit vector characterizing all phi states in the model.
             * @param psiStates A bit vector characterizing all psi states in the model.
             * @param probabilityThreshold The probability value that must be achieved or exceeded.
             * @param strictBound A flag indicating whether the probability must be achieved (in which case the flag must be set) or strictly exceeded
             * (if the flag is set to false).
             * @param checkThresholdFeasible If set, it is verified that the model can actually achieve/exceed the given probability value. If this check
             * is made and fails, an exception is thrown.
             */
            static storm::storage::VectorSet<uint_fast64_t> getMinimalCommandSet(storm::ir::Program program, std::string const& constantDefinitionString, storm::models::Mdp<T> const& labeledMdp, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, double probabilityThreshold, bool strictBound, bool checkThresholdFeasible = false) {
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

                storm::utility::ir::defineUndefinedConstants(program, constantDefinitionString);

                // (0) Check whether the MDP is indeed labeled.
                if (!labeledMdp.hasChoiceLabels()) {
                    throw storm::exceptions::InvalidArgumentException() << "Minimal command set generation is impossible for unlabeled model.";
                }
                
                // (1) Check whether its possible to exceed the threshold if checkThresholdFeasible is set.
                double maximalReachabilityProbability = 0;
                storm::modelchecker::prctl::SparseMdpPrctlModelChecker<T> modelchecker(labeledMdp, new storm::solver::GmmxxNondeterministicLinearEquationSolver<T>());
                std::vector<T> result = modelchecker.checkUntil(false, phiStates, psiStates, false, nullptr);
                for (auto state : labeledMdp.getInitialStates()) {
                    maximalReachabilityProbability = std::max(maximalReachabilityProbability, result[state]);
                }
                if ((strictBound && maximalReachabilityProbability < probabilityThreshold) || (!strictBound && maximalReachabilityProbability <= probabilityThreshold)) {
                    throw storm::exceptions::InvalidArgumentException() << "Given probability threshold " << probabilityThreshold << " can not be " << (strictBound ? "achieved" : "exceeded") << " in model with maximal reachability probability of " << maximalReachabilityProbability << ".";
                }
                
                // (2) Identify all states and commands that are relevant, because only these need to be considered later.
                RelevancyInformation relevancyInformation = determineRelevantStatesAndLabels(labeledMdp, phiStates, psiStates);
                
                // (3) Create context for solver.
                z3::context context;
                
                // (4) Create the variables for the relevant commands.
                VariableInformation variableInformation = createExpressionsForRelevantLabels(context, relevancyInformation.relevantLabels);
                LOG4CPLUS_DEBUG(logger, "Created variables.");

                // (5) After all variables have been created, create a solver for that context.
                z3::solver solver(context);

                // (6) Now assert an adder whose result variables can later be used to constrain the nummber of label
                // variables that were set to true. Initially, we are looking for a solution that has no label enabled
                // and subsequently relax that.
                variableInformation.adderVariables = assertAdder(context, solver, variableInformation);
                variableInformation.auxiliaryVariables.push_back(assertLessOrEqualKRelaxed(context, solver, variableInformation.adderVariables, 0));
                
                // (7) Add constraints that cut off a lot of suboptimal solutions.
                LOG4CPLUS_DEBUG(logger, "Asserting cuts.");
                assertExplicitCuts(labeledMdp, psiStates, variableInformation, relevancyInformation, context, solver);
                LOG4CPLUS_DEBUG(logger, "Asserted explicit cuts.");
                assertSymbolicCuts(program, labeledMdp, variableInformation, relevancyInformation, context, solver);
                LOG4CPLUS_DEBUG(logger, "Asserted symbolic cuts.");
                
                // As we are done with the setup at this point, stop the clock for the setup time.
                totalSetupTime = std::chrono::high_resolution_clock::now() - setupTimeClock;
                
                // (8) Find the smallest set of commands that satisfies all constraints. If the probability of
                // satisfying phi until psi exceeds the given threshold, the set of labels is minimal and can be returned.
                // Otherwise, the current solution has to be ruled out and the next smallest solution is retrieved from
                // the solver.
                
                // Set up some variables for the iterations.
                storm::storage::VectorSet<uint_fast64_t> commandSet(relevancyInformation.relevantLabels);
                bool done = false;
                uint_fast64_t iterations = 0;
                uint_fast64_t currentBound = 0;
                maximalReachabilityProbability = 0;
                auto iterationTimer = std::chrono::high_resolution_clock::now();
                uint_fast64_t zeroProbabilityCount = 0;
                do {
                    LOG4CPLUS_DEBUG(logger, "Computing minimal command set.");
                    solverClock = std::chrono::high_resolution_clock::now();
                    commandSet = findSmallestCommandSet(context, solver, variableInformation, currentBound);
                    totalSolverTime += std::chrono::high_resolution_clock::now() - solverClock;
                    LOG4CPLUS_DEBUG(logger, "Computed minimal command set of size " << (commandSet.size() + relevancyInformation.knownLabels.size()) << ".");
                    
                    // Restrict the given MDP to the current set of labels and compute the reachability probability.
                    modelCheckingClock = std::chrono::high_resolution_clock::now();
                    commandSet.insert(relevancyInformation.knownLabels.begin(), relevancyInformation.knownLabels.end());
                    storm::models::Mdp<T> subMdp = labeledMdp.restrictChoiceLabels(commandSet);
                    storm::modelchecker::prctl::SparseMdpPrctlModelChecker<T> modelchecker(subMdp, new storm::solver::GmmxxNondeterministicLinearEquationSolver<T>());
                    LOG4CPLUS_DEBUG(logger, "Invoking model checker.");
                    std::vector<T> result = modelchecker.checkUntil(false, phiStates, psiStates, false, nullptr);
                    LOG4CPLUS_DEBUG(logger, "Computed model checking results.");
                    totalModelCheckingTime += std::chrono::high_resolution_clock::now() - modelCheckingClock;
                    
                    // Now determine the maximal reachability probability by checking all initial states.
                    maximalReachabilityProbability = 0;
                    for (auto state : labeledMdp.getInitialStates()) {
                        maximalReachabilityProbability = std::max(maximalReachabilityProbability, result[state]);
                    }
                    
                    // Depending on whether the threshold was successfully achieved or not, we proceed by either analyzing the bad solution or stopping the iteration process.
                    analysisClock = std::chrono::high_resolution_clock::now();
                    if ((strictBound && maximalReachabilityProbability < probabilityThreshold) || (!strictBound && maximalReachabilityProbability <= probabilityThreshold)) {
                        if (maximalReachabilityProbability == 0) {
                            ++zeroProbabilityCount;
                            
                            // If there was no target state reachable, analyze the solution and guide the solver into the right direction.
                            analyzeZeroProbabilitySolution(context, solver, subMdp, labeledMdp, phiStates, psiStates, commandSet, variableInformation, relevancyInformation);
                        } else {
                            // If the reachability probability was greater than zero (i.e. there is a reachable target state), but the probability was insufficient to exceed
                            // the given threshold, we analyze the solution and try to guide the solver into the right direction.
                            analyzeInsufficientProbabilitySolution(context, solver, subMdp, labeledMdp, phiStates, psiStates, commandSet, variableInformation, relevancyInformation);
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
                if (storm::settings::Settings::getInstance()->isSet("stats")) {
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

                // (9) Return the resulting command set after undefining the constants.
                storm::utility::ir::undefineUndefinedConstants(program);
                
                return commandSet;
#else
                throw storm::exceptions::NotImplementedException() << "This functionality is unavailable since StoRM has been compiled without support for Z3.";
#endif
            }
            
            static void computeCounterexample(storm::ir::Program program, std::string const& constantDefinitionString, storm::models::Mdp<T> const& labeledMdp,  storm::property::prctl::AbstractPrctlFormula<double> const* formulaPtr) {
#ifdef STORM_HAVE_Z3
                std::cout << std::endl << "Generating minimal label counterexample for formula " << formulaPtr->toString() << std::endl;
                // First, we need to check whether the current formula is an Until-Formula.
                storm::property::prctl::ProbabilisticBoundOperator<double> const* probBoundFormula = dynamic_cast<storm::property::prctl::ProbabilisticBoundOperator<double> const*>(formulaPtr);
                if (probBoundFormula == nullptr) {
                    LOG4CPLUS_ERROR(logger, "Illegal formula " << probBoundFormula->toString() << " for counterexample generation.");
                    throw storm::exceptions::InvalidPropertyException() << "Illegal formula " << probBoundFormula->toString() << " for counterexample generation.";
                }
                
                // Check whether we were given an upper bound, because counterexample generation is limited to this case.
                if (probBoundFormula->getComparisonOperator() != storm::property::ComparisonType::LESS && probBoundFormula->getComparisonOperator() != storm::property::ComparisonType::LESS_EQUAL) {
                    LOG4CPLUS_ERROR(logger, "Illegal comparison operator in formula " << probBoundFormula->toString() << ". Only upper bounds are supported for counterexample generation.");
                    throw storm::exceptions::InvalidPropertyException() << "Illegal comparison operator in formula " << probBoundFormula->toString() << ". Only upper bounds are supported for counterexample generation.";
                }
                bool strictBound = probBoundFormula->getComparisonOperator() == storm::property::ComparisonType::LESS;
                
                // Now derive the probability threshold we need to exceed as well as the phi and psi states. Simultaneously, check whether the formula is of a valid shape.
                double bound = probBoundFormula->getBound();
                storm::property::prctl::AbstractPathFormula<double> const& pathFormula = probBoundFormula->getPathFormula();
                storm::storage::BitVector phiStates;
                storm::storage::BitVector psiStates;
                storm::modelchecker::prctl::SparseMdpPrctlModelChecker<T> modelchecker(labeledMdp, new storm::solver::GmmxxNondeterministicLinearEquationSolver<T>());
                try {
                    storm::property::prctl::Until<double> const& untilFormula = dynamic_cast<storm::property::prctl::Until<double> const&>(pathFormula);
                    
                    phiStates = untilFormula.getLeft().check(modelchecker);
                    psiStates = untilFormula.getRight().check(modelchecker);
                } catch (std::bad_cast const& e) {
                    // If the nested formula was not an until formula, it remains to check whether it's an eventually formula.
                    try {
                        storm::property::prctl::Eventually<double> const& eventuallyFormula = dynamic_cast<storm::property::prctl::Eventually<double> const&>(pathFormula);
                        
                        phiStates = storm::storage::BitVector(labeledMdp.getNumberOfStates(), true);
                        psiStates = eventuallyFormula.getChild().check(modelchecker);
                    } catch (std::bad_cast const& e) {
                        // If the nested formula is neither an until nor a finally formula, we throw an exception.
                        throw storm::exceptions::InvalidPropertyException() << "Formula nested inside probability bound operator must be an until or eventually formula for counterexample generation.";
                    }
                }
                
                // Delegate the actual computation work to the function of equal name.
                auto startTime = std::chrono::high_resolution_clock::now();
                auto labelSet = getMinimalCommandSet(program, constantDefinitionString, labeledMdp, phiStates, psiStates, bound, strictBound, true);
                auto endTime = std::chrono::high_resolution_clock::now();
                std::cout << std::endl << "Computed minimal label set of size " << labelSet.size() << " in " << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << "ms." << std::endl;
                
                std::cout << "Resulting program:" << std::endl << std::endl;
                storm::ir::Program restrictedProgram(program);
                restrictedProgram.restrictCommands(labelSet);
                std::cout << restrictedProgram.toString() << std::endl;
                std::cout << std::endl << "-------------------------------------------" << std::endl;
                
#else
                throw storm::exceptions::NotImplementedException() << "This functionality is unavailable since StoRM has been compiled without support for Z3.";
#endif
            }
            
        };
        
    } // namespace counterexamples
} // namespace storm

#endif /* STORM_COUNTEREXAMPLES_SMTMINIMALCOMMANDSETGENERATOR_MDP_H_ */
