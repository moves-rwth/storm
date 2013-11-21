#ifndef STORM_MODELCHECKER_CSL_SPARSEMARKOVAUTOMATONCSLMODELCHECKER_H_
#define STORM_MODELCHECKER_CSL_SPARSEMARKOVAUTOMATONCSLMODELCHECKER_H_

#include <stack>

#include "src/modelchecker/csl/AbstractModelChecker.h"
#include "src/models/MarkovAutomaton.h"
#include "src/storage/BitVector.h"
#include "src/solver/AbstractNondeterministicLinearEquationSolver.h"
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
                
                std::vector<ValueType> checkExpectedTime(bool min, storm::storage::BitVector const& goalStates) const {
                    // TODO: check whether the Markov automaton is closed once again? Or should that rather be done when constructing the model checker?
                    // For now we just assume that it is closed already.

                    // Start by identifying the states for which values need to be computed.
                    storm::storage::BitVector maybeStates = ~goalStates;
                    
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

                    // Set values of resulting vector according to previous result (goal states will have a value of 0) and return the result.
                    storm::utility::vector::setVectorValues<ValueType>(result, maybeStates, x);
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
