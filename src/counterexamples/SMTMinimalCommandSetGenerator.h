/*
 * SMTMinimalCommandSetGenerator.h
 *
 *  Created on: 01.10.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_COUNTEREXAMPLES_SMTMINIMALCOMMANDSETGENERATOR_MDP_H_
#define STORM_COUNTEREXAMPLES_SMTMINIMALCOMMANDSETGENERATOR_MDP_H_

#include <queue>

// To detect whether the usage of Z3 is possible, this include is neccessary.
#include "storm-config.h"

// If we have Z3 available, we have to include the C++ header.
#ifdef STORM_HAVE_Z3
#include "z3++.h"
#endif

#include "src/adapters/ExplicitModelAdapter.h"
#include "src/adapters/Z3ExpressionAdapter.h"
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
                storm::storage::BitVector relevantStates;
                std::set<uint_fast64_t> relevantLabels;
                std::set<uint_fast64_t> knownLabels;
                std::unordered_map<uint_fast64_t, std::list<uint_fast64_t>> relevantChoicesForRelevantStates;
            };
            
            struct VariableInformation {
                std::vector<z3::expr> labelVariables;
                std::vector<z3::expr> originalAuxiliaryVariables;
                std::vector<z3::expr> auxiliaryVariables;
                std::vector<z3::expr> adderVariables;
                std::map<uint_fast64_t, uint_fast64_t> labelToIndexMap;
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

                // Retrieve some references for convenient access.
                storm::storage::SparseMatrix<T> const& transitionMatrix = labeledMdp.getTransitionMatrix();
                std::vector<uint_fast64_t> const& nondeterministicChoiceIndices = labeledMdp.getNondeterministicChoiceIndices();
                std::vector<std::set<uint_fast64_t>> const& choiceLabeling = labeledMdp.getChoiceLabeling();

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
                    std::set<uint_fast64_t> remainingLabels;
                    std::set_difference(relevancyInformation.relevantLabels.begin(), relevancyInformation.relevantLabels.end(), relevancyInformation.knownLabels.begin(), relevancyInformation.knownLabels.end(), std::inserter(remainingLabels, remainingLabels.begin()));
                    relevancyInformation.relevantLabels = remainingLabels;
                }
                
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
            static VariableInformation createExpressionsForRelevantLabels(z3::context& context, std::set<uint_fast64_t> const& relevantLabels) {
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
             * Asserts the constraints that are initially known.
             *
             * @param program The program for which to build the constraints.
             * @param labeledMdp The MDP that results from the given program.
             * @param context The Z3 context in which to build the expressions.
             * @param solver The solver in which to assert the constraints.
             * @param variableInformation A structure with information about the variables for the labels.
             */
            static void assertInitialConstraints(storm::ir::Program const& program, storm::models::Mdp<T> const& labeledMdp, storm::storage::BitVector const& psiStates, z3::context& context, z3::solver& solver, VariableInformation const& variableInformation, RelevancyInformation const& relevancyInformation) {
                // Assert that at least one of the labels must be taken.
                z3::expr formula = variableInformation.labelVariables.at(0);
                for (uint_fast64_t index = 1; index < variableInformation.labelVariables.size(); ++index) {
                    formula = formula || variableInformation.labelVariables.at(index);
                }
                solver.add(formula);

                for (uint_fast64_t index = 0; index < variableInformation.labelVariables.size(); ++index) {
                    solver.add(!variableInformation.labelVariables[index] || variableInformation.originalAuxiliaryVariables[index]);
                }
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
                // * identify labels that can be found on each path to the target states.
                
                std::set<uint_fast64_t> initialLabels;
                std::map<uint_fast64_t, std::set<uint_fast64_t>> precedingLabels;
                std::set<uint_fast64_t> targetLabels;
                std::map<uint_fast64_t, std::set<uint_fast64_t>> followingLabels;
                
                // Get some data from the MDP for convenient access.
                storm::storage::SparseMatrix<T> const& transitionMatrix = labeledMdp.getTransitionMatrix();
                std::vector<uint_fast64_t> const& nondeterministicChoiceIndices = labeledMdp.getNondeterministicChoiceIndices();
                storm::storage::BitVector const& initialStates = labeledMdp.getInitialStates();
                std::vector<std::set<uint_fast64_t>> const& choiceLabeling = labeledMdp.getChoiceLabeling();
                storm::storage::SparseMatrix<bool> backwardTransitions = labeledMdp.getBackwardTransitions();
                
                for (auto currentState : relevancyInformation.relevantStates) {
                    for (auto currentChoice : relevancyInformation.relevantChoicesForRelevantStates.at(currentState)) {
                        
                        // If the state is initial, we need to add all the choice labels to the initial label set.
                        if (initialStates.get(currentState)) {
                            for (auto label : choiceLabeling[currentChoice]) {
                                initialLabels.insert(label);
                            }
                        }
                        
                        // Iterate over successors and add relevant choices of relevant successors to the following label set.
                        bool canReachTargetState = false;
                        for (typename storm::storage::SparseMatrix<T>::ConstIndexIterator successorIt = transitionMatrix.constColumnIteratorBegin(currentChoice), successorIte = transitionMatrix.constColumnIteratorEnd(currentChoice); successorIt != successorIte; ++successorIt) {
                            if (relevancyInformation.relevantStates.get(*successorIt)) {
                                for (auto relevantChoice : relevancyInformation.relevantChoicesForRelevantStates.at(*successorIt)) {
                                    for (auto labelToAdd : choiceLabeling[relevantChoice]) {
                                        for (auto labelForWhichToAdd : choiceLabeling[currentChoice]) {
                                            followingLabels[labelForWhichToAdd].insert(labelToAdd);
                                        }
                                    }
                                }
                            } else if (psiStates.get(*successorIt)) {
                                canReachTargetState = true;
                            }
                        }
                        
                        // If the choice can reach a target state directly, we add all the labels to the target label set.
                        if (canReachTargetState) {
                            for (auto label : choiceLabeling[currentChoice]) {
                                targetLabels.insert(label);
                            }
                        }
                        
                        // Iterate over predecessors and add all choices that target the current state to the preceding
                        // label set of all labels of all relevant choices of the current state.
                        for (typename storm::storage::SparseMatrix<T>::ConstIndexIterator predecessorIt = backwardTransitions.constColumnIteratorBegin(currentState), predecessorIte = backwardTransitions.constColumnIteratorEnd(currentState); predecessorIt != predecessorIte; ++predecessorIt) {
                            for (auto predecessorChoice : relevancyInformation.relevantChoicesForRelevantStates.at(*predecessorIt)) {
                                bool choiceTargetsCurrentState = false;
                                for (typename storm::storage::SparseMatrix<T>::ConstIndexIterator successorIt = transitionMatrix.constColumnIteratorBegin(predecessorChoice), successorIte = transitionMatrix.constColumnIteratorEnd(predecessorChoice); successorIt != successorIte; ++successorIt) {
                                    if (*successorIt == currentState) {
                                        choiceTargetsCurrentState = true;
                                    }
                                }
                                
                                if (choiceTargetsCurrentState) {
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
                
                std::vector<z3::expr> formulae;
                
                // Start by asserting that we take at least one initial label. We may do so only if there is no initial
                // label that is already known. Otherwise this condition would be too strong.
                std::set<uint_fast64_t> intersection;
                std::set_intersection(initialLabels.begin(), initialLabels.end(), relevancyInformation.knownLabels.begin(), relevancyInformation.knownLabels.end(), std::inserter(intersection, intersection.begin()));
                if (intersection.empty()) {
                    for (auto label : initialLabels) {
                        formulae.push_back(variableInformation.labelVariables.at(variableInformation.labelToIndexMap.at(label)));
                    }
                    assertDisjunction(context, solver, formulae);
                    formulae.clear();
                } else {
                    // If the intersection was non-empty, we clear the set so we can re-use it later.
                    intersection.clear();
                }
                
                // Likewise, if no target label is known, we may assert that there is at least one.
                std::set_intersection(targetLabels.begin(), targetLabels.end(), relevancyInformation.knownLabels.begin(), relevancyInformation.knownLabels.end(), std::inserter(intersection, intersection.begin()));
                if (intersection.empty()) {
                    for (auto label : targetLabels) {
                        formulae.push_back(variableInformation.labelVariables.at(variableInformation.labelToIndexMap.at(label)));
                    }
                    assertDisjunction(context, solver, formulae);
                } else {
                    // If the intersection was non-empty, we clear the set so we can re-use it later.
                    intersection.clear();
                }
                
                // Now assert that for each non-target label, we take a following label.
                for (auto const& labelSetPair : followingLabels) {
                    formulae.clear();
                    if (targetLabels.find(labelSetPair.first) == targetLabels.end()) {
                        // Also, if there is a known label that may follow the current label, we don't need to assert
                        // anything here.
                        std::set_intersection(labelSetPair.second.begin(), labelSetPair.second.end(), relevancyInformation.knownLabels.begin(), relevancyInformation.knownLabels.end(), std::inserter(intersection, intersection.begin()));
                        if (intersection.empty()) {
                            formulae.push_back(!variableInformation.labelVariables.at(variableInformation.labelToIndexMap.at(labelSetPair.first)));
                            for (auto followingLabel : labelSetPair.second) {
                                if (followingLabel != labelSetPair.first) {
                                    formulae.push_back(variableInformation.labelVariables.at(variableInformation.labelToIndexMap.at(followingLabel)));
                                }
                            }
                        } else {
                            // If the intersection was non-empty, we clear the set so we can re-use it later.
                            intersection.clear();
                        }
                    }
                    if (formulae.size() > 0) {
                        assertDisjunction(context, solver, formulae);
                    }
                }
                
                // FIXME: This is currently disabled because it derives less information than the following backward cuts.
                // Consequently, assert that for each non-initial label, we take preceding command.
//                for (auto const& labelSetPair : precedingLabels) {
//                    formulae.clear();
//                    if (initialLabels.find(labelSetPair.first) == initialLabels.end()) {
//                        // Also, if there is a known label that may follow the current label, we don't need to assert
//                        // anything here.
//                        std::set_intersection(labelSetPair.second.begin(), labelSetPair.second.end(), relevancyInformation.knownLabels.begin(), relevancyInformation.knownLabels.end(), std::inserter(intersection, intersection.begin()));
//                        if (intersection.empty()) {
//                            formulae.push_back(!variableInformation.labelVariables.at(variableInformation.labelToIndexMap.at(labelSetPair.first)));
//                            for (auto followingLabel : labelSetPair.second) {
//                                formulae.push_back(variableInformation.labelVariables.at(variableInformation.labelToIndexMap.at(followingLabel)));
//                            }
//                        } else {
//                            // If the intersection was non-empty, we clear the set so we can re-use it later.
//                            intersection.clear();
//                        }
//                    }
//                    if (formulae.size() > 0) {
//                        assertDisjunction(context, solver, formulae);
//                    }
//                }
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
                // FIXME: Include synchronisation cuts.
                // FIXME: Fix backward cuts in the presence of synchronizing actions.
                std::map<uint_fast64_t, std::set<uint_fast64_t>> precedingLabels;

                // Get some data from the MDP for convenient access.
                storm::storage::SparseMatrix<T> const& transitionMatrix = labeledMdp.getTransitionMatrix();
                std::vector<std::set<uint_fast64_t>> const& choiceLabeling = labeledMdp.getChoiceLabeling();
                storm::storage::SparseMatrix<bool> backwardTransitions = labeledMdp.getBackwardTransitions();
                
                // Compute the set of labels that may precede a given action.
                for (auto currentState : relevancyInformation.relevantStates) {
                    for (auto currentChoice : relevancyInformation.relevantChoicesForRelevantStates.at(currentState)) {
                        // Iterate over predecessors and add all choices that target the current state to the preceding
                        // label set of all labels of all relevant choices of the current state.
                        for (typename storm::storage::SparseMatrix<T>::ConstIndexIterator predecessorIt = backwardTransitions.constColumnIteratorBegin(currentState), predecessorIte = backwardTransitions.constColumnIteratorEnd(currentState); predecessorIt != predecessorIte; ++predecessorIt) {
                            for (auto predecessorChoice : relevancyInformation.relevantChoicesForRelevantStates.at(*predecessorIt)) {
                                bool choiceTargetsCurrentState = false;
                                for (typename storm::storage::SparseMatrix<T>::ConstIndexIterator successorIt = transitionMatrix.constColumnIteratorBegin(predecessorChoice), successorIte = transitionMatrix.constColumnIteratorEnd(predecessorChoice); successorIt != successorIte; ++successorIt) {
                                    if (*successorIt == currentState) {
                                        choiceTargetsCurrentState = true;
                                    }
                                }
                                
                                if (choiceTargetsCurrentState) {
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
                
                // FIXME: The following procedure to assert backward cuts is not correct in the presence of synchronizing
                // actions, because it may be the case that several synchronizing commands are necessary to enable another
                // command and not just one.
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
                
                std::map<uint_fast64_t, std::set<uint_fast64_t>> backwardImplications;
                
                // Now check for possible backward cuts.
                for (uint_fast64_t moduleIndex = 0; moduleIndex < program.getNumberOfModules(); ++moduleIndex) {
                    storm::ir::Module const& module = program.getModule(moduleIndex);
                    
                    for (uint_fast64_t commandIndex = 0; commandIndex < module.getNumberOfCommands(); ++commandIndex) {
                        storm::ir::Command const& command = module.getCommand(commandIndex);

                        // If the label of the command is not relevant, skip it entirely.
                        if (relevancyInformation.relevantLabels.find(command.getGlobalIndex()) == relevancyInformation.relevantLabels.end()) continue;
                        
                        // Save the state of the solver so we can easily backtrack.
                        localSolver.push();
                        
                        // Check if the command is enabled in the initial state.
                        localSolver.add(expressionAdapter.translateExpression(command.getGuard()));
                        localSolver.add(initialStateExpression);
                        
                        z3::check_result checkResult = localSolver.check();
                        localSolver.pop();
                        localSolver.push();

                        if (checkResult == z3::unsat) {
                            localSolver.add(!expressionAdapter.translateExpression(command.getGuard()));
                            localSolver.push();
                            
                            // We need to check all commands of the all modules, because they could enable the current
                            // command via a global variable.
                            for (uint_fast64_t otherModuleIndex = 0; otherModuleIndex < program.getNumberOfModules(); ++otherModuleIndex) {
                                storm::ir::Module const& otherModule = program.getModule(otherModuleIndex);
                                
                                for (uint_fast64_t otherCommandIndex = 0; otherCommandIndex < otherModule.getNumberOfCommands(); ++otherCommandIndex) {
                                    storm::ir::Command const& otherCommand = otherModule.getCommand(otherCommandIndex);
                                    
                                    // We don't need to consider irrelevant commands and the command itself.
                                    if (relevancyInformation.relevantLabels.find(otherCommand.getGlobalIndex()) == relevancyInformation.relevantLabels.end()
                                        && relevancyInformation.knownLabels.find(otherCommand.getGlobalIndex()) == relevancyInformation.knownLabels.end()) {
                                        continue;
                                    }
                                    if (moduleIndex == otherModuleIndex && commandIndex == otherCommandIndex) continue;
                                    
                                    std::vector<z3::expr> formulae;
                                    formulae.reserve(otherCommand.getNumberOfUpdates());
                                    
                                    localSolver.push();
                                    
                                    for (uint_fast64_t updateIndex = 0; updateIndex < otherCommand.getNumberOfUpdates(); ++updateIndex) {
                                        std::unique_ptr<storm::ir::expressions::BaseExpression> weakestPrecondition = storm::utility::ir::getWeakestPrecondition(command.getGuard(), {otherCommand.getUpdate(updateIndex)});
                                        
                                        formulae.push_back(expressionAdapter.translateExpression(weakestPrecondition));
                                    }
                                    
                                    assertDisjunction(localContext, localSolver, formulae);
                                    
                                    // If the assertions were satisfiable, this means the other command could successfully
                                    // enable the current command.
                                    if (localSolver.check() == z3::sat) {
                                        backwardImplications[command.getGlobalIndex()].insert(otherCommand.getGlobalIndex());
                                    }
                                    
                                    localSolver.pop();
                                }
                            }
                            
                            // Remove the negated guard from the solver assertions.
                            localSolver.pop();
                        }
                        
                        // Restore state of solver where only the variable bounds are asserted.
                        localSolver.pop();
                    }
                }
                
                std::vector<z3::expr> formulae;
                for (auto const& labelImplicationsPair : backwardImplications) {
                    // We only need to make this an implication if the label is not already known. If it is known,
                    // we can directly assert the disjunction of the implications.
                    if (relevancyInformation.knownLabels.find(labelImplicationsPair.first) == relevancyInformation.knownLabels.end()) {
                        formulae.push_back(!variableInformation.labelVariables.at(variableInformation.labelToIndexMap.at(labelImplicationsPair.first)));
                    }
                    
                    std::set<uint_fast64_t> actualImplications;
                    std::set_intersection(labelImplicationsPair.second.begin(), labelImplicationsPair.second.end(), precedingLabels.at(labelImplicationsPair.first).begin(), precedingLabels.at(labelImplicationsPair.first).end(), std::inserter(actualImplications, actualImplications.begin()));

                    // We should assert the implications if they are not already known to be true anyway.
                    std::set<uint_fast64_t> knownImplications;
                    std::set_intersection(actualImplications.begin(), actualImplications.end(), relevancyInformation.knownLabels.begin(), relevancyInformation.knownLabels.end(), std::inserter(knownImplications, knownImplications.begin()));

                    if (knownImplications.empty()) {
                        for (auto label : actualImplications) {
                            formulae.push_back(variableInformation.labelVariables.at(variableInformation.labelToIndexMap.at(label)));
                        }
                    
                        assertDisjunction(context, solver, formulae);
                        formulae.clear();
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
            
            static bool bitIsSet(uint64_t value, uint64_t index) {
                uint64_t mask = 1 << (index & 63);
                return (value & mask) != 0;
            }
            
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
            static void assertLessOrEqualOne(z3::context& context, z3::solver& solver, std::vector<z3::expr> const& input) {
                std::vector<z3::expr> tmp;
                tmp.reserve(input.size() - 1);
                for (uint_fast64_t index = 1; index < input.size(); ++index) {
                    tmp.push_back(!input[index]);
                }
                assertConjunction(context, solver, tmp);
            }
            
            /*!
             * Asserts that at most one of the blocking variables may be true at any time.
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
            static void ruleOutSolution(z3::context& context, z3::solver& solver, std::set<uint_fast64_t> const& commandSet, VariableInformation const& variableInformation) {
                std::map<uint_fast64_t, uint_fast64_t>::const_iterator labelIndexIterator = variableInformation.labelToIndexMap.begin();
                z3::expr blockSolutionExpression(context);
                if (commandSet.find(labelIndexIterator->first) != commandSet.end()) {
                    blockSolutionExpression = !variableInformation.labelVariables[labelIndexIterator->second];
                } else {
                    blockSolutionExpression = variableInformation.labelVariables[labelIndexIterator->second];
                }
                ++labelIndexIterator;

                for (; labelIndexIterator != variableInformation.labelToIndexMap.end(); ++labelIndexIterator) {
                    if (commandSet.find(labelIndexIterator->first) != commandSet.end()) {
                        blockSolutionExpression = blockSolutionExpression || !variableInformation.labelVariables[labelIndexIterator->second];
                    } else {
                        blockSolutionExpression = blockSolutionExpression || variableInformation.labelVariables[labelIndexIterator->second];
                    }
                }
                
                solver.add(blockSolutionExpression);
            }

            static std::set<uint_fast64_t> getUsedLabelSet(z3::context& context, z3::model const& model, VariableInformation const& variableInformation, std::vector<z3::expr> const& usedVariables) {
                std::set<uint_fast64_t> result;
                for (auto const& labelIndexPair : variableInformation.labelToIndexMap) {
                    z3::expr auxValue = model.eval(usedVariables.at(labelIndexPair.second));
                    
                    // Check whether the auxiliary variable was set or not.
                    if (eq(auxValue, context.bool_val(true))) {
                        result.insert(labelIndexPair.first);
                    } else if (eq(auxValue, context.bool_val(false))) {
                        // Nothing to do in this case.
                    } else if (eq(auxValue, usedVariables.at(labelIndexPair.second))) {
                        // If the auxiliary variable is a don't care, then we don't take the corresponding command.
                    } else {
                        throw storm::exceptions::InvalidStateException() << "Could not retrieve value of boolean variable from illegal value.";
                    }
                }
                return result;
            }
            
            
            /*!
             * Finds the smallest set of labels such that the constraint system of the solver is still satisfiable.
             *
             * @param context The Z3 context in which to build the expressions.
             * @param solver The solver to use for the satisfiability evaluation.
             * @param variableInformation A structure with information about the variables for the labels.
             * @return The smallest set of labels such that the constraint system of the solver is still satisfiable.
             */
            static std::set<uint_fast64_t> findSmallestCommandSet(z3::context& context, z3::solver& solver, VariableInformation& variableInformation, std::vector<z3::expr>& softConstraints, uint_fast64_t& currentBound, uint_fast64_t& nextFreeVariableIndex, bool useFuMalik = false) {
                if (useFuMalik) {
                    while (!fuMalikMaxsatStep(context, solver, variableInformation.auxiliaryVariables, softConstraints, nextFreeVariableIndex)) {
                        // Intentionally left empty.
                    }
                    
                    // Now we are ready to construct the label set from the model of the solver.
                    return getUsedLabelSet(context, solver.get_model(), variableInformation, variableInformation.originalAuxiliaryVariables);
                } else {
                    // Check if we can find a solution with the current bound.
                    z3::expr assumption = !variableInformation.auxiliaryVariables.back();
                    while (solver.check(1, &assumption) == z3::unsat) {
                        // If the constraints are unsatisfiable, we need to relax the last at-most-k constraint and
                        // try with an increased bound.
                        LOG4CPLUS_DEBUG(logger, "Constraint system is unsatisfiable with at most " << currentBound << " taken commands; increasing bound.");
                        solver.add(variableInformation.auxiliaryVariables.back());
                        variableInformation.auxiliaryVariables.push_back(assertLessOrEqualKRelaxed(context, solver, variableInformation.adderVariables, ++currentBound));
                        assumption = !variableInformation.auxiliaryVariables.back();
                    }
                    
                    // At this point we know that the constraint system was satisfiable, so compute the induced label
                    // set and return it.
                    return getUsedLabelSet(context, solver.get_model(), variableInformation, variableInformation.labelVariables);
                }
            }
            
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
#endif
            
        public:
            static std::set<uint_fast64_t> getMinimalCommandSet(storm::ir::Program program, std::string const& constantDefinitionString, storm::models::Mdp<T> const& labeledMdp, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, double probabilityThreshold, bool checkThresholdFeasible = false, bool useFuMalik = false) {
#ifdef STORM_HAVE_Z3
                storm::utility::ir::defineUndefinedConstants(program, constantDefinitionString);

                // (0) Check whether the MDP is indeed labeled.
                if (!labeledMdp.hasChoiceLabels()) {
                    throw storm::exceptions::InvalidArgumentException() << "Minimal command set generation is impossible for unlabeled model.";
                }
                
                // (1) FIXME: check whether its possible to exceed the threshold if checkThresholdFeasible is set.

                // (2) Identify all states and commands that are relevant, because only these need to be considered later.
                RelevancyInformation relevancyInformation = determineRelevantStatesAndLabels(labeledMdp, phiStates, psiStates);
                
                // (3) Create context for solver.
                z3::context context;
                
                // (4) Create the variables for the relevant commands.
                VariableInformation variableInformation = createExpressionsForRelevantLabels(context, relevancyInformation.relevantLabels);
                LOG4CPLUS_DEBUG(logger, "Created variables.");

                // (5) After all variables have been created, create a solver for that context.
                z3::solver solver(context);

                // (5) Build the initial constraint system.
                if (useFuMalik) {
                    assertInitialConstraints(program, labeledMdp, psiStates, context, solver, variableInformation, relevancyInformation);
                    LOG4CPLUS_DEBUG(logger, "Asserted initial constraints.");
                }

                // (6) If we are supposed to use the counter-circuit method, we need to assert the adder circuit.
                if (useFuMalik) {
                    variableInformation.auxiliaryVariables = variableInformation.originalAuxiliaryVariables;
                } else {
                    variableInformation.adderVariables = assertAdder(context, solver, variableInformation);
                    variableInformation.auxiliaryVariables.push_back(assertLessOrEqualKRelaxed(context, solver, variableInformation.adderVariables, 0));
                }
                
                // (6) Add constraints that cut off a lot of suboptimal solutions.
                assertExplicitCuts(labeledMdp, psiStates, variableInformation, relevancyInformation, context, solver);
                LOG4CPLUS_DEBUG(logger, "Asserted explicit cuts.");
                assertSymbolicCuts(program, labeledMdp, variableInformation, relevancyInformation, context, solver);
                LOG4CPLUS_DEBUG(logger, "Asserted symbolic cuts.");
                
                // (7) Find the smallest set of commands that satisfies all constraints. If the probability of
                // satisfying phi until psi exceeds the given threshold, the set of labels is minimal and can be returned.
                // Otherwise, the current solution has to be ruled out and the next smallest solution is retrieved from
                // the solver.
                
                // Start by building the initial vector of constraints out of which we want to satisfy maximally many.
                std::vector<z3::expr> softConstraints;
                softConstraints.reserve(variableInformation.labelVariables.size());
                for (auto const& labelExpr : variableInformation.labelVariables) {
                    softConstraints.push_back(!labelExpr);
                }
                
                // Create an index counter that keeps track of the next free index we can use for blocking variables.
                uint_fast64_t nextFreeVariableIndex = 0;
                
                // Keep track of the command set we used to achieve the current probability as well as the probability
                // itself.
                std::set<uint_fast64_t> commandSet(relevancyInformation.relevantLabels);
                double maximalReachabilityProbability = 0;
                bool done = false;
                uint_fast64_t iterations = 0;
                uint_fast64_t currentBound = 0;
                
                do {
                    LOG4CPLUS_DEBUG(logger, "Computing minimal command set.");
                    commandSet = findSmallestCommandSet(context, solver, variableInformation, softConstraints, currentBound, nextFreeVariableIndex, useFuMalik);
                    LOG4CPLUS_DEBUG(logger, "Computed minimal command set of size " << commandSet.size() << ".");
                    
                    // Restrict the given MDP to the current set of labels and compute the reachability probability.
                    commandSet.insert(relevancyInformation.knownLabels.begin(), relevancyInformation.knownLabels.end());
                    storm::models::Mdp<T> subMdp = labeledMdp.restrictChoiceLabels(commandSet);
                    storm::modelchecker::prctl::SparseMdpPrctlModelChecker<T> modelchecker(subMdp, new storm::solver::GmmxxNondeterministicLinearEquationSolver<T>());
                    LOG4CPLUS_DEBUG(logger, "Invoking model checker.");
                    std::vector<T> result = modelchecker.checkUntil(false, phiStates, psiStates, false, nullptr);
                    LOG4CPLUS_DEBUG(logger, "Computed model checking results.");
                    
                    // Now determine the maximal reachability probability by checking all initial states.
                    for (auto state : labeledMdp.getInitialStates()) {
                        maximalReachabilityProbability = std::max(maximalReachabilityProbability, result[state]);
                    }
                    
                    if (maximalReachabilityProbability <= probabilityThreshold) {
                        // In case we have not yet exceeded the given threshold, we have to rule out the current solution.
                        ruleOutSolution(context, solver, commandSet, variableInformation);
                    } else {
                        done = true;
                    }
                    ++iterations;
                } while (!done);
                LOG4CPLUS_INFO(logger, "Found minimal label set after " << iterations << " iterations.");
                
                // Verify the results.
                storm::ir::Program programCopy(program);
                programCopy.restrictCommands(commandSet);
                std::cout << programCopy.toString() << std::endl;
                
                // (8) Return the resulting command set after undefining the constants.
                storm::utility::ir::undefineUndefinedConstants(program);
                return commandSet;
                
#else
                throw storm::exceptions::NotImplementedException() << "This functionality is unavailable since StoRM has been compiled without support for Z3.";
#endif
            }
            
        };
        
    } // namespace counterexamples
} // namespace storm

#endif /* STORM_COUNTEREXAMPLES_SMTMINIMALCOMMANDSETGENERATOR_MDP_H_ */
