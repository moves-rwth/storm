#ifndef STORM_MODELCHECKER_CSL_SPARSEMARKOVAUTOMATONCSLMODELCHECKER_H_
#define STORM_MODELCHECKER_CSL_SPARSEMARKOVAUTOMATONCSLMODELCHECKER_H_

#include <utility>

#include "src/modelchecker/csl/AbstractModelChecker.h"
#include "src/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "src/models/MarkovAutomaton.h"
#include "src/storage/BitVector.h"
#include "src/storage/MaximalEndComponentDecomposition.h"
#include "src/solver/NondeterministicLinearEquationSolver.h"
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
                explicit SparseMarkovAutomatonCslModelChecker(storm::models::MarkovAutomaton<ValueType> const& model, std::shared_ptr<storm::solver::NondeterministicLinearEquationSolver<ValueType>> nondeterministicLinearEquationSolver) : AbstractModelChecker<ValueType>(model), nondeterministicLinearEquationSolver(nondeterministicLinearEquationSolver) {
                    // Intentionally left empty.
                }
                
				/*
					This Second constructor is NEEDED and a workaround for a common Bug in C++ with nested templates
					See: http://stackoverflow.com/questions/14401308/visual-c-cannot-deduce-given-template-arguments-for-function-used-as-defaul
				*/
				explicit SparseMarkovAutomatonCslModelChecker(storm::models::MarkovAutomaton<ValueType> const& model) : AbstractModelChecker<ValueType>(model), nondeterministicLinearEquationSolver(storm::utility::solver::getNondeterministicLinearEquationSolver<ValueType>()) {
					// Intentionally left empty.
				}

				/*!
				 * Virtual destructor. Needs to be virtual, because this class has virtual methods.
				 */
				virtual ~SparseMarkovAutomatonCslModelChecker() {
					// Intentionally left empty.
				}

                /*!
                 * Returns a constant reference to the MDP associated with this model checker.
                 * @returns A constant reference to the MDP associated with this model checker.
                 */
                storm::models::MarkovAutomaton<ValueType> const& getModel() const {
                    return AbstractModelChecker<ValueType>::template getModel<storm::models::MarkovAutomaton<ValueType>>();
                }
                
                /*!
				 * Checks the given formula that is a P operator over a path formula featuring a value bound.
				 *
				 * @param formula The formula to check.
				 * @returns The set of states satisfying the formula represented by a bit vector.
				 */
				virtual storm::storage::BitVector checkProbabilisticBoundOperator(storm::properties::csl::ProbabilisticBoundOperator<ValueType> const& formula) const override{
					// For P< and P<= the MA satisfies the formula iff the probability maximizing scheduler is used.
					// For P> and P>=               "              iff the probability minimizing         "        .
					if(formula.getComparisonOperator() == storm::properties::LESS || formula.getComparisonOperator() == storm::properties::LESS_EQUAL) {
						this->minimumOperatorStack.push(false);
					}
					else {
						this->minimumOperatorStack.push(true);
					}

					// First, we need to compute the probability for satisfying the path formula for each state.
					std::vector<ValueType> quantitativeResult = formula.getChild()->check(*this, false);

					//Remove the minimizing operator entry from the stack.
					this->minimumOperatorStack.pop();

					// Create resulting bit vector that will hold the yes/no-answer for every state.
					storm::storage::BitVector result(quantitativeResult.size());

					// Now, we can compute which states meet the bound specified in this operator and set the
					// corresponding bits to true in the resulting vector.
					for (uint_fast64_t i = 0; i < quantitativeResult.size(); ++i) {
						if (formula.meetsBound(quantitativeResult[i])) {
							result.set(i, true);
						}
					}

					return result;
				}

                std::vector<ValueType> checkUntil(storm::properties::csl::Until<ValueType> const& formula, bool qualitative) const {
                    // Test wheter it is specified if the minimum or the maximum probabilities are to be computed.
                	if(this->minimumOperatorStack.empty()) {
                		LOG4CPLUS_ERROR(logger, "Formula does not specify either min or max optimality, which is not meaningful over nondeterministic models.");
                		throw storm::exceptions::InvalidArgumentException() << "Formula does not specify either min or max optimality, which is not meaningful over nondeterministic models.";
                	}
                	storm::storage::BitVector leftStates = formula.getLeft()->check(*this);
                    storm::storage::BitVector rightStates = formula.getRight()->check(*this);
                    return computeUnboundedUntilProbabilities(this->minimumOperatorStack.top(), leftStates, rightStates, qualitative).first;
                }
                
                std::pair<std::vector<ValueType>, storm::storage::TotalScheduler> computeUnboundedUntilProbabilities(bool min, storm::storage::BitVector const& leftStates, storm::storage::BitVector const& rightStates, bool qualitative) const {
                    return storm::modelchecker::prctl::SparseMdpPrctlModelChecker<ValueType>::computeUnboundedUntilProbabilities(min, this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), this->getModel().getInitialStates(), leftStates, rightStates, nondeterministicLinearEquationSolver, qualitative);
                }
                
                std::vector<ValueType> checkTimeBoundedUntil(storm::properties::csl::TimeBoundedUntil<ValueType> const& formula, bool qualitative) const {
                    throw storm::exceptions::NotImplementedException() << "Model checking Until formulas on Markov automata is not yet implemented.";
                }
                
                std::vector<ValueType> checkTimeBoundedEventually(storm::properties::csl::TimeBoundedEventually<ValueType> const& formula, bool qualitative) const {
                	// Test wheter it is specified if the minimum or the maximum probabilities are to be computed.
					if(this->minimumOperatorStack.empty()) {
						LOG4CPLUS_ERROR(logger, "Formula does not specify either min or max optimality, which is not meaningful over nondeterministic models.");
						throw storm::exceptions::InvalidArgumentException() << "Formula does not specify either min or max optimality, which is not meaningful over nondeterministic models.";
					}
                	storm::storage::BitVector goalStates = formula.getChild()->check(*this);
                    return this->checkTimeBoundedEventually(this->minimumOperatorStack.top(), goalStates, formula.getLowerBound(), formula.getUpperBound());
                }
                
                std::vector<ValueType> checkGlobally(storm::properties::csl::Globally<ValueType> const& formula, bool qualitative) const {
                    throw storm::exceptions::NotImplementedException() << "Model checking Globally formulas on Markov automata is not yet implemented.";
                }
                
                std::vector<ValueType> checkEventually(storm::properties::csl::Eventually<ValueType> const& formula, bool qualitative) const {
                	// Test wheter it is specified if the minimum or the maximum probabilities are to be computed.
					if(this->minimumOperatorStack.empty()) {
						LOG4CPLUS_ERROR(logger, "Formula does not specify either min or max optimality, which is not meaningful over nondeterministic models.");
						throw storm::exceptions::InvalidArgumentException() << "Formula does not specify either min or max optimality, which is not meaningful over nondeterministic models.";
					}
                	storm::storage::BitVector subFormulaStates = formula.getChild()->check(*this);
                    return computeUnboundedUntilProbabilities(this->minimumOperatorStack.top(), storm::storage::BitVector(this->getModel().getNumberOfStates(), true), subFormulaStates, qualitative).first;
                }
                
                std::vector<ValueType> checkNext(storm::properties::csl::Next<ValueType> const& formula, bool qualitative) const {
                    throw storm::exceptions::NotImplementedException() << "Model checking Next formulas on Markov automata is not yet implemented.";
                }
                
                static void computeBoundedReachabilityProbabilities(bool min, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRates, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& goalStates, storm::storage::BitVector const& markovianNonGoalStates, storm::storage::BitVector const& probabilisticNonGoalStates, std::vector<ValueType>& markovianNonGoalValues, std::vector<ValueType>& probabilisticNonGoalValues, ValueType delta, uint_fast64_t numberOfSteps) {
                    // Start by computing four sparse matrices:
                    // * a matrix aMarkovian with all (discretized) transitions from Markovian non-goal states to all Markovian non-goal states.
                    // * a matrix aMarkovianToProbabilistic with all (discretized) transitions from Markovian non-goal states to all probabilistic non-goal states.
                    // * a matrix aProbabilistic with all (non-discretized) transitions from probabilistic non-goal states to other probabilistic non-goal states.
                    // * a matrix aProbabilisticToMarkovian with all (non-discretized) transitions from probabilistic non-goal states to all Markovian non-goal states.
                    typename storm::storage::SparseMatrix<ValueType> aMarkovian = transitionMatrix.getSubmatrix(true, markovianNonGoalStates, markovianNonGoalStates, true);
                    typename storm::storage::SparseMatrix<ValueType> aMarkovianToProbabilistic = transitionMatrix.getSubmatrix(true, markovianNonGoalStates, probabilisticNonGoalStates);
                    typename storm::storage::SparseMatrix<ValueType> aProbabilistic = transitionMatrix.getSubmatrix(true, probabilisticNonGoalStates, probabilisticNonGoalStates);
                    typename storm::storage::SparseMatrix<ValueType> aProbabilisticToMarkovian = transitionMatrix.getSubmatrix(true, probabilisticNonGoalStates, markovianNonGoalStates);
                    
                    // The matrices with transitions from Markovian states need to be digitized.
                    // Digitize aMarkovian. Based on whether the transition is a self-loop or not, we apply the two digitization rules.
                    uint_fast64_t rowIndex = 0;
                    for (auto state : markovianNonGoalStates) {
                        for (auto& element : aMarkovian.getRow(rowIndex)) {
                            ValueType eTerm = std::exp(-exitRates[state] * delta);
                            if (element.getColumn() == rowIndex) {
                                element.setValue((storm::utility::constantOne<ValueType>() - eTerm) * element.getValue() + eTerm);
                            } else {
                                element.setValue((storm::utility::constantOne<ValueType>() - eTerm) * element.getValue());
                            }
                        }
                        ++rowIndex;
                    }
                    
                    // Digitize aMarkovianToProbabilistic. As there are no self-loops in this case, we only need to apply the digitization formula for regular successors.
                    rowIndex = 0;
                    for (auto state : markovianNonGoalStates) {
                        for (auto& element : aMarkovianToProbabilistic.getRow(rowIndex)) {
                            element.setValue((1 - std::exp(-exitRates[state] * delta)) * element.getValue());
                        }
                        ++rowIndex;
                    }

                    // Initialize the two vectors that hold the variable one-step probabilities to all target states for probabilistic and Markovian (non-goal) states.
                    std::vector<ValueType> bProbabilistic(aProbabilistic.getRowCount());
                    std::vector<ValueType> bMarkovian(markovianNonGoalStates.getNumberOfSetBits());
                    
                    // Compute the two fixed right-hand side vectors, one for Markovian states and one for the probabilistic ones.
                    std::vector<ValueType> bProbabilisticFixed = transitionMatrix.getConstrainedRowSumVector(probabilisticNonGoalStates, goalStates);
                    std::vector<ValueType> bMarkovianFixed;
                    bMarkovianFixed.reserve(markovianNonGoalStates.getNumberOfSetBits());
                    for (auto state : markovianNonGoalStates) {
                        bMarkovianFixed.push_back(storm::utility::constantZero<ValueType>());
                        
                        for (auto& element : transitionMatrix.getRowGroup(state)) {
                            if (goalStates.get(element.getColumn())) {
                                bMarkovianFixed.back() += (1 - std::exp(-exitRates[state] * delta)) * element.getValue();
                            }
                        }
                    }
                    
                    std::shared_ptr<storm::solver::NondeterministicLinearEquationSolver<ValueType>> nondeterministiclinearEquationSolver = storm::utility::solver::getNondeterministicLinearEquationSolver<ValueType>();
                    
                    // Perform the actual value iteration
                    // * loop until the step bound has been reached
                    // * in the loop:
                    // *    perform value iteration using A_PSwG, v_PS and the vector b where b = (A * 1_G)|PS + A_PStoMS * v_MS
                    //      and 1_G being the characteristic vector for all goal states.
                    // *    perform one timed-step using v_MS := A_MSwG * v_MS + A_MStoPS * v_PS + (A * 1_G)|MS
                    std::vector<ValueType> markovianNonGoalValuesSwap(markovianNonGoalValues);
                    std::vector<ValueType> multiplicationResultScratchMemory(aProbabilistic.getRowCount());
                    std::vector<ValueType> aProbabilisticScratchMemory(probabilisticNonGoalValues.size());
                    for (uint_fast64_t currentStep = 0; currentStep < numberOfSteps; ++currentStep) {
                        // Start by (re-)computing bProbabilistic = bProbabilisticFixed + aProbabilisticToMarkovian * vMarkovian.
                        aProbabilisticToMarkovian.multiplyWithVector(markovianNonGoalValues, bProbabilistic);
                        storm::utility::vector::addVectorsInPlace(bProbabilistic, bProbabilisticFixed);
                        
                        // Now perform the inner value iteration for probabilistic states.
                        nondeterministiclinearEquationSolver->solveEquationSystem(min, aProbabilistic, probabilisticNonGoalValues, bProbabilistic, &multiplicationResultScratchMemory, &aProbabilisticScratchMemory);
                        
                        // (Re-)compute bMarkovian = bMarkovianFixed + aMarkovianToProbabilistic * vProbabilistic.
                        aMarkovianToProbabilistic.multiplyWithVector(probabilisticNonGoalValues, bMarkovian);
                        storm::utility::vector::addVectorsInPlace(bMarkovian, bMarkovianFixed);
                        aMarkovian.multiplyWithVector(markovianNonGoalValues, markovianNonGoalValuesSwap);
                        std::swap(markovianNonGoalValues, markovianNonGoalValuesSwap);
                        storm::utility::vector::addVectorsInPlace(markovianNonGoalValues, bMarkovian);
                    }
                    
                    // After the loop, perform one more step of the value iteration for PS states.
                    aProbabilisticToMarkovian.multiplyWithVector(markovianNonGoalValues, bProbabilistic);
                    storm::utility::vector::addVectorsInPlace(bProbabilistic, bProbabilisticFixed);
                    nondeterministiclinearEquationSolver->solveEquationSystem(min, aProbabilistic, probabilisticNonGoalValues, bProbabilistic, &multiplicationResultScratchMemory, &aProbabilisticScratchMemory);
                }
                
                std::vector<ValueType> checkTimeBoundedEventually(bool min, storm::storage::BitVector const& goalStates, ValueType lowerBound, ValueType upperBound) const {
                    // Check whether the automaton is closed.
                    if (!this->getModel().isClosed()) {
                        throw storm::exceptions::InvalidArgumentException() << "Unable to compute time-bounded reachability on non-closed Markov automaton.";
                    }
                    
                    // Check whether the given bounds were valid.
                    if (lowerBound < storm::utility::constantZero<ValueType>() || upperBound < storm::utility::constantZero<ValueType>() || upperBound < lowerBound) {
                        throw storm::exceptions::InvalidArgumentException() << "Illegal interval [";
                    }

                    // Get some data fields for convenient access.
                    typename storm::storage::SparseMatrix<ValueType> const& transitionMatrix = this->getModel().getTransitionMatrix();
                    std::vector<ValueType> const& exitRates = this->getModel().getExitRates();
                    storm::storage::BitVector const& markovianStates = this->getModel().getMarkovianStates();

                    // (1) Compute the accuracy we need to achieve the required error bound.
                    ValueType maxExitRate = this->getModel().getMaximalExitRate();
                    ValueType delta = (2 * storm::settings::Settings::getInstance()->getOptionByLongName("digiprecision").getArgument(0).getValueAsDouble()) / (upperBound * maxExitRate * maxExitRate);
                    
                    // (2) Compute the number of steps we need to make for the interval.
                    uint_fast64_t numberOfSteps = static_cast<uint_fast64_t>(std::ceil((upperBound - lowerBound) / delta));
                    std::cout << "Performing " << numberOfSteps << " iterations (delta=" << delta << ") for interval [" << lowerBound << ", " << upperBound << "]." << std::endl;
                    
                    // (3) Compute the non-goal states and initialize two vectors
                    // * vProbabilistic holds the probability values of probabilistic non-goal states.
                    // * vMarkovian holds the probability values of Markovian non-goal states.
                    storm::storage::BitVector const& markovianNonGoalStates = markovianStates & ~goalStates;
                    storm::storage::BitVector const& probabilisticNonGoalStates = ~markovianStates & ~goalStates;
                    std::vector<ValueType> vProbabilistic(probabilisticNonGoalStates.getNumberOfSetBits());
                    std::vector<ValueType> vMarkovian(markovianNonGoalStates.getNumberOfSetBits());
                    
                    computeBoundedReachabilityProbabilities(min, transitionMatrix, exitRates, markovianStates, goalStates, markovianNonGoalStates, probabilisticNonGoalStates, vMarkovian, vProbabilistic, delta, numberOfSteps);

                    // (4) If the lower bound of interval was non-zero, we need to take the current values as the starting values for a subsequent value iteration.
                    if (lowerBound != storm::utility::constantZero<ValueType>()) {
                        std::vector<ValueType> vAllProbabilistic((~markovianStates).getNumberOfSetBits());
                        std::vector<ValueType> vAllMarkovian(markovianStates.getNumberOfSetBits());
                        
                        // Create the starting value vectors for the next value iteration based on the results of the previous one.
                        storm::utility::vector::setVectorValues<ValueType>(vAllProbabilistic, goalStates % ~markovianStates, storm::utility::constantOne<ValueType>());
                        storm::utility::vector::setVectorValues<ValueType>(vAllProbabilistic, ~goalStates % ~markovianStates, vProbabilistic);
                        storm::utility::vector::setVectorValues<ValueType>(vAllMarkovian, goalStates % markovianStates, storm::utility::constantOne<ValueType>());
                        storm::utility::vector::setVectorValues<ValueType>(vAllMarkovian, ~goalStates % markovianStates, vMarkovian);
                        
                        // Compute the number of steps to reach the target interval.
                        numberOfSteps = static_cast<uint_fast64_t>(std::ceil(lowerBound / delta));
                        std::cout << "Performing " << numberOfSteps << " iterations (delta=" << delta << ") for interval [0, " << lowerBound << "]." << std::endl;
                        
                        // Compute the bounded reachability for interval [0, b-a].
                        computeBoundedReachabilityProbabilities(min, transitionMatrix, exitRates, markovianStates, storm::storage::BitVector(this->getModel().getNumberOfStates()), markovianStates, ~markovianStates, vAllMarkovian, vAllProbabilistic, delta, numberOfSteps);
                        
                        // Create the result vector out of vAllProbabilistic and vAllMarkovian and return it.
                        std::vector<ValueType> result(this->getModel().getNumberOfStates());
                        storm::utility::vector::setVectorValues(result, ~markovianStates, vAllProbabilistic);
                        storm::utility::vector::setVectorValues(result, markovianStates, vAllMarkovian);
                        
                        return result;
                    } else {
                        // Create the result vector out of 1_G, vProbabilistic and vMarkovian and return it.
                        std::vector<ValueType> result(this->getModel().getNumberOfStates());
                        storm::utility::vector::setVectorValues<ValueType>(result, goalStates, storm::utility::constantOne<ValueType>());
                        storm::utility::vector::setVectorValues(result, probabilisticNonGoalStates, vProbabilistic);
                        storm::utility::vector::setVectorValues(result, markovianNonGoalStates, vMarkovian);
                        return result;
                    }
                }
                
                std::vector<ValueType> checkLongRunAverage(bool min, storm::storage::BitVector const& goalStates) const {
                    // Check whether the automaton is closed.
                    if (!this->getModel().isClosed()) {
                        throw storm::exceptions::InvalidArgumentException() << "Unable to compute long-run average on non-closed Markov automaton.";
                    }
                    
                    // If there are no goal states, we avoid the computation and directly return zero.
                    if (goalStates.empty()) {
                        return std::vector<ValueType>(this->getModel().getNumberOfStates(), storm::utility::constantZero<ValueType>());
                    }
                    
                    // Likewise, if all bits are set, we can avoid the computation and set.
                    if ((~goalStates).empty()) {
                        return std::vector<ValueType>(this->getModel().getNumberOfStates(), storm::utility::constantOne<ValueType>());
                    }

                    // Start by decomposing the Markov automaton into its MECs.
                    storm::storage::MaximalEndComponentDecomposition<double> mecDecomposition(this->getModel());
                    
                    // Get some data members for convenience.
                    typename storm::storage::SparseMatrix<ValueType> const& transitionMatrix = this->getModel().getTransitionMatrix();
                    std::vector<uint_fast64_t> const& nondeterministicChoiceIndices = this->getModel().getNondeterministicChoiceIndices();
                    
                    // Now start with compute the long-run average for all end components in isolation.
                    std::vector<ValueType> lraValuesForEndComponents;
                    
                    // While doing so, we already gather some information for the following steps.
                    std::vector<uint_fast64_t> stateToMecIndexMap(this->getModel().getNumberOfStates());
                    storm::storage::BitVector statesInMecs(this->getModel().getNumberOfStates());
                    
                    for (uint_fast64_t currentMecIndex = 0; currentMecIndex < mecDecomposition.size(); ++currentMecIndex) {
                        storm::storage::MaximalEndComponent const& mec = mecDecomposition[currentMecIndex];
                        
                        // Gather information for later use.
                        for (auto const& stateChoicesPair : mec) {
                            uint_fast64_t state = stateChoicesPair.first;
                            
                            statesInMecs.set(state);
                            stateToMecIndexMap[state] = currentMecIndex;
                        }

                        // Compute the LRA value for the current MEC.
                        lraValuesForEndComponents.push_back(this->computeLraForMaximalEndComponent(min, transitionMatrix, nondeterministicChoiceIndices, this->getModel().getMarkovianStates(), this->getModel().getExitRates(), goalStates, mec, currentMecIndex));
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
                    typename storm::storage::SparseMatrixBuilder<ValueType> sspMatrixBuilder(0, 0, 0, true, numberOfStatesNotInMecs + mecDecomposition.size());
                    
                    // If the source state is not contained in any MEC, we copy its choices (and perform the necessary modifications).
                    uint_fast64_t currentChoice = 0;
                    for (auto state : statesNotContainedInAnyMec) {
                        sspMatrixBuilder.newRowGroup(currentChoice);
                        
                        for (uint_fast64_t choice = nondeterministicChoiceIndices[state]; choice < nondeterministicChoiceIndices[state + 1]; ++choice, ++currentChoice) {
                            std::vector<ValueType> auxiliaryStateToProbabilityMap(mecDecomposition.size());
                            b.push_back(storm::utility::constantZero<ValueType>());
                            
                            for (auto element : transitionMatrix.getRow(choice)) {
                                if (statesNotContainedInAnyMec.get(element.getColumn())) {
                                    // If the target state is not contained in an MEC, we can copy over the entry.
                                    sspMatrixBuilder.addNextValue(currentChoice, statesNotInMecsBeforeIndex[element.getColumn()], element.getValue());
                                } else {
                                    // If the target state is contained in MEC i, we need to add the probability to the corresponding field in the vector
                                    // so that we are able to write the cumulative probability to the MEC into the matrix.
                                    auxiliaryStateToProbabilityMap[stateToMecIndexMap[element.getColumn()]] += element.getValue();
                                }
                            }
                            
                            // Now insert all (cumulative) probability values that target an MEC.
                            for (uint_fast64_t mecIndex = 0; mecIndex < auxiliaryStateToProbabilityMap.size(); ++mecIndex) {
                                if (auxiliaryStateToProbabilityMap[mecIndex] != 0) {
                                    sspMatrixBuilder.addNextValue(currentChoice, firstAuxiliaryStateIndex + mecIndex, auxiliaryStateToProbabilityMap[mecIndex]);
                                }
                            }
                        }
                    }
                    
                    // Now we are ready to construct the choices for the auxiliary states.
                    for (uint_fast64_t mecIndex = 0; mecIndex < mecDecomposition.size(); ++mecIndex) {
                        storm::storage::MaximalEndComponent const& mec = mecDecomposition[mecIndex];
                        sspMatrixBuilder.newRowGroup(currentChoice);
                        
                        for (auto const& stateChoicesPair : mec) {
                            uint_fast64_t state = stateChoicesPair.first;
                            boost::container::flat_set<uint_fast64_t> const& choicesInMec = stateChoicesPair.second;
                            
                            for (uint_fast64_t choice = nondeterministicChoiceIndices[state]; choice < nondeterministicChoiceIndices[state + 1]; ++choice) {
                                std::vector<ValueType> auxiliaryStateToProbabilityMap(mecDecomposition.size());
                                
                                // If the choice is not contained in the MEC itself, we have to add a similar distribution to the auxiliary state.
                                if (choicesInMec.find(choice) == choicesInMec.end()) {
                                    b.push_back(storm::utility::constantZero<ValueType>());

                                    for (auto element : transitionMatrix.getRow(choice)) {
                                        if (statesNotContainedInAnyMec.get(element.getColumn())) {
                                            // If the target state is not contained in an MEC, we can copy over the entry.
                                            sspMatrixBuilder.addNextValue(currentChoice, statesNotInMecsBeforeIndex[element.getColumn()], element.getValue());
                                        } else {
                                            // If the target state is contained in MEC i, we need to add the probability to the corresponding field in the vector
                                            // so that we are able to write the cumulative probability to the MEC into the matrix.
                                            auxiliaryStateToProbabilityMap[stateToMecIndexMap[element.getColumn()]] += element.getValue();
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
                                                sspMatrixBuilder.addNextValue(currentChoice, firstAuxiliaryStateIndex + targetMecIndex, auxiliaryStateToProbabilityMap[targetMecIndex]);
                                            }
                                        }
                                    }
                                    
                                    ++currentChoice;
                                }
                            }
                        }
                        
                        // For each auxiliary state, there is the option to achieve the reward value of the LRA associated with the MEC.
                        ++currentChoice;
                        b.push_back(lraValuesForEndComponents[mecIndex]);
                    }
                    
                    // Finalize the matrix and solve the corresponding system of equations.
                    storm::storage::SparseMatrix<ValueType> sspMatrix = sspMatrixBuilder.build(currentChoice);
                    
                    std::vector<ValueType> x(numberOfStatesNotInMecs + mecDecomposition.size());
                    nondeterministicLinearEquationSolver->solveEquationSystem(min, sspMatrix, x, b);
                    
                    // Prepare result vector.
                    std::vector<ValueType> result(this->getModel().getNumberOfStates());
                    
                    // Set the values for states not contained in MECs.
                    storm::utility::vector::setVectorValues(result, statesNotContainedInAnyMec, x);
                    
                    // Set the values for all states in MECs.
                    for (auto state : statesInMecs) {
                        result[state] = lraValuesForEndComponents[stateToMecIndexMap[state]];
                    }

                    return result;
                }
                
                std::vector<ValueType> checkExpectedTime(bool min, storm::storage::BitVector const& goalStates) const {
                    // Reduce the problem of computing the expected time to computing expected rewards where the rewards
                    // for all probabilistic states are zero and the reward values of Markovian states is 1.
                    std::vector<ValueType> rewardValues(this->getModel().getNumberOfStates(), storm::utility::constantZero<ValueType>());
                    storm::utility::vector::setVectorValues(rewardValues, this->getModel().getMarkovianStates(), storm::utility::constantOne<ValueType>());
                    return this->computeExpectedRewards(min, goalStates, rewardValues);
                }
                
            protected:
                /*!
                 * Computes the long-run average value for the given maximal end component of a Markov automaton.
                 *
                 * @param min Sets whether the long-run average is to be minimized or maximized.
                 * @param transitionMatrix The transition matrix of the underlying Markov automaton.
                 * @param nondeterministicChoiceIndices A vector indicating at which row the choice of a given state begins.
                 * @param markovianStates A bit vector storing all markovian states.
                 * @param exitRates A vector with exit rates for all states. Exit rates of probabilistic states are assumed to be zero.
                 * @param goalStates A bit vector indicating which states are to be considered as goal states.
                 * @param mec The maximal end component to consider for computing the long-run average.
                 * @param mecIndex The index of the MEC.
                 * @return The long-run average of being in a goal state for the given MEC.
                 */
                static ValueType computeLraForMaximalEndComponent(bool min, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices, storm::storage::BitVector const& markovianStates, std::vector<ValueType> const& exitRates, storm::storage::BitVector const& goalStates, storm::storage::MaximalEndComponent const& mec, uint_fast64_t mecIndex = 0) {
                    std::shared_ptr<storm::solver::LpSolver> solver = storm::utility::solver::getLpSolver("LRA for MEC");
                    solver->setModelSense(min ? storm::solver::LpSolver::ModelSense::Maximize : storm::solver::LpSolver::ModelSense::Minimize);
                    
                    // First, we need to create the variables for the problem.
                    std::map<uint_fast64_t, std::string> stateToVariableNameMap;
                    for (auto const& stateChoicesPair : mec) {
                        std::string variableName = "x" + std::to_string(stateChoicesPair.first);
                        stateToVariableNameMap[stateChoicesPair.first] = variableName;
                        solver->addUnboundedContinuousVariable(variableName);
                    }
                    solver->addUnboundedContinuousVariable("k", 1);
                    solver->update();
                    
                    // Now we encode the problem as constraints.
                    for (auto const& stateChoicesPair : mec) {
                        uint_fast64_t state = stateChoicesPair.first;
                        
                        // Now, based on the type of the state, create a suitable constraint.
                        if (markovianStates.get(state)) {
                            storm::expressions::Expression constraint = storm::expressions::Expression::createDoubleVariable(stateToVariableNameMap.at(state));
                            
                            for (auto element : transitionMatrix.getRow(nondeterministicChoiceIndices[state])) {
                                constraint = constraint - storm::expressions::Expression::createDoubleVariable(stateToVariableNameMap.at(element.getColumn()));
                            }
                            
                            constraint = constraint + storm::expressions::Expression::createDoubleLiteral(storm::utility::constantOne<ValueType>() / exitRates[state]) * storm::expressions::Expression::createDoubleVariable("k");
                            storm::expressions::Expression rightHandSide = goalStates.get(state) ? storm::expressions::Expression::createDoubleLiteral(storm::utility::constantOne<ValueType>() / exitRates[state]) : storm::expressions::Expression::createDoubleLiteral(storm::utility::constantZero<ValueType>());
                            if (min) {
                                constraint = constraint <= rightHandSide;
                            } else {
                                constraint = constraint >= rightHandSide;
                            }
                            solver->addConstraint("state" + std::to_string(state), constraint);
                        } else {
                            // For probabilistic states, we want to add the constraint x_s <= sum P(s, a, s') * x_s' where a is the current action
                            // and the sum ranges over all states s'.
                            for (auto choice : stateChoicesPair.second) {
                                storm::expressions::Expression constraint = storm::expressions::Expression::createDoubleVariable(stateToVariableNameMap.at(state));

                                for (auto element : transitionMatrix.getRow(choice)) {
                                    constraint = constraint - storm::expressions::Expression::createDoubleVariable(stateToVariableNameMap.at(element.getColumn()));
                                }
                                
                                storm::expressions::Expression rightHandSide = storm::expressions::Expression::createDoubleLiteral(storm::utility::constantZero<ValueType>());
                                if (min) {
                                    constraint = constraint <= rightHandSide;
                                } else {
                                    constraint = constraint >= rightHandSide;
                                }
                                solver->addConstraint("state" + std::to_string(state), constraint);
                            }
                        }
                    }
                    
                    solver->optimize();
                    return solver->getContinuousValue("k");
                }
                
                /*!
                 * Computes the expected reward that is gained from each state before entering any of the goal states.
                 *
                 * @param min Indicates whether minimal or maximal rewards are to be computed.
                 * @param goalStates The goal states that define until which point rewards are gained.
                 * @param stateRewards A vector that defines the reward gained in each state. For probabilistic states, this is an instantaneous reward
                 * that is fully gained and for Markovian states the actually gained reward is dependent on the expected time to stay in the
                 * state, i.e. it is gouverned by the exit rate of the state.
                 * @return A vector that contains the expected reward for each state of the model.
                 */
                std::vector<ValueType> computeExpectedRewards(bool min, storm::storage::BitVector const& goalStates, std::vector<ValueType> const& stateRewards) const {
                    // Check whether the automaton is closed.
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
                            infinityStates = storm::utility::graph::performProbGreater0A(this->getModel().getTransitionMatrix(), this->getModel().getNondeterministicChoiceIndices(), this->getModel().getBackwardTransitions(), storm::storage::BitVector(this->getModel().getNumberOfStates(), true), unionOfNonGoalBSccs);
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
                            infinityStates = storm::utility::graph::performProbGreater0E(this->getModel().getTransitionMatrix(), this->getModel().getNondeterministicChoiceIndices(), this->getModel().getBackwardTransitions(), storm::storage::BitVector(this->getModel().getNumberOfStates(), true), unionOfNonGoalMaximalEndComponents);
                        } else {
                            // Otherwise, we have no infinity states.
                            infinityStates = storm::storage::BitVector(this->getModel().getNumberOfStates());
                        }
                    }
                    
                    // Now we identify the states for which values need to be computed.
                    storm::storage::BitVector maybeStates = ~(goalStates | infinityStates);
                    
                    // Then, we can eliminate the rows and columns for all states whose values are already known to be 0.
                    std::vector<ValueType> x(maybeStates.getNumberOfSetBits());
                    storm::storage::SparseMatrix<ValueType> submatrix = this->getModel().getTransitionMatrix().getSubmatrix(true, maybeStates, maybeStates);
                    
                    // Now prepare the expected reward values for all states so they can be used as the right-hand side of the equation system.
                    std::vector<ValueType> rewardValues(stateRewards);
                    for (auto state : this->getModel().getMarkovianStates()) {
                        rewardValues[state] = rewardValues[state] / this->getModel().getExitRates()[state];
                    }
                    
                    // Finally, prepare the actual right-hand side.
                    std::vector<ValueType> b(submatrix.getRowCount());
                    storm::utility::vector::selectVectorValuesRepeatedly(b, maybeStates, this->getModel().getNondeterministicChoiceIndices(), rewardValues);
                    
                    // Solve the corresponding system of equations.
                    std::shared_ptr<storm::solver::NondeterministicLinearEquationSolver<ValueType>> nondeterministiclinearEquationSolver = storm::utility::solver::getNondeterministicLinearEquationSolver<ValueType>();
                    nondeterministiclinearEquationSolver->solveEquationSystem(min, submatrix, x, b);
                    
                    // Create resulting vector.
                    std::vector<ValueType> result(this->getModel().getNumberOfStates());
                    
                    // Set values of resulting vector according to previous result and return the result.
                    storm::utility::vector::setVectorValues<ValueType>(result, maybeStates, x);
                    storm::utility::vector::setVectorValues(result, goalStates, storm::utility::constantZero<ValueType>());
                    storm::utility::vector::setVectorValues(result, infinityStates, storm::utility::constantInfinity<ValueType>());
                    
                    return result;
                }
                
                /*!
                 * A solver that is used for solving systems of linear equations that are the result of nondeterministic choices.
                 */
                std::shared_ptr<storm::solver::NondeterministicLinearEquationSolver<ValueType>> nondeterministicLinearEquationSolver;
            };
        }
    }
}

#endif /* STORM_MODELCHECKER_CSL_SPARSEMARKOVAUTOMATONCSLMODELCHECKER_H_ */
