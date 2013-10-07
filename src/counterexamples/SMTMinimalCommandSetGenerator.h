/*
 * SMTMinimalCommandSetGenerator.h
 *
 *  Created on: 01.10.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_COUNTEREXAMPLES_SMTMINIMALCOMMANDSETGENERATOR_MDP_H_
#define STORM_COUNTEREXAMPLES_SMTMINIMALCOMMANDSETGENERATOR_MDP_H_

// To detect whether the usage of Z3 is possible, this include is neccessary.
#include "storm-config.h"

// If we have Z3 available, we have to include the C++ header.
#ifdef STORM_HAVE_Z3
#include "z3++.h"
#endif

#include "src/ir/Program.h"
#include "src/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "src/solver/GmmxxNondeterministicLinearEquationSolver.h"

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
                std::unordered_map<uint_fast64_t, std::list<uint_fast64_t>> relevantChoicesForRelevantStates;
            };
            
            struct VariableInformation {
                std::vector<z3::expr> labelVariables;
                std::vector<z3::expr> auxiliaryVariables;
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
                
                LOG4CPLUS_DEBUG(logger, "Found " << relevancyInformation.relevantLabels.size() << " relevant labels.");
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
                    
                    variableInformation.auxiliaryVariables.push_back(context.bool_const(variableName.str().c_str()));
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
                    solver.add(!variableInformation.labelVariables[index] || variableInformation.auxiliaryVariables[index]);
                }
                
                std::vector<std::set<uint_fast64_t>> const& choiceLabeling = labeledMdp.getChoiceLabeling();
                storm::storage::SparseMatrix<T> const& transitionMatrix = labeledMdp.getTransitionMatrix();

                // Assert that at least one of the labels of one of the relevant initial states is taken.
                std::vector<z3::expr> expressionVector;
                bool firstAssignment = true;
                for (auto state : labeledMdp.getInitialStates()) {
                    if (relevancyInformation.relevantStates.get(state)) {
                        for (auto const& choice : relevancyInformation.relevantChoicesForRelevantStates.at(state)) {
                            for (auto const& label : choiceLabeling[choice]) {
                                z3::expr labelExpression = variableInformation.labelVariables.at(variableInformation.labelToIndexMap.at(label));
                                if (firstAssignment) {
                                    expressionVector.push_back(labelExpression);
                                    firstAssignment = false;
                                } else {
                                    expressionVector.back() = expressionVector.back() && labelExpression;
                                }
                            }
                        }
                    }
                }
                assertDisjunction(context, solver, expressionVector);
                
                // Assert that at least one of the labels that are selected can reach a target state in one step.
                storm::storage::SparseMatrix<bool> backwardTransitions = labeledMdp.getBackwardTransitions();

                // Compute the set of predecessors of target states.
                std::unordered_set<uint_fast64_t> predecessors;
                for (auto state : psiStates) {
                    for (typename storm::storage::SparseMatrix<T>::ConstIndexIterator predecessorIt = backwardTransitions.constColumnIteratorBegin(state); predecessorIt != backwardTransitions.constColumnIteratorEnd(state); ++predecessorIt) {
                        if (state != *predecessorIt) {
                            predecessors.insert(*predecessorIt);
                        }
                    }
                }

                expressionVector.clear();
                firstAssignment = true;
                for (auto state : predecessors) {
                    for (auto choice : relevancyInformation.relevantChoicesForRelevantStates.at(state)) {
                        for (typename storm::storage::SparseMatrix<T>::ConstIndexIterator successorIt = transitionMatrix.constColumnIteratorBegin(choice); successorIt != transitionMatrix.constColumnIteratorEnd(choice); ++successorIt) {
                            if (psiStates.get(*successorIt)) {
                                for (auto const& label : choiceLabeling[choice]) {
                                    z3::expr labelExpression = variableInformation.labelVariables.at(variableInformation.labelToIndexMap.at(label));
                                    if (firstAssignment) {
                                        expressionVector.push_back(labelExpression);
                                        firstAssignment = false;
                                    } else {
                                        expressionVector.back() = expressionVector.back() && labelExpression;
                                    }
                                }
                            }
                        }
                    }
                }
                assertDisjunction(context, solver, expressionVector);
            }
            
            /*!
             * Asserts cuts that rule out a lot of suboptimal solutions.
             *
             * @param program The program for which to derive the cuts.
             * @param context The Z3 context in which to build the expressions.
             * @param solver The solver to use for the satisfiability evaluation.
             */
            static void assertCuts(storm::ir::Program const& program, z3::context& context, z3::solver& solver) {
                // TODO.
            }

            /*!
             * Asserts that the disjunction of the given formulae holds.
             *
             * @param context The Z3 context in which to build the expressions.
             * @param solver The solver to use for the satisfiability evaluation.
             * @param formulaVector A vector of expressions that shall form the disjunction.
             */
            static void assertDisjunction(z3::context& context, z3::solver& solver, std::vector<z3::expr> const& formulaVector) {
                z3::expr disjunction(context);
                for (uint_fast64_t i = 0; i < formulaVector.size(); ++i) {
                    if (i == 0) {
                        disjunction = formulaVector[i];
                    } else {
                        disjunction = disjunction || formulaVector[i];
                    }
                }
                solver.add(disjunction);
            }
            
            /*!
             * Asserts that at most one of the blocking variables may be true at any time.
             *
             * @param context The Z3 context in which to build the expressions.
             * @param solver The solver to use for the satisfiability evaluation.
             * @param blockingVariables A vector of variables out of which only one may be true.
             */
            static void assertAtMostOne(z3::context& context, z3::solver& solver, std::vector<z3::expr> const& blockingVariables) {
                for (uint_fast64_t i = 0; i < blockingVariables.size(); ++i) {
                    for (uint_fast64_t j = i + 1; j < blockingVariables.size(); ++j) {
                        solver.add(!blockingVariables[i] || !blockingVariables[j]);
                    }
                }
            }

            
            /*!
             * Performs one Fu-Malik-Maxsat step.
             *
             * @param context The Z3 context in which to build the expressions.
             * @param solver The solver to use for the satisfiability evaluation.
             * @param variableInformation A structure with information about the variables for the labels.
             * @return True iff the constraint system was satisfiable.
             */
            static bool fuMalikMaxsatStep(z3::context& context, z3::solver& solver, VariableInformation& variableInformation, std::vector<z3::expr>& softConstraints, uint_fast64_t& nextFreeVariableIndex) {
                z3::expr_vector assumptions(context);
                for (auto const& auxVariable : variableInformation.auxiliaryVariables) {
                    assumptions.push_back(!auxVariable);
                }
                                
                // Check whether the assumptions are satisfiable.
                z3::check_result result = solver.check(assumptions);
                
                if (result == z3::check_result::sat) {
                    return true;
                } else {
                    z3::expr_vector unsatCore = solver.unsat_core();
                    
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
                                variableInformation.auxiliaryVariables[softConstraintIndex] = context.bool_const(variableName.str().c_str());
                                
                                softConstraints[softConstraintIndex] = softConstraints[softConstraintIndex] || blockingVariables.back();
                                
                                solver.add(softConstraints[softConstraintIndex] || variableInformation.auxiliaryVariables[softConstraintIndex]);
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

            
            /*!
             * Finds the smallest set of labels such that the constraint system of the solver is still satisfiable.
             *
             * @param context The Z3 context in which to build the expressions.
             * @param solver The solver to use for the satisfiability evaluation.
             * @param variableInformation A structure with information about the variables for the labels.
             * @return The smallest set of labels such that the constraint system of the solver is still satisfiable.
             */
            static std::set<uint_fast64_t> findSmallestCommandSet(z3::context& context, z3::solver& solver, VariableInformation& variableInformation, std::vector<z3::expr>& softConstraints, uint_fast64_t& nextFreeVariableIndex) {
                for (uint_fast64_t i = 0; ; ++i) {
                    if (fuMalikMaxsatStep(context, solver, variableInformation, softConstraints, nextFreeVariableIndex)) {
                        break;
                    }
                }
                
                // Now we are ready to construct the label set from the model of the solver.
                std::set<uint_fast64_t> result;
                z3::model model = solver.get_model();
                for (auto const& labelIndexPair : variableInformation.labelToIndexMap) {
                    z3::expr value = model.eval(variableInformation.labelVariables[labelIndexPair.second]);
                    
                    std::stringstream resultStream;
                    resultStream << value;
                    if (resultStream.str() == "true") {
                        result.insert(labelIndexPair.first);
                    } else if (resultStream.str() == "false") {
                        // Nothing to do in this case.
                    } else {
                        throw storm::exceptions::InvalidStateException() << "Could not retrieve value of boolean variable.";
                    }
                }
                
                return result;
            }
#endif
            
        public:
            static std::set<uint_fast64_t> getMinimalCommandSet(storm::ir::Program const& program, storm::models::Mdp<T> const& labeledMdp, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, double probabilityThreshold, bool checkThresholdFeasible = false) {
#ifdef STORM_HAVE_Z3
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
                
                // (5) After all variables have been created, create a solver for that context.
                z3::solver solver(context);

                // (5) Build the initial constraint system.
                assertInitialConstraints(program, labeledMdp, psiStates, context, solver, variableInformation, relevancyInformation);
                
                // (6) Add constraints that cut off a lot of suboptimal solutions.
                assertCuts(program, context, solver);
                
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
                std::set<uint_fast64_t> commandSet;
                double maximalReachabilityProbability = 0;
                bool done = false;
                uint_fast64_t iterations = 0;
                do {
                    commandSet = findSmallestCommandSet(context, solver, variableInformation, softConstraints, nextFreeVariableIndex);
                    
                    // Restrict the given MDP to the current set of labels and compute the reachability probability.
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
                    std::cout << "Achieved probability: " << maximalReachabilityProbability << " with " << commandSet.size() << " commands in iteration " << iterations << "." << std::endl;
                    ++iterations;
                } while (!done);
                
                std::cout << "Achieved probability: " << maximalReachabilityProbability << " with " << commandSet.size() << " commands." << std::endl;
                std::cout << "Taken commands are:" << std::endl;
                for (auto label : commandSet) {
                    std::cout << label << ", ";
                }
                std::cout << std::endl;
                
                // (8) Return the resulting command set.
                return commandSet;
                
#else
                throw storm::exceptions::NotImplementedException() << "This functionality is unavailable since StoRM has been compiled without support for Z3.";
#endif
            }
            
        };
        
    } // namespace counterexamples
} // namespace storm

#endif /* STORM_COUNTEREXAMPLES_SMTMINIMALCOMMANDSETGENERATOR_MDP_H_ */
