#ifndef STORM_MODELCHECKER_CSL_SPARSEMARKOVAUTOMATONCSLMODELCHECKER_H_
#define STORM_MODELCHECKER_CSL_SPARSEMARKOVAUTOMATONCSLMODELCHECKER_H_

#include <stack>
#include <iomanip>

#include "src/modelchecker/csl/AbstractModelChecker.h"
#include "src/models/MarkovAutomaton.h"
#include "src/storage/BitVector.h"
#include "src/storage/MaximalEndComponentDecomposition.h"
#include "src/solver/AbstractNondeterministicLinearEquationSolver.h"
#include "src/solver/LpSolver.h"
#include "src/utility/solver.h"
#include "src/utility/graph.h"
#include "src/exceptions/NotImplementedException.h"

namespace storm {
    namespace modelchecker {
        namespace csl {

            template<typename ValueType>
            class SparseMarkovAutomatonCslModelChecker : public AbstractModelChecker<ValueType> {
            public:
                
                explicit SparseMarkovAutomatonCslModelChecker(storm::models::MarkovAutomaton<ValueType> const& model, storm::solver::AbstractNondeterministicLinearEquationSolver<ValueType>* linearEquationSolver) : AbstractModelChecker<ValueType>(model), minimumOperatorStack(), linearEquationSolver(linearEquationSolver) {
                    // Intentionally left empty.
                }
                
                /*!
                 * Returns a constant reference to the MDP associated with this model checker.
                 * @returns A constant reference to the MDP associated with this model checker.
                 */
                storm::models::MarkovAutomaton<ValueType> const& getModel() const {
                    return AbstractModelChecker<ValueType>::template getModel<storm::models::MarkovAutomaton<ValueType>>();
                }
                
                std::vector<ValueType> checkUntil(storm::property::csl::Until<ValueType> const& formula, bool qualitative) const {
                    throw storm::exceptions::NotImplementedException() << "Model checking Until formulas on Markov automata is not yet implemented.";
                }
                
                std::vector<ValueType> checkTimeBoundedUntil(storm::property::csl::TimeBoundedUntil<ValueType> const& formula, bool qualitative) const {
                    throw storm::exceptions::NotImplementedException() << "Model checking Until formulas on Markov automata is not yet implemented.";
                }
                
                std::vector<ValueType> checkTimeBoundedEventually(storm::property::csl::TimeBoundedEventually<ValueType> const& formula, bool qualitative) const {
                    throw storm::exceptions::NotImplementedException() << "Model checking time-bounded Until formulas on Markov automata is not yet implemented.";
                }
                
                std::vector<ValueType> checkGlobally(storm::property::csl::Globally<ValueType> const& formula, bool qualitative) const {
                    throw storm::exceptions::NotImplementedException() << "Model checking Globally formulas on Markov automata is not yet implemented.";
                }
                
                std::vector<ValueType> checkEventually(storm::property::csl::Eventually<ValueType> const& formula, bool qualitative) const {
                    throw storm::exceptions::NotImplementedException() << "Model checking Eventually formulas on Markov automata is not yet implemented.";
                }
                
                std::vector<ValueType> checkNext(storm::property::csl::Next<ValueType> const& formula, bool qualitative) const {
                    throw storm::exceptions::NotImplementedException() << "Model checking Next formulas on Markov automata is not yet implemented.";
                }
                
                std::vector<ValueType> checkNoBoundOperator(storm::property::csl::AbstractNoBoundOperator<ValueType> const& formula) const {
                    // Check if the operator was an non-optimality operator and report an error in that case.
                    if (!formula.isOptimalityOperator()) {
                        LOG4CPLUS_ERROR(logger, "Formula does not specify neither min nor max optimality, which is not meaningful for nondeterministic models.");
                        throw storm::exceptions::InvalidArgumentException() << "Formula does not specify neither min nor max optimality, which is not meaningful for nondeterministic models.";
                    }
                    minimumOperatorStack.push(formula.isMinimumOperator());
                    std::vector<ValueType> result = formula.check(*this, false);
                    minimumOperatorStack.pop();
                    return result;
                }
                
                std::vector<ValueType> checkTimeBoundedEventually(bool min, storm::storage::BitVector const& goalStates, uint_fast64_t lowerBound, uint_fast64_t upperBound) const {
                    if (!this->getModel().isClosed()) {
                        throw storm::exceptions::InvalidArgumentException() << "Unable to compute time-bounded reachability on non-closed Markov automaton.";
                    }
                    
                    // (1) Compute the number of steps we need to take.
                    ValueType lambda = this->getModel().getMaximalExitRate();
                    ValueType delta = (2 * storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble()) / (upperBound * lambda * lambda);
                    LOG4CPLUS_INFO(logger, "Determined delta to be " << delta << ".");
                    
                    // Get some data fields for convenient access.
                    std::vector<uint_fast64_t> const& nondeterministicChoiceIndices = this->getModel().getNondeterministicChoiceIndices();
                    std::vector<ValueType> const& exitRates = this->getModel().getExitRates();
                    typename storm::storage::SparseMatrix<ValueType> const& transitionMatrix = this->getModel().getTransitionMatrix();
                    
                    // (2) Compute four sparse matrices:
                    // * a matrix A_MS with all (discretized) transitions from Markovian non-goal states to all Markovian non-goal states.
                    // * a matrix A_MStoPS with all (discretized) transitions from Markovian non-goal states to all probabilistic non-goal states.
                    // * a matrix A_PS with all (non-discretized) transitions from probabilistic non-goal states to other probabilistic non-goal states. This matrix has more rows than columns.
                    // * a matrix A_PStoMS with all (non-discretized) transitions from probabilistic non-goal states to all Markovian non-goal states. This matrix may have any shape.
                    storm::storage::BitVector const& markovianNonGoalStates = this->getModel().getMarkovianStates() & ~goalStates;
                    storm::storage::BitVector const& probabilisticNonGoalStates = ~this->getModel().getMarkovianStates() & ~goalStates;
                    
                    typename storm::storage::SparseMatrix<ValueType> aMarkovian = this->getModel().getTransitionMatrix().getSubmatrix(markovianNonGoalStates, this->getModel().getNondeterministicChoiceIndices(), true);
                    typename storm::storage::SparseMatrix<ValueType> aMarkovianToProbabilistic = this->getModel().getTransitionMatrix().getSubmatrix(markovianNonGoalStates, probabilisticNonGoalStates, nondeterministicChoiceIndices);
                    typename storm::storage::SparseMatrix<ValueType> aProbabilistic = this->getModel().getTransitionMatrix().getSubmatrix(probabilisticNonGoalStates, this->getModel().getNondeterministicChoiceIndices());
                    typename storm::storage::SparseMatrix<ValueType> aProbabilisticToMarkovian = this->getModel().getTransitionMatrix().getSubmatrix(probabilisticNonGoalStates, markovianNonGoalStates, nondeterministicChoiceIndices);
                    
                    // The matrices with transitions from Markovian states need to be digitized.
                    
                    // Digitize aMarkovian. Based on whether the transition is a self-loop or not, we apply the two digitization rules.
                    uint_fast64_t rowIndex = 0;
                    for (auto state : markovianNonGoalStates) {
                        typename storm::storage::SparseMatrix<ValueType>::MutableRows row = aMarkovian.getMutableRow(rowIndex);
                        for (auto element : row) {
                            ValueType eTerm = std::exp(-exitRates[state] * delta);
                            if (element.column() == rowIndex) {
                                element.value() = (storm::utility::constGetOne<ValueType>() - eTerm) * element.value() + eTerm;
                            } else {
                                element.value() = (storm::utility::constGetOne<ValueType>() - eTerm) * element.value();
                            }
                        }
                        ++rowIndex;
                    }
                    
                    // Digitize aMarkovianToProbabilistic. As there are no self-loops in this case, we only need to apply the digitization formula for regular successors.
                    rowIndex = 0;
                    for (auto state : markovianNonGoalStates) {
                        typename storm::storage::SparseMatrix<ValueType>::MutableRows row = aMarkovianToProbabilistic.getMutableRow(rowIndex);
                        for (auto element : row) {
                            element.value() = (1 - std::exp(-exitRates[state] * delta)) * element.value();
                        }
                        ++rowIndex;
                    }
                    
                    // (3) Initialize two vectors:
                    // * v_PS holds the probability values of probabilistic non-goal states.
                    // * v_MS holds the probability values of Markovian non-goal states.
                    std::vector<ValueType> vProbabilistic(probabilisticNonGoalStates.getNumberOfSetBits());
                    std::vector<ValueType> vMarkovian(markovianNonGoalStates.getNumberOfSetBits());
                    
                    // (4) Compute the two fixed right-hand side vectors, one for Markovian states and one for the probabilistic ones.
                    std::vector<ValueType> bProbabilistic = this->getModel().getTransitionMatrix().getConstrainedRowSumVector(probabilisticNonGoalStates, nondeterministicChoiceIndices, ~this->getModel().getMarkovianStates() & goalStates, aProbabilistic.getRowCount());
                    storm::storage::BitVector probabilisticGoalStates = ~this->getModel().getMarkovianStates() & goalStates;
                    std::vector<ValueType> bMarkovian;
                    bMarkovian.reserve(markovianNonGoalStates.getNumberOfSetBits());
                    for (auto state : markovianNonGoalStates) {
                        bMarkovian.push_back(storm::utility::constGetZero<ValueType>());
                        
                        typename storm::storage::SparseMatrix<ValueType>::Rows row = transitionMatrix.getRow(rowIndex);
                        for (auto element : row) {
                            bMarkovian.back() += (1 - std::exp(-exitRates[state] * delta)) * element.value();
                        }
                    }
                    
                    std::cout << delta << std::endl;
                    std::cout << markovianNonGoalStates.toString() << std::endl;
                    std::cout << aMarkovian.toString() << std::endl;
                    std::cout << aMarkovianToProbabilistic.toString() << std::endl;
                    std::cout << probabilisticNonGoalStates.toString() << std::endl;
                    std::cout << aProbabilistic.toString() << std::endl;
                    std::cout << aProbabilisticToMarkovian.toString() << std::endl;
                    std::cout << bProbabilistic << std::endl;
                    std::cout << bMarkovian << std::endl;
                    
                    // (3) Perform value iteration
                    // * loop until the step bound has been reached
                    // * in the loop:
                    // *    perform value iteration using A_PSwG, v_PS and the vector b where b = (A * 1_G)|PS + A_PStoMS * v_MS
                    //      and 1_G being the characteristic vector for all goal states.
                    // *    perform one timed-step using v_MS := A_MSwG * v_MS + A_MStoPS * v_PS + (A * 1_G)|MS
                    //
                    // After the loop, perform one more step of the value iteration for PS states.
                    
                    
                    
                    
                    
                    // (4) Finally, create the result vector out of 1_G and v_all.
                    
                    // Return dummy vector for the time being.
                    return std::vector<ValueType>();
                }
                
                std::vector<ValueType> checkLongRunAverage(bool min, storm::storage::BitVector const& goalStates) const {
                    if (!this->getModel().isClosed()) {
                        throw storm::exceptions::InvalidArgumentException() << "Unable to compute long-run average on non-closed Markov automaton.";
                    }

                    // Start by decomposing the Markov automaton into its MECs.
                    storm::storage::MaximalEndComponentDecomposition<double> mecDecomposition(this->getModel());
                    
                    // Get some data members for convenience.
                    typename storm::storage::SparseMatrix<ValueType> const& transitionMatrix = this->getModel().getTransitionMatrix();
                    std::vector<uint_fast64_t> const& nondeterministicChoiceIndices = this->getModel().getNondeterministicChoiceIndices();
                    
                    // Now compute the long-run average for all end components in isolation.
                    std::vector<ValueType> lraValuesForEndComponents;
                    
                    // While doing so, we already gather some information for the following steps.
                    std::vector<uint_fast64_t> stateToMecIndexMap(this->getModel().getNumberOfStates());
                    storm::storage::BitVector statesInMecs(this->getModel().getNumberOfStates());
                    
                    for (uint_fast64_t currentMecIndex = 0; currentMecIndex < mecDecomposition.size(); ++currentMecIndex) {
                        storm::storage::MaximalEndComponent const& mec = mecDecomposition[currentMecIndex];
                        
                        std::unique_ptr<storm::solver::LpSolver> solver = storm::utility::solver::getLpSolver("LRA for MEC " + std::to_string(currentMecIndex));
                        solver->setModelSense(min ? storm::solver::LpSolver::MAXIMIZE : storm::solver::LpSolver::MINIMIZE);

                        // First, we need to create the variables for the problem.
                        std::map<uint_fast64_t, uint_fast64_t> stateToVariableIndexMap;
                        for (auto const& stateChoicesPair : mec) {
                            stateToVariableIndexMap[stateChoicesPair.first] = solver->createContinuousVariable("x" + std::to_string(stateChoicesPair.first), storm::solver::LpSolver::UNBOUNDED, 0, 0, 0);
                        }
                        uint_fast64_t lraValueVariableIndex = solver->createContinuousVariable("k", storm::solver::LpSolver::UNBOUNDED, 0, 0, 1);
                        
                        // Now we encode the problem as constraints.
                        std::vector<uint_fast64_t> variables;
                        std::vector<double> coefficients;
                        for (auto const& stateChoicesPair : mec) {
                            uint_fast64_t state = stateChoicesPair.first;
                            
                            // Gather information for later use.
                            statesInMecs.set(state);
                            stateToMecIndexMap[state] = currentMecIndex;
                            
                            // Now, based on the type of the state, create a suitable constraint.
                            if (this->getModel().isMarkovianState(state)) {
                                variables.clear();
                                coefficients.clear();
                                
                                variables.push_back(stateToVariableIndexMap.at(state));
                                coefficients.push_back(1);
                                
                                typename storm::storage::SparseMatrix<ValueType>::Rows row = transitionMatrix.getRow(nondeterministicChoiceIndices[state]);
                                for (auto element : row) {
                                    variables.push_back(stateToVariableIndexMap.at(element.column()));
                                    coefficients.push_back(-element.value());
                                }
                                
                                variables.push_back(lraValueVariableIndex);
                                coefficients.push_back(storm::utility::constGetOne<ValueType>() / this->getModel().getExitRate(state));
                                
                                solver->addConstraint("state" + std::to_string(state), variables, coefficients, min ? storm::solver::LpSolver::LESS_EQUAL : storm::solver::LpSolver::GREATER_EQUAL, goalStates.get(state) ? storm::utility::constGetOne<ValueType>() / this->getModel().getExitRate(state) : storm::utility::constGetZero<ValueType>());
                            } else {
                                // For probabilistic states, we want to add the constraint x_s <= sum P(s, a, s') * x_s' where a is the current action
                                // and the sum ranges over all states s'.
                                for (auto choice : stateChoicesPair.second) {
                                    variables.clear();
                                    coefficients.clear();
                                    
                                    variables.push_back(stateToVariableIndexMap.at(state));
                                    coefficients.push_back(1);
                                    
                                    typename storm::storage::SparseMatrix<ValueType>::Rows row = transitionMatrix.getRow(choice);
                                    for (auto element : row) {
                                        variables.push_back(stateToVariableIndexMap.at(element.column()));
                                        coefficients.push_back(-element.value());
                                    }
                                    
                                    solver->addConstraint("state" + std::to_string(state), variables, coefficients, min ? storm::solver::LpSolver::LESS_EQUAL : storm::solver::LpSolver::GREATER_EQUAL, storm::utility::constGetZero<ValueType>());
                                }
                            }
                        }
                        
                        solver->optimize();
                        lraValuesForEndComponents.push_back(solver->getContinuousValue(lraValueVariableIndex));
                    }
                    
                    // For fast transition rewriting, we build some auxiliary data structures.
                    storm::storage::BitVector statesNotContainedInAnyMec = ~statesInMecs;
                    uint_fast64_t firstAuxiliaryStateIndex = statesNotContainedInAnyMec.getNumberOfSetBits();
                    uint_fast64_t lastStateNotInMecs = 0;
                    uint_fast64_t numberOfStatesNotInMecs = 0;
                    std::vector<uint_fast64_t> statesNotInMecsBeforeIndex;
                    statesNotInMecsBeforeIndex.reserve(this->getModel().getNumberOfStates());
                    for (auto state : statesNotContainedInAnyMec) {
                        while (lastStateNotInMecs <= state) {
                            statesNotInMecsBeforeIndex.push_back(numberOfStatesNotInMecs);
                            ++lastStateNotInMecs;
                        }
                        ++numberOfStatesNotInMecs;
                    }

                    // Finally, we are ready to create the SSP matrix and right-hand side of the SSP.
                    std::vector<ValueType> b;
                    std::vector<uint_fast64_t> sspNondeterministicChoiceIndices;
                    sspNondeterministicChoiceIndices.reserve(numberOfStatesNotInMecs + mecDecomposition.size() + 1);
                    typename storm::storage::SparseMatrix<ValueType> sspMatrix;
                    sspMatrix.initialize();

                    // If the source state is not contained in any MEC, we copy its choices (and perform the necessary modifications).
                    uint_fast64_t currentChoice = 0;
                    for (auto state : statesNotContainedInAnyMec) {
                        sspNondeterministicChoiceIndices.push_back(currentChoice);
                        
                        for (uint_fast64_t choice = nondeterministicChoiceIndices[state]; choice < nondeterministicChoiceIndices[state + 1]; ++choice, ++currentChoice) {
                            std::vector<ValueType> auxiliaryStateToProbabilityMap(mecDecomposition.size());
                            b.push_back(storm::utility::constGetZero<ValueType>());
                            
                            typename storm::storage::SparseMatrix<ValueType>::Rows row = transitionMatrix.getRow(choice);
                            for (auto element : row) {
                                if (statesNotContainedInAnyMec.get(element.column())) {
                                    // If the target state is not contained in an MEC, we can copy over the entry.
                                    sspMatrix.insertNextValue(currentChoice, statesNotInMecsBeforeIndex[element.column()], element.value());
                                } else {
                                    // If the target state is contained in MEC i, we need to add the probability to the corresponding field in the vector
                                    // so that we are able to write the cumulative probability to the MEC into the matrix.
                                    auxiliaryStateToProbabilityMap[stateToMecIndexMap[element.column()]] += element.value();
                                }
                            }
                            
                            // Now insert all (cumulative) probability values that target an MEC.
                            for (uint_fast64_t mecIndex = 0; mecIndex < auxiliaryStateToProbabilityMap.size(); ++mecIndex) {
                                if (auxiliaryStateToProbabilityMap[mecIndex] != 0) {
                                    sspMatrix.insertNextValue(currentChoice, firstAuxiliaryStateIndex + mecIndex, auxiliaryStateToProbabilityMap[mecIndex]);
                                }
                            }
                        }
                    }
                    
                    // Now we are ready to construct the choices for the auxiliary states.
                    for (uint_fast64_t mecIndex = 0; mecIndex < mecDecomposition.size(); ++mecIndex) {
                        storm::storage::MaximalEndComponent const& mec = mecDecomposition[mecIndex];
                        sspNondeterministicChoiceIndices.push_back(currentChoice);
                        
                        for (auto const& stateChoicesPair : mec) {
                            uint_fast64_t state = stateChoicesPair.first;
                            storm::storage::VectorSet<uint_fast64_t> const& choicesInMec = stateChoicesPair.second;
                            
                            for (uint_fast64_t choice = nondeterministicChoiceIndices[state]; choice < nondeterministicChoiceIndices[state + 1]; ++choice) {
                                std::vector<ValueType> auxiliaryStateToProbabilityMap(mecDecomposition.size());
                                
                                // If the choice is not contained in the MEC itself, we have to add a similar distribution to the auxiliary state.
                                if (!choicesInMec.contains(choice)) {
                                    b.push_back(storm::utility::constGetZero<ValueType>());

                                    typename storm::storage::SparseMatrix<ValueType>::Rows row = transitionMatrix.getRow(choice);
                                    for (auto element : row) {
                                        if (statesNotContainedInAnyMec.get(element.column())) {
                                            // If the target state is not contained in an MEC, we can copy over the entry.
                                            sspMatrix.insertNextValue(currentChoice, statesNotInMecsBeforeIndex[element.column()], element.value());
                                        } else {
                                            // If the target state is contained in MEC i, we need to add the probability to the corresponding field in the vector
                                            // so that we are able to write the cumulative probability to the MEC into the matrix.
                                            auxiliaryStateToProbabilityMap[stateToMecIndexMap[element.column()]] += element.value();
                                        }
                                    }
                                    
                                    // Now insert all (cumulative) probability values that target an MEC.
                                    for (uint_fast64_t targetMecIndex = 0; targetMecIndex < auxiliaryStateToProbabilityMap.size(); ++targetMecIndex) {
                                        if (auxiliaryStateToProbabilityMap[targetMecIndex] != 0) {
                                            // If the target MEC is the same as the current one, instead of adding a transition, we need to add the weighted reward
                                            // to the right-hand side vector of the SSP.
                                            if (mecIndex == targetMecIndex) {
                                                b.back() += auxiliaryStateToProbabilityMap[mecIndex] * lraValuesForEndComponents[mecIndex];
                                            } else {
                                                // Otherwise, we add a transition to the auxiliary state that is associated with the target MEC.
                                                sspMatrix.insertNextValue(currentChoice, firstAuxiliaryStateIndex + targetMecIndex, auxiliaryStateToProbabilityMap[targetMecIndex]);
                                            }
                                        }
                                    }
                                    
                                    ++currentChoice;
                                }
                            }
                        }
                        
                        // For each auxiliary state, there is the option to achieve the reward value of the LRA associated with the MEC.
                        sspMatrix.insertEmptyRow();
                        ++currentChoice;
                        b.push_back(lraValuesForEndComponents[mecIndex]);
                    }
                    
                    // Add the sentinel element at the end.
                    sspNondeterministicChoiceIndices.push_back(currentChoice);
                    
                    // Finalize the matrix and solve the corresponding system of equations.
                    sspMatrix.finalize();
                    std::vector<ValueType> x(numberOfStatesNotInMecs + mecDecomposition.size());
                    if (linearEquationSolver != nullptr) {
                        this->linearEquationSolver->solveEquationSystem(min, sspMatrix, x, b, sspNondeterministicChoiceIndices);
                    } else {
                        throw storm::exceptions::InvalidStateException() << "No valid linear equation solver available.";
                    }
                    
                    // Prepare result vector.
                    std::vector<ValueType> result(this->getModel().getNumberOfStates());
                    
                    // Set the values for states not contained in MECs.
                    uint_fast64_t stateIndex = 0;
                    for (auto state : statesNotContainedInAnyMec) {
                        result[state] = x[stateIndex];
                        ++stateIndex;
                    }
                    
                    // Set the values for all states in MECs.
                    for (auto state : statesInMecs) {
                        result[state] = lraValuesForEndComponents[stateToMecIndexMap[state]];
                    }

                    return result;
                }
                
                std::vector<ValueType> checkExpectedTime(bool min, storm::storage::BitVector const& goalStates) const {
                    if (!this->getModel().isClosed()) {
                        throw storm::exceptions::InvalidArgumentException() << "Unable to compute expected time on non-closed Markov automaton.";
                    }

                    // First, we need to check which states have infinite expected time (by definition).
                    storm::storage::BitVector infinityStates;
                    if (min) {
                        // If we need to compute the minimum expected times, we have to set the values of those states to infinity that, under all schedulers,
                        // reach a bottom SCC without a goal state.
                        
                        // So we start by computing all bottom SCCs without goal states.
                        storm::storage::StronglyConnectedComponentDecomposition<double> sccDecomposition(this->getModel(), ~goalStates, true, true);
                        
                        // Now form the union of all these SCCs.
                        storm::storage::BitVector unionOfNonGoalBSccs(this->getModel().getNumberOfStates());
                        for (auto const& scc : sccDecomposition) {
                            for (auto state : scc) {
                                unionOfNonGoalBSccs.set(state);
                            }
                        }
                        
                        // Finally, if this union is non-empty, compute the states such that all schedulers reach some state of the union.
                        if (!unionOfNonGoalBSccs.empty()) {
                            infinityStates = storm::utility::graph::performProbGreater0A(this->getModel(), this->getModel().getBackwardTransitions(), storm::storage::BitVector(this->getModel().getNumberOfStates(), true), unionOfNonGoalBSccs);
                        } else {
                            // Otherwise, we have no infinity states.
                            infinityStates = storm::storage::BitVector(this->getModel().getNumberOfStates());
                        }
                    } else {
                        // If we maximize the property, the expected time of a state is infinite, if an end-component without any goal state is reachable.
                        
                        // So we start by computing all MECs that have no goal state.
                        storm::storage::MaximalEndComponentDecomposition<double> mecDecomposition(this->getModel(), ~goalStates);
                        
                        // Now we form the union of all states in these end components.
                        storm::storage::BitVector unionOfNonGoalMaximalEndComponents(this->getModel().getNumberOfStates());
                        for (auto const& mec : mecDecomposition) {
                            for (auto const& stateActionPair : mec) {
                                unionOfNonGoalMaximalEndComponents.set(stateActionPair.first);
                            }
                        }
                        
                        if (!unionOfNonGoalMaximalEndComponents.empty()) {
                            // Now we need to check for which states there exists a scheduler that reaches one of the previously computed states.
                            infinityStates = storm::utility::graph::performProbGreater0E(this->getModel(), this->getModel().getBackwardTransitions(), storm::storage::BitVector(this->getModel().getNumberOfStates(), true), unionOfNonGoalMaximalEndComponents);
                        } else {
                            // Otherwise, we have no infinity states.
                            infinityStates = storm::storage::BitVector(this->getModel().getNumberOfStates());
                        }
                    }

                    // Now we identify the states for which values need to be computed.
                    storm::storage::BitVector maybeStates = ~(goalStates | infinityStates);
                    
                    // Then, we can eliminate the rows and columns for all states whose values are already known to be 0.
                    std::vector<ValueType> x(maybeStates.getNumberOfSetBits());
                    std::vector<uint_fast64_t> subNondeterministicChoiceIndices = this->computeNondeterministicChoiceIndicesForConstraint(maybeStates);
                    storm::storage::SparseMatrix<ValueType> submatrix = this->getModel().getTransitionMatrix().getSubmatrix(maybeStates, this->getModel().getNondeterministicChoiceIndices());

                    // Now prepare the mean sojourn times for all states so they can be used as the right-hand side of the equation system.
                    std::vector<ValueType> meanSojournTimes(this->getModel().getExitRates());
                    for (auto state : this->getModel().getMarkovianStates()) {
                        meanSojournTimes[state] = storm::utility::constGetOne<ValueType>() / meanSojournTimes[state];
                    }
                    
                    // Finally, prepare the actual right-hand side.
                    std::vector<ValueType> b(submatrix.getRowCount());
                    storm::utility::vector::selectVectorValuesRepeatedly(b, maybeStates, this->getModel().getNondeterministicChoiceIndices(), meanSojournTimes);
                    
                    // Solve the corresponding system of equations.
                    if (linearEquationSolver != nullptr) {
                        this->linearEquationSolver->solveEquationSystem(min, submatrix, x, b, subNondeterministicChoiceIndices);
                    } else {
                        throw storm::exceptions::InvalidStateException() << "No valid linear equation solver available.";
                    }
                    
                    // Create resulting vector.
                    std::vector<ValueType> result(this->getModel().getNumberOfStates());

                    // Set values of resulting vector according to previous result and return the result.
                    storm::utility::vector::setVectorValues<ValueType>(result, maybeStates, x);
                    storm::utility::vector::setVectorValues(result, goalStates, storm::utility::constGetZero<ValueType>());
                    storm::utility::vector::setVectorValues(result, infinityStates, storm::utility::constGetInfinity<ValueType>());

                    return result;
                }
                
            protected:
                /*!
                 * A stack used for storing whether we are currently computing min or max probabilities or rewards, respectively.
                 * The topmost element is true if and only if we are currently computing minimum probabilities or rewards.
                 */
                mutable std::stack<bool> minimumOperatorStack;
                
            private:
                /*!
                 * Computes the nondeterministic choice indices vector resulting from reducing the full system to the states given
                 * by the parameter constraint.
                 *
                 * @param constraint A bit vector specifying which states are kept.
                 * @returns A vector of the nondeterministic choice indices of the subsystem induced by the given constraint.
                 */
                std::vector<uint_fast64_t> computeNondeterministicChoiceIndicesForConstraint(storm::storage::BitVector const& constraint) const {
                    // First, get a reference to the full nondeterministic choice indices.
                    std::vector<uint_fast64_t> const& nondeterministicChoiceIndices = this->getModel().getNondeterministicChoiceIndices();
                    
                    // Reserve the known amount of slots for the resulting vector.
                    std::vector<uint_fast64_t> subNondeterministicChoiceIndices(constraint.getNumberOfSetBits() + 1);
                    uint_fast64_t currentRowCount = 0;
                    uint_fast64_t currentIndexCount = 1;
                    
                    // Set the first element as this will clearly begin at offset 0.
                    subNondeterministicChoiceIndices[0] = 0;
                    
                    // Loop over all states that need to be kept and copy the relative indices of the nondeterministic choices over
                    // to the resulting vector.
                    for (auto index : constraint) {
                        subNondeterministicChoiceIndices[currentIndexCount] = currentRowCount + nondeterministicChoiceIndices[index + 1] - nondeterministicChoiceIndices[index];
                        currentRowCount += nondeterministicChoiceIndices[index + 1] - nondeterministicChoiceIndices[index];
                        ++currentIndexCount;
                    }
                    
                    // Put a sentinel element at the end.
                    subNondeterministicChoiceIndices[constraint.getNumberOfSetBits()] = currentRowCount;
                    
                    return subNondeterministicChoiceIndices;
                }
                
                // An object that is used for solving linear equations and performing matrix-vector multiplication.
                std::unique_ptr<storm::solver::AbstractNondeterministicLinearEquationSolver<ValueType>> linearEquationSolver;
            };
        }
    }
}

#endif /* STORM_MODELCHECKER_CSL_SPARSEMARKOVAUTOMATONCSLMODELCHECKER_H_ */
