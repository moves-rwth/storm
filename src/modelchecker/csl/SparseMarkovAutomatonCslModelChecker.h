#ifndef STORM_MODELCHECKER_CSL_SPARSEMARKOVAUTOMATONCSLMODELCHECKER_H_
#define STORM_MODELCHECKER_CSL_SPARSEMARKOVAUTOMATONCSLMODELCHECKER_H_

#include <stack>

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
                
                std::vector<ValueType> checkLongRunAverage(bool min, storm::storage::BitVector const& goalStates) const {
                    // Start by decomposing the Markov automaton into its MECs.
                    storm::storage::MaximalEndComponentDecomposition<double> mecDecomposition(this->getModel());
                    
                    // Get some data members for convenience.
                    typename storm::storage::SparseMatrix<ValueType> const& transitionMatrix = this->getModel().getTransitionMatrix();
                    std::vector<uint_fast64_t> const& nondeterministicChoiceIndices = this->getModel().getNondeterministicChoiceIndices();
                    
                    // Now compute the long-run average for all end components in isolation.
                    std::vector<ValueType> lraValuesForEndComponents;
                    for (storm::storage::MaximalEndComponent const& mec : mecDecomposition) {
                        std::unique_ptr<storm::solver::LpSolver> solver = storm::utility::solver::getLpSolver("Long-Run Average");
                        solver->setModelSense(storm::solver::LpSolver::MAXIMIZE);

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
                                coefficients.push_back(this->getModel().getExitRate(state));
                                
                                solver->addConstraint("state" + std::to_string(state), variables, coefficients, storm::solver::LpSolver::LESS_EQUAL, storm::utility::constGetOne<ValueType>() / this->getModel().getExitRate(state));
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
                                    
                                    solver->addConstraint("state" + std::to_string(state), variables, coefficients, storm::solver::LpSolver::LESS_EQUAL, storm::utility::constGetZero<ValueType>());
                                }
                            }
                        }
                        
                        solver->optimize();
                        lraValuesForEndComponents.push_back(solver->getContinuousValue(lraValueVariableIndex));
                    }
                    
                    return lraValuesForEndComponents;
                }
                
                std::vector<ValueType> checkExpectedTime(bool min, storm::storage::BitVector const& goalStates) const {
                    // TODO: check whether the Markov automaton is closed once again? Or should that rather be done when constructing the model checker?
                    // For now we just assume that it is closed already.

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
