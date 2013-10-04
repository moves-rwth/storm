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
             * @return A set of relevant labels, where relevant is defined as above.
             */
            static std::set<uint_fast64_t> getRelevantLabels(storm::models::Mdp<T> const& labeledMdp, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates) {
                // Create result.
                std::set<uint_fast64_t> relevantLabels;
                
                // Compute all relevant states, i.e. states for which there exists a scheduler that has a non-zero
                // probabilitiy of satisfying phi until psi.
                storm::storage::SparseMatrix<bool> backwardTransitions = labeledMdp.getBackwardTransitions();
                storm::storage::BitVector relevantStates = storm::utility::graph::performProbGreater0E(labeledMdp, backwardTransitions, phiStates, psiStates);
                relevantStates &= ~psiStates;

                // Retrieve some references for convenient access.
                storm::storage::SparseMatrix<T> const& transitionMatrix = labeledMdp.getTransitionMatrix();
                std::vector<uint_fast64_t> const& nondeterministicChoiceIndices = labeledMdp.getNondeterministicChoiceIndices();
                std::vector<std::set<uint_fast64_t>> const& choiceLabeling = labeledMdp.getChoiceLabeling();

                // Now traverse all choices of all relevant states and check whether there is a successor target state.
                // If so, the associated labels become relevant. Also, if a choice of relevant state has at least one
                // relevant successor, the choice becomes relevant.
                for (auto state : relevantStates) {
                    for (uint_fast64_t row = nondeterministicChoiceIndices[state]; row < nondeterministicChoiceIndices[state + 1]; ++row) {
                        for (typename storm::storage::SparseMatrix<T>::ConstIndexIterator successorIt = transitionMatrix.constColumnIteratorBegin(row); successorIt != transitionMatrix.constColumnIteratorEnd(row); ++successorIt) {
                            // If there is a relevant successor, we need to add the labels of the current choice.
                            if (relevantStates.get(*successorIt) || psiStates.get(*successorIt)) {
                                for (auto const& label : choiceLabeling[row]) {
                                    relevantLabels.insert(label);
                                }
                            }
                        }
                    }
                }
                
                LOG4CPLUS_DEBUG(logger, "Found " << relevantLabels.size() << " relevant labels.");
                return relevantLabels;
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
             * @param context The Z3 context in which to build the expressions.
             * @param solver The solver in which to assert the constraints.
             * @param variableInformation A structure with information about the variables for the labels.
             */
            static void assertInitialConstraints(storm::ir::Program const& program, z3::context& context, z3::solver& solver, VariableInformation const& variableInformation) {
                // Assert that at least one of the labels must be taken.
                z3::expr formula = variableInformation.labelVariables.at(0);
                for (uint_fast64_t index = 1; index < variableInformation.labelVariables.size(); ++index) {
                    formula = formula || variableInformation.labelVariables.at(index);
                }
                solver.add(formula);
                
                for (uint_fast64_t index = 0; index < variableInformation.labelVariables.size(); ++index) {
                    solver.add(!variableInformation.labelVariables[index] || variableInformation.auxiliaryVariables[index]);
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
            static bool fuMalikMaxsatStep(z3::context& context, z3::solver& solver, VariableInformation const& variableInformation, uint_fast64_t& nextFreeVariableIndex) {
                z3::expr_vector assumptions(context);
                for (auto const& auxVariable : variableInformation.auxiliaryVariables) {
                    assumptions.push_back(!auxVariable);
                }
                
                std::cout << solver << std::endl;
                
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
                    
                    // Create fresh blocking variables.
                    for (uint_fast64_t i = 0; i < unsatCore.size(); ++i) {
                        variableName.clear();
                        variableName.str("");
                        variableName << "b" << nextFreeVariableIndex;
                        blockingVariables.push_back(context.bool_const(variableName.str().c_str()));
                        ++nextFreeVariableIndex;
                    }
                }
                
                return false;
            }
            
            /*!
             * Finds the smallest set of labels such that the constraint system of the solver is still satisfiable.
             *
             * @param context The Z3 context in which to build the expressions.
             * @param solver The solver to use for the satisfiability evaluation.
             * @param variableInformation A structure with information about the variables for the labels.
             * @return The smallest set of labels such that the constraint system of the solver is still satisfiable.
             */
            static std::set<uint_fast64_t> findSmallestCommandSet(z3::context& context, z3::solver& solver, VariableInformation const& variableInformation, uint_fast64_t& nextFreeVariableIndex) {
                for (uint_fast64_t i = 0;; ++i) {
                    if (fuMalikMaxsatStep(context, solver, variableInformation)) {
                        break;
                    }
                }
                
                // Now we are ready to construct the label set from the model of the solver.
                std::set<uint_fast64_t> result;
                
                for (auto const& labelIndexPair : variableInformation.labelToIndexMap) {
                    result.insert(labelIndexPair.first);
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

                // (2) Identify all commands that are relevant, because only these need to be considered later.
                std::set<uint_fast64_t> relevantCommands = getRelevantLabels(labeledMdp, phiStates, psiStates);
                
                // (3) Create context for solver.
                z3::context context;
                
                // (4) Create the variables for the relevant commands.
                VariableInformation variableInformation = createExpressionsForRelevantLabels(context, relevantCommands);
                
                // (5) After all variables have been created, create a solver for that context.
                z3::solver solver(context);

                // (5) Build the initial constraint system.
                assertInitialConstraints(program, context, solver, variableInformation);
                
                // (6) Find the smallest set of commands that satisfies all constraints. If the probability of
                // satisfying phi until psi exceeds the given threshold, the set of labels is minimal and can be returned.
                // Otherwise, the current solution has to be ruled out and the next smallest solution is retrieved from
                // the solver.
                double maximalReachabilityProbability = 0;
                // Create an index counter that keeps track of the next free index we can use for blocking variables.
                uint_fast64_t nextFreeVariableIndex = 0;
                std::set<uint_fast64_t> commandSet;
                do {
                    commandSet = findSmallestCommandSet(context, solver, variableInformation, nextFreeVariableIndex);
                    
                    storm::models::Mdp<T> subMdp = labeledMdp.restrictChoiceLabels(commandSet);
                    storm::modelchecker::prctl::SparseMdpPrctlModelChecker<T> modelchecker(subMdp, new storm::solver::GmmxxNondeterministicLinearEquationSolver<T>());
                    std::vector<T> result = modelchecker.checkUntil(false, phiStates, psiStates, false, nullptr);
                    
                    // Now determine the maximalReachabilityProbability.
                    for (auto state : labeledMdp.getInitialStates()) {
                        maximalReachabilityProbability = std::max(maximalReachabilityProbability, result[state]);
                    }
                } while (maximalReachabilityProbability < probabilityThreshold);
                
                std::cout << "Achieved probability: " << maximalReachabilityProbability << " with " << commandSet.size() << " commands." << std::endl;
                std::cout << "Taken commands are:" << std::endl;
                for (auto label : commandSet) {
                    std::cout << label << ", ";
                }
                std::cout << std::endl;
                
                // (7) Return the resulting command set.
                return commandSet;
                
#else
                throw storm::exceptions::NotImplementedException() << "This functionality is unavailable since StoRM has been compiled without support for Z3.";
#endif
            }
            
        };
        
    } // namespace counterexamples
} // namespace storm

#endif /* STORM_COUNTEREXAMPLES_SMTMINIMALCOMMANDSETGENERATOR_MDP_H_ */
