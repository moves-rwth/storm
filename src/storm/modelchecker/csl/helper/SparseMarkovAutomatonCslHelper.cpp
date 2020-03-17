#include "storm/modelchecker/csl/helper/SparseMarkovAutomatonCslHelper.h"

#include "storm/environment/Environment.h"
#include "storm/environment/solver/MinMaxSolverEnvironment.h"
#include "storm/environment/solver/TopologicalSolverEnvironment.h"
#include "storm/environment/solver/LongRunAverageSolverEnvironment.h"
#include "storm/environment/solver/EigenSolverEnvironment.h"
#include "storm/environment/solver/TimeBoundedSolverEnvironment.h"
#include "storm/exceptions/InvalidOperationException.h"
#include "storm/exceptions/UncheckedRequirementException.h"
#include "storm/modelchecker/prctl/helper/SparseMdpPrctlHelper.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/GeneralSettings.h"
#include "storm/settings/modules/MinMaxEquationSolverSettings.h"
#include "storm/solver/Multiplier.h"
#include "storm/solver/MinMaxLinearEquationSolver.h"
#include "storm/solver/LpSolver.h"
#include "storm/storage/StronglyConnectedComponentDecomposition.h"
#include "storm/storage/MaximalEndComponentDecomposition.h"
#include "storm/storage/expressions/Variable.h"
#include "storm/storage/expressions/Expression.h"
#include "storm/utility/macros.h"
#include "storm/utility/vector.h"
#include "storm/utility/graph.h"
#include "storm/utility/NumberTraits.h"
#include "storm/utility/SignalHandler.h"



namespace storm {
    namespace modelchecker {
        namespace helper {
            
            template<typename ValueType>
            std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> setUpProbabilisticStatesSolver(storm::Environment& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitions) {
                    std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> solver;
                    // The min-max system has no end components as we assume non-zeno MAs.
                    if (transitions.getNonzeroEntryCount() > 0) {
                        storm::solver::GeneralMinMaxLinearEquationSolverFactory<ValueType> factory;
                        bool isAcyclic = !storm::utility::graph::hasCycle(transitions);
                        if (isAcyclic) {
                            env.solver().minMax().setMethod(storm::solver::MinMaxMethod::Acyclic);
                        }
                        solver = factory.create(env, transitions);
                        solver->setHasUniqueSolution(true); // Assume non-zeno MA
                        solver->setHasNoEndComponents(true); // assume non-zeno MA
                        solver->setLowerBound(storm::utility::zero<ValueType>());
                        solver->setUpperBound(storm::utility::one<ValueType>());
                        solver->setCachingEnabled(true);
                        solver->setRequirementsChecked(true);
                        auto req = solver->getRequirements(env, dir);
                        req.clearBounds();
                        req.clearUniqueSolution();
                        if (isAcyclic) {
                            req.clearAcyclic();
                        }
                        STORM_LOG_THROW(!req.hasEnabledCriticalRequirement(), storm::exceptions::UncheckedRequirementException, "The solver requirement " << req.getEnabledRequirementsAsString() << " has not been checked.");
                    }
                    return solver;
            }
            
            template<typename ValueType>
            class UnifPlusHelper {
            public:
                UnifPlusHelper(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates) : transitionMatrix(transitionMatrix), exitRateVector(exitRateVector), markovianStates(markovianStates) {
                    // Intentionally left empty
                }
                
                std::vector<ValueType> computeBoundedUntilProbabilities(storm::Environment const& env, OptimizationDirection dir, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, ValueType const& upperTimeBound, boost::optional<storm::storage::BitVector> const& relevantStates = boost::none) {
                    // Since there is no lower time bound, we can treat the psiStates as if they are absorbing.
                    
                    // Compute some important subsets of states
                    storm::storage::BitVector maybeStates = ~(getProb0States(dir, phiStates, psiStates) | psiStates);
                    storm::storage::BitVector markovianMaybeStates = markovianStates & maybeStates;
                    storm::storage::BitVector probabilisticMaybeStates = ~markovianStates & maybeStates;
                    storm::storage::BitVector markovianStatesModMaybeStates = markovianMaybeStates % maybeStates;
                    storm::storage::BitVector probabilisticStatesModMaybeStates = probabilisticMaybeStates % maybeStates;
                    // Catch the case where this query can be solved by solving the untimed variant instead.
                    // This is the case if there is no Markovian maybe state (e.g. if the initial state is already a psi state) of if the time bound is infinity.
                    if (markovianMaybeStates.empty() || storm::utility::isInfinity(upperTimeBound)) {
                        return SparseMarkovAutomatonCslHelper::computeUntilProbabilities<ValueType>(env, dir, transitionMatrix, transitionMatrix.transpose(true), phiStates, psiStates, false, false).values;
                    }
                    
                    boost::optional<storm::storage::BitVector> relevantMaybeStates;
                    if (relevantStates) {
                        relevantMaybeStates = relevantStates.get() % maybeStates;
                    }
                    // Store the best solution known so far (useful in cases where the computation gets aborted)
                    std::vector<ValueType> bestKnownSolution;
                    if (relevantMaybeStates) {
                        bestKnownSolution.resize(relevantStates->size());
                    }
                    
                    // Get the exit rates restricted to only markovian maybe states.
                    std::vector<ValueType> markovianExitRates = storm::utility::vector::filterVector(exitRateVector, markovianMaybeStates);
                    
                    // Obtain parameters of the algorithm
                    auto two = storm::utility::convertNumber<ValueType>(2.0);
                    // Truncation error
                    ValueType kappa = storm::utility::convertNumber<ValueType>(env.solver().timeBounded().getUnifPlusKappa());
                    // Precision to be achieved
                    ValueType epsilon = two * storm::utility::convertNumber<ValueType>(env.solver().timeBounded().getPrecision());
                    bool relativePrecision = env.solver().timeBounded().getRelativeTerminationCriterion();
                    // Uniformization rate
                    ValueType lambda = *std::max_element(markovianExitRates.begin(), markovianExitRates.end());
                    STORM_LOG_DEBUG("Initial lambda is " << lambda << ".");

                    // Split the transitions into various part
                    // The (uniformized) probabilities to go from a Markovian state to a psi state in one step
                    std::vector<std::pair<uint64_t, ValueType>> markovianToPsiProbabilities = getSparseOneStepProbabilities(markovianMaybeStates, psiStates);
                    for (auto& entry : markovianToPsiProbabilities) {
                        entry.second *= markovianExitRates[entry.first] / lambda;
                    }
                    // Uniformized transitions from Markovian maybe states to all other maybe states. Inserts selfloop entries.
                    storm::storage::SparseMatrix<ValueType> markovianToMaybeTransitions = getUniformizedMarkovianTransitions(markovianExitRates, lambda, maybeStates, markovianMaybeStates);
                    // Transitions from probabilistic maybe states to probabilistic maybe states.
                    storm::storage::SparseMatrix<ValueType> probabilisticToProbabilisticTransitions = transitionMatrix.getSubmatrix(true, probabilisticMaybeStates, probabilisticMaybeStates, false);
                    // Transitions from probabilistic maybe states to Markovian maybe states.
                    storm::storage::SparseMatrix<ValueType> probabilisticToMarkovianTransitions = transitionMatrix.getSubmatrix(true, probabilisticMaybeStates, markovianMaybeStates, false);
                    // The probabilities to go from a probabilistic state to a psi state in one step
                    std::vector<std::pair<uint64_t, ValueType>> probabilisticToPsiProbabilities = getSparseOneStepProbabilities(probabilisticMaybeStates, psiStates);

                    // Set up a solver for the transitions between probabilistic states (if there are some)
                    Environment solverEnv = env;
                    solverEnv.solver().setForceExact(true); // Errors within the inner iterations can propagate significantly
                    auto solver = setUpProbabilisticStatesSolver(solverEnv, dir, probabilisticToProbabilisticTransitions);
                    
                    // Allocate auxiliary memory that can be used during the iterations
                    std::vector<ValueType> maybeStatesValuesLower(maybeStates.getNumberOfSetBits(), storm::utility::zero<ValueType>()); // should be zero initially
                    std::vector<ValueType> maybeStatesValuesWeightedUpper(maybeStates.getNumberOfSetBits(), storm::utility::zero<ValueType>()); // should be zero initially
                    std::vector<ValueType> maybeStatesValuesUpper(maybeStates.getNumberOfSetBits(), storm::utility::zero<ValueType>()); // should be zero initially
                    std::vector<ValueType> nextMarkovianStateValues = std::move(markovianExitRates); // At this point, the markovianExitRates are no longer needed, so we 'move' them away instead of allocating new memory
                    std::vector<ValueType> nextProbabilisticStateValues(probabilisticToProbabilisticTransitions.getRowGroupCount());
                    std::vector<ValueType> eqSysRhs(probabilisticToProbabilisticTransitions.getRowCount());
                    
                    // Start the outer iterations which increase the uniformization rate until lower and upper bound on the result vector is sufficiently small
                    storm::utility::ProgressMeasurement progressIterations("iterations");
                    uint64_t iteration = 0;
                    progressIterations.startNewMeasurement(iteration);
                    bool converged = false;
                    bool abortedInnerIterations = false;
                    while (!converged) {
                        // Maximal step size
                        uint64_t N = storm::utility::ceil(lambda * upperTimeBound * std::exp(2) - storm::utility::log(kappa * epsilon));
                        // Compute poisson distribution.
                        // The division by 8 is similar to what is done for CTMCs (probably to reduce numerical impacts?)
                        auto foxGlynnResult = storm::utility::numerical::foxGlynn(lambda * upperTimeBound, epsilon * kappa / storm::utility::convertNumber<ValueType>(8.0));
                        // Scale the weights so they sum to one.
                        //storm::utility::vector::scaleVectorInPlace(foxGlynnResult.weights, storm::utility::one<ValueType>() / foxGlynnResult.totalWeight);
                        
                        // Set up multiplier
                        auto markovianToMaybeMultiplier = storm::solver::MultiplierFactory<ValueType>().create(env, markovianToMaybeTransitions);
                        auto probabilisticToMarkovianMultiplier = storm::solver::MultiplierFactory<ValueType>().create(env, probabilisticToMarkovianTransitions);
                        
                        //Perform inner iterations first for upper, then for lower bound
                        STORM_LOG_ASSERT(!storm::utility::vector::hasNonZeroEntry(maybeStatesValuesUpper), "Current values need to be initialized with zero.");
                        for (bool computeLowerBound : {false, true}) {
                            auto& maybeStatesValues = computeLowerBound ? maybeStatesValuesLower : maybeStatesValuesWeightedUpper;
                            ValueType targetValue = computeLowerBound ? storm::utility::zero<ValueType>() : storm::utility::one<ValueType>();
                            storm::utility::ProgressMeasurement progressSteps("steps in iteration " + std::to_string(iteration) + " for " + std::string(computeLowerBound ? "lower" : "upper") + " bounds.");
                            progressSteps.setMaxCount(N);
                            progressSteps.startNewMeasurement(0);
                            bool firstIteration = true; // The first iterations can be irrelevant, because they will only produce zeroes anyway.
                            int64_t k = N;
                            // Iteration k = N is always non-relevant
                            for (--k; k >= 0; --k) {
                                
                                // Check whether the iteration is relevant, that is, whether it will contribute non-zero values to the overall result
                                if (computeLowerBound) {
                                    // Check whether the value for visiting a target state will be zero.
                                    if (static_cast<uint64_t>(k) > foxGlynnResult.right) {
                                        // Reaching this point means that we are in one of the earlier iterations where fox glynn told us to cut off
                                        continue;
                                    }
                                } else {
                                    uint64_t i = N-1-k;
                                    if (i > foxGlynnResult.right) {
                                        // Reaching this point means that we are in a later iteration which will not contribute to the upper bound
                                        // Since i will only get larger in subsequent iterations, we can directly break here.
                                        break;
                                    }
                                }
                                
                                // Compute the values at Markovian maybe states.
                                if (firstIteration) {
                                    firstIteration = false;
                                    // Reaching this point means that this is the very first relevant iteration.
                                    // If we are in the very first relevant iteration, we know that all states from the previous iteration have value zero.
                                    // It is therefore valid (and necessary) to just set the values of Markovian states to zero.
                                    std::fill(nextMarkovianStateValues.begin(), nextMarkovianStateValues.end(), storm::utility::zero<ValueType>());
                                } else {
                                    // Compute the values at Markovian maybe states.
                                    markovianToMaybeMultiplier->multiply(env, maybeStatesValues, nullptr, nextMarkovianStateValues);
                                    for (auto const& oneStepProb : markovianToPsiProbabilities) {
                                        nextMarkovianStateValues[oneStepProb.first] += oneStepProb.second * targetValue;
                                    }
                                }

                                // Update the value when reaching a psi state.
                                // This has to be done after updating the Markovian state values since we needed the 'old' target value above.
                                if (computeLowerBound && static_cast<uint64_t>(k) >= foxGlynnResult.left) {
                                    assert(static_cast<uint64_t>(k) <= foxGlynnResult.right); // has to hold since this iteration is relevant
                                    targetValue += foxGlynnResult.weights[k - foxGlynnResult.left];
                                }
                                
                                // Compute the values at probabilistic states.
                                probabilisticToMarkovianMultiplier->multiply(env, nextMarkovianStateValues, nullptr, eqSysRhs);
                                for (auto const& oneStepProb : probabilisticToPsiProbabilities) {
                                    eqSysRhs[oneStepProb.first] += oneStepProb.second * targetValue;
                                }
                                if (solver) {
                                    solver->solveEquations(solverEnv, dir, nextProbabilisticStateValues, eqSysRhs);
                                } else {
                                    storm::utility::vector::reduceVectorMinOrMax(dir, eqSysRhs, nextProbabilisticStateValues, probabilisticToProbabilisticTransitions.getRowGroupIndices());
                                }
                                
                                // Create the new values for the maybestates
                                // Fuse the results together
                                storm::utility::vector::setVectorValues(maybeStatesValues, markovianStatesModMaybeStates, nextMarkovianStateValues);
                                storm::utility::vector::setVectorValues(maybeStatesValues, probabilisticStatesModMaybeStates, nextProbabilisticStateValues);
                                if (!computeLowerBound) {
                                    // Add the scaled values to the actual result vector
                                    uint64_t i = N-1-k;
                                    if (i >= foxGlynnResult.left) {
                                        assert(i <= foxGlynnResult.right); // has to hold since this iteration is considered relevant.
                                        ValueType const& weight = foxGlynnResult.weights[i - foxGlynnResult.left];
                                        storm::utility::vector::addScaledVector(maybeStatesValuesUpper, maybeStatesValuesWeightedUpper, weight);
                                    }
                                }

                                progressSteps.updateProgress(N-k);
                                if (storm::utility::resources::isTerminate()) {
                                    abortedInnerIterations = true;
                                    break;
                                }
                            }

                            if (computeLowerBound) {
                                storm::utility::vector::scaleVectorInPlace(maybeStatesValuesLower, storm::utility::one<ValueType>() / foxGlynnResult.totalWeight);
                            } else {
                                storm::utility::vector::scaleVectorInPlace(maybeStatesValuesUpper, storm::utility::one<ValueType>() / foxGlynnResult.totalWeight);
                            }
                            
                            if (abortedInnerIterations || storm::utility::resources::isTerminate()) {
                                break;
                            }
                            
                            // Check if the lower and upper bound are sufficiently close to each other
                            converged = checkConvergence(maybeStatesValuesLower, maybeStatesValuesUpper, relevantMaybeStates, epsilon, relativePrecision, kappa);
                            if (converged) {
                                break;
                            }
                            
                            // Store the best solution we have found so far.
                            if (relevantMaybeStates) {
                                auto currentSolIt = bestKnownSolution.begin();
                                for (auto const& state : relevantMaybeStates.get()) {
                                    // We take the average of the lower and upper bounds
                                    *currentSolIt = (maybeStatesValuesLower[state] + maybeStatesValuesUpper[state]) / two;
                                    ++currentSolIt;
                                }
                            }
                        }
                        
                        if (!converged) {
                            // Increase the uniformization rate and prepare the next run
                            
                            // Double lambda.
                            ValueType oldLambda = lambda;
                            lambda *= two;
                            STORM_LOG_DEBUG("Increased lambda to " << lambda << ".");
                            
                            if (relativePrecision) {
                                // Reduce kappa a bit
                                ValueType minValue;
                                if (relevantMaybeStates) {
                                    minValue = storm::utility::vector::min_if(maybeStatesValuesUpper, relevantMaybeStates.get());
                                } else {
                                    minValue = *std::min_element(maybeStatesValuesUpper.begin(), maybeStatesValuesUpper.end());
                                }
                                minValue *= storm::utility::convertNumber<ValueType>(env.solver().timeBounded().getUnifPlusKappa());
                                kappa = std::min(kappa, minValue);
                                STORM_LOG_DEBUG("Decreased kappa to " << kappa << ".");
                            }
                            
                            // Apply uniformization with new rate
                            uniformize(markovianToMaybeTransitions, markovianToPsiProbabilities, oldLambda, lambda, markovianStatesModMaybeStates);
                            
                            // Reset the values of the maybe states to zero.
                            std::fill(maybeStatesValuesUpper.begin(), maybeStatesValuesUpper.end(), storm::utility::zero<ValueType>());
                        }
                        progressIterations.updateProgress(++iteration);
                        if (storm::utility::resources::isTerminate()) {
                            STORM_LOG_WARN("Aborted unif+ in iteration " << iteration << ".");
                            break;
                        }
                    }

                    // Prepare the result vector
                    std::vector<ValueType> result(transitionMatrix.getRowGroupCount(), storm::utility::zero<ValueType>());
                    storm::utility::vector::setVectorValues(result, psiStates, storm::utility::one<ValueType>());
                    
                    if (abortedInnerIterations && iteration > 1 && relevantMaybeStates && relevantStates) {
                        // We should take the stored solution instead of the current (probably more incorrect) lower/upper values
                        storm::utility::vector::setVectorValues(result, maybeStates & relevantStates.get(), bestKnownSolution);
                    } else {
                        // We take the average of the lower and upper bounds
                        storm::utility::vector::applyPointwise<ValueType, ValueType, ValueType>(maybeStatesValuesLower, maybeStatesValuesUpper, maybeStatesValuesLower, [&two] (ValueType const& a, ValueType const& b) -> ValueType { return (a + b) / two; });
    
                        storm::utility::vector::setVectorValues(result, maybeStates, maybeStatesValuesLower);
                    }
                    return result;
                }

            private:
                
                bool checkConvergence(std::vector<ValueType> const& lower, std::vector<ValueType> const& upper, boost::optional<storm::storage::BitVector> const& relevantValues, ValueType const& epsilon, bool relative, ValueType& kappa) {
                    STORM_LOG_ASSERT(!relevantValues.is_initialized() || relevantValues->size() == lower.size(), "Relevant values size mismatch.");
                    if (!relative) {
                        if (relevantValues) {
                            return storm::utility::vector::equalModuloPrecision(lower, upper, relevantValues.get(), epsilon * (storm::utility::one<ValueType>() - kappa), false);
                        } else {
                            return storm::utility::vector::equalModuloPrecision(lower, upper, epsilon * (storm::utility::one<ValueType>() - kappa), false);
                        }
                    }
                    ValueType truncationError = epsilon * kappa;
                    for (uint64_t i = 0; i < lower.size(); ++i) {
                        if (relevantValues) {
                            i = relevantValues->getNextSetIndex(i);
                            if (i == lower.size()) {
                                break;
                            }
                        }
                        if (lower[i] == upper[i]) {
                            continue;
                        }
                        if (lower[i] <= truncationError) {
                            return false;
                        }
                        ValueType absDiff = upper[i] - lower[i] + truncationError;
                        ValueType relDiff = absDiff / lower[i];
                        if (relDiff > epsilon) {
                            return false;
                        }
                        STORM_LOG_ASSERT(absDiff > storm::utility::zero<ValueType>(), "Upper bound " << upper[i] << " is smaller than lower bound " << lower[i] << ".");
                    }
                    return true;
                }
                
                storm::storage::SparseMatrix<ValueType> getUniformizedMarkovianTransitions(std::vector<ValueType> const& oldRates, ValueType uniformizationRate, storm::storage::BitVector const& maybeStates, storm::storage::BitVector const& markovianMaybeStates) {
                    // We need a submatrix whose rows correspond to the markovian states and columns correpsond to the maybestates.
                    // In addition, we need 'selfloop' entries for the markovian maybe states.
                    
                    // First build a submatrix without selfloop entries
                    auto submatrix = transitionMatrix.getSubmatrix(true, markovianMaybeStates, maybeStates);
                    assert(submatrix.getRowCount() == submatrix.getRowGroupCount());

                    // Now add selfloop entries at the correct positions and apply uniformization
                    storm::storage::SparseMatrixBuilder<ValueType> builder(submatrix.getRowCount(), submatrix.getColumnCount());
                    auto markovianStateColumns = markovianMaybeStates % maybeStates;
                    uint64_t row = 0;
                    for (auto const& selfloopColumn : markovianStateColumns) {
                        ValueType const& oldExitRate = oldRates[row];
                        bool foundSelfoop = false;
                        for (auto const& entry : submatrix.getRow(row)) {
                            if (entry.getColumn() == selfloopColumn) {
                                foundSelfoop = true;
                                ValueType newSelfLoop = uniformizationRate - oldExitRate + entry.getValue() * oldExitRate;
                                builder.addNextValue(row, entry.getColumn(), newSelfLoop / uniformizationRate);
                            } else {
                                builder.addNextValue(row, entry.getColumn(), entry.getValue() * oldExitRate / uniformizationRate);
                            }
                        }
                        if (!foundSelfoop) {
                            ValueType newSelfLoop = uniformizationRate - oldExitRate;
                            builder.addNextValue(row, selfloopColumn, newSelfLoop / uniformizationRate);
                        }
                        ++row;
                    }
                    assert(row == submatrix.getRowCount());
    
                    return builder.build();
                }

                void uniformize(storm::storage::SparseMatrix<ValueType>& matrix, std::vector<std::pair<uint64_t, ValueType>>& oneSteps,  std::vector<ValueType> const& oldRates, ValueType uniformizationRate, storm::storage::BitVector const& selfloopColumns) {
                    uint64_t row = 0;
                    for (auto const& selfloopColumn : selfloopColumns) {
                        ValueType const& oldExitRate = oldRates[row];
                        if (oldExitRate == uniformizationRate) {
                            // Already uniformized.
                            ++row;
                            continue;
                        }
                        for (auto& v : matrix.getRow(row)) {
                            if (v.getColumn() == selfloopColumn) {
                                ValueType newSelfLoop = uniformizationRate - oldExitRate + v.getValue() * oldExitRate;
                                v.setValue(newSelfLoop / uniformizationRate);
                            } else {
                                v.setValue(v.getValue() * oldExitRate / uniformizationRate);
                            }
                        }
                        ++row;
                    }
                    assert(row == matrix.getRowCount());
                    for (auto& oneStep : oneSteps) {
                        oneStep.second *= oldRates[oneStep.first] / uniformizationRate;
                    }
                }
                
                /// Uniformizes the given matrix assuming that it is already uniform. The selfloopColumns indicate for each row, the column indices that correspond to the 'selfloops' for that row
                void uniformize(storm::storage::SparseMatrix<ValueType>& matrix, std::vector<std::pair<uint64_t, ValueType>>& oneSteps, ValueType oldUniformizationRate, ValueType newUniformizationRate, storm::storage::BitVector const& selfloopColumns) {
                    if (oldUniformizationRate != newUniformizationRate) {
                        assert(oldUniformizationRate < newUniformizationRate);
                        ValueType rateDiff = newUniformizationRate - oldUniformizationRate;
                        ValueType rateFraction = oldUniformizationRate / newUniformizationRate;
                        uint64_t row = 0;
                        for (auto const& selfloopColumn : selfloopColumns) {
                            for (auto& v : matrix.getRow(row)) {
                                if (v.getColumn() == selfloopColumn) {
                                    ValueType newSelfLoop = rateDiff + v.getValue() * oldUniformizationRate;
                                    v.setValue(newSelfLoop / newUniformizationRate);
                                } else {
                                    v.setValue(v.getValue() * rateFraction);
                                }
                            }
                            ++row;
                        }
                        assert(row == matrix.getRowCount());
                        for (auto& oneStep : oneSteps) {
                            oneStep.second *= rateFraction;
                        }
                    }
                }
                
                storm::storage::BitVector getProb0States(OptimizationDirection dir, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates) const {
                    if (dir == storm::solver::OptimizationDirection::Maximize) {
                        return storm::utility::graph::performProb0A(transitionMatrix.transpose(true), phiStates, psiStates);
                    } else {
                        return storm::utility::graph::performProb0E(transitionMatrix, transitionMatrix.getRowGroupIndices(), transitionMatrix.transpose(true), phiStates, psiStates);
                    }
                }
                
                /*!
                 * Returns a vector with pairs of state indices and non-zero probabilities to move from the corresponding state to a target state.
                 * The state indices are with respect to the number of states satisfying the sourceStateConstraint, i.e. the indices are in the range [0, sourceStateConstraint.getNumberOfSetBits())
                 */
                std::vector<std::pair<uint64_t, ValueType>> getSparseOneStepProbabilities(storm::storage::BitVector const& sourceStateConstraint, storm::storage::BitVector const& targetStateConstraint) const {
                    auto denseResult = transitionMatrix.getConstrainedRowGroupSumVector(sourceStateConstraint, targetStateConstraint);
                    std::vector<std::pair<uint64_t, ValueType>> sparseResult;
                    for (uint64 i = 0; i < denseResult.size(); ++i) {
                        auto const& val = denseResult[i];
                        if (!storm::utility::isZero(val)) {
                            sparseResult.emplace_back(i, val);
                        }
                    }
                    return sparseResult;
                }
                
                storm::storage::SparseMatrix<ValueType> const& transitionMatrix;
                std::vector<ValueType> const& exitRateVector;
                storm::storage::BitVector const& markovianStates;
            };
            
            template <typename ValueType>
            void computeBoundedReachabilityProbabilitiesImca(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRates, storm::storage::BitVector const& goalStates, storm::storage::BitVector const& markovianNonGoalStates, storm::storage::BitVector const& probabilisticNonGoalStates, std::vector<ValueType>& markovianNonGoalValues, std::vector<ValueType>& probabilisticNonGoalValues, ValueType delta, uint64_t numberOfSteps) {
                
                // Start by computing four sparse matrices:
                // * a matrix aMarkovian with all (discretized) transitions from Markovian non-goal states to all Markovian non-goal states.
                // * a matrix aMarkovianToProbabilistic with all (discretized) transitions from Markovian non-goal states to all probabilistic non-goal states.
                // * a matrix aProbabilistic with all (non-discretized) transitions from probabilistic non-goal states to other probabilistic non-goal states.
                // * a matrix aProbabilisticToMarkovian with all (non-discretized) transitions from probabilistic non-goal states to all Markovian non-goal states.
                typename storm::storage::SparseMatrix<ValueType> aMarkovian = transitionMatrix.getSubmatrix(true, markovianNonGoalStates, markovianNonGoalStates, true);
                
                bool existProbabilisticStates = !probabilisticNonGoalStates.empty();
                typename storm::storage::SparseMatrix<ValueType> aMarkovianToProbabilistic;
                typename storm::storage::SparseMatrix<ValueType> aProbabilistic;
                typename storm::storage::SparseMatrix<ValueType> aProbabilisticToMarkovian;
                if (existProbabilisticStates) {
                    aMarkovianToProbabilistic = transitionMatrix.getSubmatrix(true, markovianNonGoalStates, probabilisticNonGoalStates);
                    aProbabilistic = transitionMatrix.getSubmatrix(true, probabilisticNonGoalStates, probabilisticNonGoalStates);
                    aProbabilisticToMarkovian = transitionMatrix.getSubmatrix(true, probabilisticNonGoalStates, markovianNonGoalStates);
                }
                
                // The matrices with transitions from Markovian states need to be digitized.
                // Digitize aMarkovian. Based on whether the transition is a self-loop or not, we apply the two digitization rules.
                uint64_t rowIndex = 0;
                for (auto state : markovianNonGoalStates) {
                    for (auto& element : aMarkovian.getRow(rowIndex)) {
                        ValueType eTerm = std::exp(-exitRates[state] * delta);
                        if (element.getColumn() == rowIndex) {
                            element.setValue((storm::utility::one<ValueType>() - eTerm) * element.getValue() + eTerm);
                        } else {
                            element.setValue((storm::utility::one<ValueType>() - eTerm) * element.getValue());
                        }
                    }
                    ++rowIndex;
                }
                
                // Digitize aMarkovianToProbabilistic. As there are no self-loops in this case, we only need to apply the digitization formula for regular successors.
                if (existProbabilisticStates) {
                    rowIndex = 0;
                    for (auto state : markovianNonGoalStates) {
                        for (auto& element : aMarkovianToProbabilistic.getRow(rowIndex)) {
                            element.setValue((1 - std::exp(-exitRates[state] * delta)) * element.getValue());
                        }
                        ++rowIndex;
                    }
                }
                
                // Initialize the two vectors that hold the variable one-step probabilities to all target states for probabilistic and Markovian (non-goal) states.
                std::vector<ValueType> bProbabilistic(existProbabilisticStates ? aProbabilistic.getRowCount() : 0);
                std::vector<ValueType> bMarkovian(markovianNonGoalStates.getNumberOfSetBits());
                
                // Compute the two fixed right-hand side vectors, one for Markovian states and one for the probabilistic ones.
                std::vector<ValueType> bProbabilisticFixed;
                if (existProbabilisticStates) {
                    bProbabilisticFixed = transitionMatrix.getConstrainedRowGroupSumVector(probabilisticNonGoalStates, goalStates);
                }
                std::vector<ValueType> bMarkovianFixed;
                bMarkovianFixed.reserve(markovianNonGoalStates.getNumberOfSetBits());
                for (auto state : markovianNonGoalStates) {
                    bMarkovianFixed.push_back(storm::utility::zero<ValueType>());
                    
                    for (auto& element : transitionMatrix.getRowGroup(state)) {
                        if (goalStates.get(element.getColumn())) {
                            bMarkovianFixed.back() += (1 - std::exp(-exitRates[state] * delta)) * element.getValue();
                        }
                    }
                }
                
                // Create a solver object (only if there are actually transitions between probabilistic states)
                auto solverEnv = env;
                solverEnv.solver().setForceExact(true);
                auto solver = setUpProbabilisticStatesSolver(solverEnv, dir, aProbabilistic);

                // Perform the actual value iteration
                // * loop until the step bound has been reached
                // * in the loop:
                // *    perform value iteration using A_PSwG, v_PS and the vector b where b = (A * 1_G)|PS + A_PStoMS * v_MS
                //      and 1_G being the characteristic vector for all goal states.
                // *    perform one timed-step using v_MS := A_MSwG * v_MS + A_MStoPS * v_PS + (A * 1_G)|MS
                std::vector<ValueType> markovianNonGoalValuesSwap(markovianNonGoalValues);
                for (uint64_t currentStep = 0; currentStep < numberOfSteps; ++currentStep) {
                    if (existProbabilisticStates) {
                        // Start by (re-)computing bProbabilistic = bProbabilisticFixed + aProbabilisticToMarkovian * vMarkovian.
                        aProbabilisticToMarkovian.multiplyWithVector(markovianNonGoalValues, bProbabilistic);
                        storm::utility::vector::addVectors(bProbabilistic, bProbabilisticFixed, bProbabilistic);
                        
                        // Now perform the inner value iteration for probabilistic states.
                        if (solver) {
                            solver->solveEquations(solverEnv, dir, probabilisticNonGoalValues, bProbabilistic);
                        } else {
                            storm::utility::vector::reduceVectorMinOrMax(dir, bProbabilistic, probabilisticNonGoalValues, aProbabilistic.getRowGroupIndices());
                        }
                        
                        // (Re-)compute bMarkovian = bMarkovianFixed + aMarkovianToProbabilistic * vProbabilistic.
                        aMarkovianToProbabilistic.multiplyWithVector(probabilisticNonGoalValues, bMarkovian);
                        storm::utility::vector::addVectors(bMarkovian, bMarkovianFixed, bMarkovian);
                    }
                    
                    aMarkovian.multiplyWithVector(markovianNonGoalValues, markovianNonGoalValuesSwap);
                    std::swap(markovianNonGoalValues, markovianNonGoalValuesSwap);
                    if (existProbabilisticStates) {
                        storm::utility::vector::addVectors(markovianNonGoalValues, bMarkovian, markovianNonGoalValues);
                    } else {
                        storm::utility::vector::addVectors(markovianNonGoalValues, bMarkovianFixed, markovianNonGoalValues);
                    }
                    if (storm::utility::resources::isTerminate()) {
                        break;
                    }
                }
                
                if (existProbabilisticStates) {
                    // After the loop, perform one more step of the value iteration for PS states.
                    aProbabilisticToMarkovian.multiplyWithVector(markovianNonGoalValues, bProbabilistic);
                    storm::utility::vector::addVectors(bProbabilistic, bProbabilisticFixed, bProbabilistic);
                    if (solver) {
                        solver->solveEquations(solverEnv, dir, probabilisticNonGoalValues, bProbabilistic);
                    } else {
                        storm::utility::vector::reduceVectorMinOrMax(dir, bProbabilistic, probabilisticNonGoalValues, aProbabilistic.getRowGroupIndices());
                    }
                }
            }
            
            template <typename ValueType>
            std::vector<ValueType> computeBoundedUntilProbabilitiesImca(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, std::pair<double, double> const& boundsPair) {
                STORM_LOG_TRACE("Using IMCA's technique to compute bounded until probabilities.");
                
                uint64_t numberOfStates = transitionMatrix.getRowGroupCount();
                
                // 'Unpack' the bounds to make them more easily accessible.
                double lowerBound = boundsPair.first;
                double upperBound = boundsPair.second;
                
                // (1) Compute the accuracy we need to achieve the required error bound.
                ValueType maxExitRate = 0;
                for (auto value : exitRateVector) {
                    maxExitRate = std::max(maxExitRate, value);
                }
                ValueType delta = (2.0 * storm::utility::convertNumber<ValueType>(env.solver().timeBounded().getPrecision())) / (upperBound * maxExitRate * maxExitRate);
                
                // (2) Compute the number of steps we need to make for the interval.
                uint64_t numberOfSteps = static_cast<uint64_t>(std::ceil((upperBound - lowerBound) / delta));
                STORM_LOG_INFO("Performing " << numberOfSteps << " iterations (delta=" << delta << ") for interval [" << lowerBound << ", " << upperBound << "]." << std::endl);
                
                // (3) Compute the non-goal states and initialize two vectors
                // * vProbabilistic holds the probability values of probabilistic non-goal states.
                // * vMarkovian holds the probability values of Markovian non-goal states.
                storm::storage::BitVector const& markovianNonGoalStates = markovianStates & ~psiStates;
                storm::storage::BitVector const& probabilisticNonGoalStates = ~markovianStates & ~psiStates;
                std::vector<ValueType> vProbabilistic(probabilisticNonGoalStates.getNumberOfSetBits());
                std::vector<ValueType> vMarkovian(markovianNonGoalStates.getNumberOfSetBits());
                
                computeBoundedReachabilityProbabilitiesImca(env, dir, transitionMatrix, exitRateVector, psiStates, markovianNonGoalStates, probabilisticNonGoalStates, vMarkovian, vProbabilistic, delta, numberOfSteps);
                
                // (4) If the lower bound of interval was non-zero, we need to take the current values as the starting values for a subsequent value iteration.
                if (lowerBound != storm::utility::zero<ValueType>()) {
                    std::vector<ValueType> vAllProbabilistic((~markovianStates).getNumberOfSetBits());
                    std::vector<ValueType> vAllMarkovian(markovianStates.getNumberOfSetBits());
                    
                    // Create the starting value vectors for the next value iteration based on the results of the previous one.
                    storm::utility::vector::setVectorValues<ValueType>(vAllProbabilistic, psiStates % ~markovianStates, storm::utility::one<ValueType>());
                    storm::utility::vector::setVectorValues<ValueType>(vAllProbabilistic, ~psiStates % ~markovianStates, vProbabilistic);
                    storm::utility::vector::setVectorValues<ValueType>(vAllMarkovian, psiStates % markovianStates, storm::utility::one<ValueType>());
                    storm::utility::vector::setVectorValues<ValueType>(vAllMarkovian, ~psiStates % markovianStates, vMarkovian);
                    
                    // Compute the number of steps to reach the target interval.
                    numberOfSteps = static_cast<uint64_t>(std::ceil(lowerBound / delta));
                    STORM_LOG_INFO("Performing " << numberOfSteps << " iterations (delta=" << delta << ") for interval [0, " << lowerBound << "]." << std::endl);
                    
                    // Compute the bounded reachability for interval [0, b-a].
                    computeBoundedReachabilityProbabilitiesImca(env, dir, transitionMatrix, exitRateVector, storm::storage::BitVector(numberOfStates), markovianStates, ~markovianStates, vAllMarkovian, vAllProbabilistic, delta, numberOfSteps);
                    
                    // Create the result vector out of vAllProbabilistic and vAllMarkovian and return it.
                    std::vector<ValueType> result(numberOfStates, storm::utility::zero<ValueType>());
                    storm::utility::vector::setVectorValues(result, ~markovianStates, vAllProbabilistic);
                    storm::utility::vector::setVectorValues(result, markovianStates, vAllMarkovian);
                    
                    return result;
                } else {
                    // Create the result vector out of 1_G, vProbabilistic and vMarkovian and return it.
                    std::vector<ValueType> result(numberOfStates);
                    storm::utility::vector::setVectorValues<ValueType>(result, psiStates, storm::utility::one<ValueType>());
                    storm::utility::vector::setVectorValues(result, probabilisticNonGoalStates, vProbabilistic);
                    storm::utility::vector::setVectorValues(result, markovianNonGoalStates, vMarkovian);
                    return result;
                }
            }
            
            template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type>
            std::vector<ValueType> SparseMarkovAutomatonCslHelper::computeBoundedUntilProbabilities(Environment const& env, storm::solver::SolveGoal<ValueType>&& goal, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, std::pair<double, double> const& boundsPair) {
                STORM_LOG_THROW(!env.solver().isForceExact(), storm::exceptions::InvalidOperationException, "Exact computations not possible for bounded until probabilities.");
                
                // Choose the applicable method
                auto method = env.solver().timeBounded().getMaMethod();
                if (method == storm::solver::MaBoundedReachabilityMethod::Imca) {
                    if (!phiStates.full()) {
                        STORM_LOG_WARN("Using Unif+ method because IMCA method does not support (phi Until psi) for non-trivial phi");
                        method = storm::solver::MaBoundedReachabilityMethod::UnifPlus;
                    }
                } else {
                    STORM_LOG_ASSERT(method == storm::solver::MaBoundedReachabilityMethod::UnifPlus, "Unknown solution method.");
                    if (!storm::utility::isZero(boundsPair.first)) {
                        STORM_LOG_WARN("Using IMCA method because Unif+ does not support a lower bound > 0.");
                        method = storm::solver::MaBoundedReachabilityMethod::Imca;
                    }
                }

                if (method == storm::solver::MaBoundedReachabilityMethod::Imca) {
                    return computeBoundedUntilProbabilitiesImca(env, goal.direction(), transitionMatrix, exitRateVector, markovianStates, psiStates, boundsPair);
                } else {
                        UnifPlusHelper<ValueType> helper(transitionMatrix, exitRateVector, markovianStates);
                        boost::optional<storm::storage::BitVector> relevantValues;
                        if (goal.hasRelevantValues()) {
                            relevantValues = std::move(goal.relevantValues());
                        }
                        return helper.computeBoundedUntilProbabilities(env, goal.direction(), phiStates, psiStates, boundsPair.second, relevantValues);
                }
            }
              
            template <typename ValueType, typename std::enable_if<!storm::NumberTraits<ValueType>::SupportsExponential, int>::type>
            std::vector<ValueType> SparseMarkovAutomatonCslHelper::computeBoundedUntilProbabilities(Environment const& env, storm::solver::SolveGoal<ValueType>&& goal, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, std::pair<double, double> const& boundsPair) {
                STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Computing bounded until probabilities is unsupported for this value type.");
            }

            template<typename ValueType>
            MDPSparseModelCheckingHelperReturnType<ValueType> SparseMarkovAutomatonCslHelper::computeUntilProbabilities(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool qualitative, bool produceScheduler) {
                return storm::modelchecker::helper::SparseMdpPrctlHelper<ValueType>::computeUntilProbabilities(env, dir, transitionMatrix, backwardTransitions, phiStates, psiStates, qualitative, produceScheduler);
            }
            
            template <typename ValueType, typename RewardModelType>
            MDPSparseModelCheckingHelperReturnType<ValueType> SparseMarkovAutomatonCslHelper::computeTotalRewards(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, RewardModelType const& rewardModel, bool produceScheduler) {
                
                // Get a reward model where the state rewards are scaled accordingly
                std::vector<ValueType> stateRewardWeights(transitionMatrix.getRowGroupCount(), storm::utility::zero<ValueType>());
                for (auto const markovianState : markovianStates) {
                    stateRewardWeights[markovianState] = storm::utility::one<ValueType>() / exitRateVector[markovianState];
                }
                std::vector<ValueType> totalRewardVector = rewardModel.getTotalActionRewardVector(transitionMatrix, stateRewardWeights);
                RewardModelType scaledRewardModel(boost::none, std::move(totalRewardVector));
                
                return SparseMdpPrctlHelper<ValueType>::computeTotalRewards(env, dir, transitionMatrix, backwardTransitions, scaledRewardModel, false, produceScheduler);
            }
            
            template <typename ValueType, typename RewardModelType>
            MDPSparseModelCheckingHelperReturnType<ValueType> SparseMarkovAutomatonCslHelper::computeReachabilityRewards(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, RewardModelType const& rewardModel, storm::storage::BitVector const& psiStates, bool produceScheduler) {
                
                // Get a reward model where the state rewards are scaled accordingly
                std::vector<ValueType> stateRewardWeights(transitionMatrix.getRowGroupCount(), storm::utility::zero<ValueType>());
                for (auto const markovianState : markovianStates) {
                    stateRewardWeights[markovianState] = storm::utility::one<ValueType>() / exitRateVector[markovianState];
                }
                std::vector<ValueType> totalRewardVector = rewardModel.getTotalActionRewardVector(transitionMatrix, stateRewardWeights);
                RewardModelType scaledRewardModel(boost::none, std::move(totalRewardVector));
                
                return SparseMdpPrctlHelper<ValueType>::computeReachabilityRewards(env, dir, transitionMatrix, backwardTransitions, scaledRewardModel, psiStates, false, produceScheduler);
            }
            
            template<typename ValueType>
            std::vector<ValueType> SparseMarkovAutomatonCslHelper::computeLongRunAverageProbabilities(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates) {
            
                uint64_t numberOfStates = transitionMatrix.getRowGroupCount();

                // If there are no goal states, we avoid the computation and directly return zero.
                if (psiStates.empty()) {
                    return std::vector<ValueType>(numberOfStates, storm::utility::zero<ValueType>());
                }
                
                // Likewise, if all bits are set, we can avoid the computation and set.
                if (psiStates.full()) {
                    return std::vector<ValueType>(numberOfStates, storm::utility::one<ValueType>());
                }
                
                // Otherwise, reduce the long run average probabilities to long run average rewards.
                // Every Markovian goal state gets reward one.
                std::vector<ValueType> stateRewards(transitionMatrix.getRowGroupCount(), storm::utility::zero<ValueType>());
                storm::utility::vector::setVectorValues(stateRewards, markovianStates & psiStates, storm::utility::one<ValueType>());
                storm::models::sparse::StandardRewardModel<ValueType> rewardModel(std::move(stateRewards));
                
                return computeLongRunAverageRewards(env, dir, transitionMatrix, backwardTransitions, exitRateVector, markovianStates, rewardModel);
                
            }
            
            template<typename ValueType, typename RewardModelType>
            std::vector<ValueType> SparseMarkovAutomatonCslHelper::computeLongRunAverageRewards(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, RewardModelType const& rewardModel) {
                
                uint64_t numberOfStates = transitionMatrix.getRowGroupCount();

                // Start by decomposing the Markov automaton into its MECs.
                storm::storage::MaximalEndComponentDecomposition<ValueType> mecDecomposition(transitionMatrix, backwardTransitions);
                
                // Get some data members for convenience.
                std::vector<uint64_t> const& nondeterministicChoiceIndices = transitionMatrix.getRowGroupIndices();
                
                // Now start with compute the long-run average for all end components in isolation.
                std::vector<ValueType> lraValuesForEndComponents;
                
                // While doing so, we already gather some information for the following steps.
                std::vector<uint64_t> stateToMecIndexMap(numberOfStates);
                storm::storage::BitVector statesInMecs(numberOfStates);
                
                auto underlyingSolverEnvironment = env;
                if (env.solver().isForceSoundness()) {
                    // For sound computations, the error in the MECS plus the error in the remaining system should be less then the user defined precsion.
                    underlyingSolverEnvironment.solver().minMax().setPrecision(env.solver().lra().getPrecision() / storm::utility::convertNumber<storm::RationalNumber>(2));
                    underlyingSolverEnvironment.solver().minMax().setRelativeTerminationCriterion(env.solver().lra().getRelativeTerminationCriterion());
                    underlyingSolverEnvironment.solver().lra().setPrecision(env.solver().lra().getPrecision() / storm::utility::convertNumber<storm::RationalNumber>(2));
                }
                
                for (uint64_t currentMecIndex = 0; currentMecIndex < mecDecomposition.size(); ++currentMecIndex) {
                    storm::storage::MaximalEndComponent const& mec = mecDecomposition[currentMecIndex];
                    
                    // Gather information for later use.
                    for (auto const& stateChoicesPair : mec) {
                        uint64_t state = stateChoicesPair.first;
                        
                        statesInMecs.set(state);
                        stateToMecIndexMap[state] = currentMecIndex;
                    }
                    
                    // Compute the LRA value for the current MEC.
                    lraValuesForEndComponents.push_back(computeLraForMaximalEndComponent(underlyingSolverEnvironment, dir, transitionMatrix, exitRateVector, markovianStates, rewardModel, mec));
                }
                
                // For fast transition rewriting, we build some auxiliary data structures.
                storm::storage::BitVector statesNotContainedInAnyMec = ~statesInMecs;
                uint64_t firstAuxiliaryStateIndex = statesNotContainedInAnyMec.getNumberOfSetBits();
                uint64_t lastStateNotInMecs = 0;
                uint64_t numberOfStatesNotInMecs = 0;
                std::vector<uint64_t> statesNotInMecsBeforeIndex;
                statesNotInMecsBeforeIndex.reserve(numberOfStates);
                for (auto state : statesNotContainedInAnyMec) {
                    while (lastStateNotInMecs <= state) {
                        statesNotInMecsBeforeIndex.push_back(numberOfStatesNotInMecs);
                        ++lastStateNotInMecs;
                    }
                    ++numberOfStatesNotInMecs;
                }
                uint64_t numberOfSspStates = numberOfStatesNotInMecs + mecDecomposition.size();
                
                // Finally, we are ready to create the SSP matrix and right-hand side of the SSP.
                std::vector<ValueType> b;
                typename storm::storage::SparseMatrixBuilder<ValueType> sspMatrixBuilder(0, numberOfSspStates , 0, false, true, numberOfSspStates);
                
                // If the source state is not contained in any MEC, we copy its choices (and perform the necessary modifications).
                uint64_t currentChoice = 0;
                for (auto state : statesNotContainedInAnyMec) {
                    sspMatrixBuilder.newRowGroup(currentChoice);
                    
                    for (uint64_t choice = nondeterministicChoiceIndices[state]; choice < nondeterministicChoiceIndices[state + 1]; ++choice, ++currentChoice) {
                        std::vector<ValueType> auxiliaryStateToProbabilityMap(mecDecomposition.size());
                        b.push_back(storm::utility::zero<ValueType>());
                        
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
                        for (uint64_t mecIndex = 0; mecIndex < auxiliaryStateToProbabilityMap.size(); ++mecIndex) {
                            if (auxiliaryStateToProbabilityMap[mecIndex] != 0) {
                                sspMatrixBuilder.addNextValue(currentChoice, firstAuxiliaryStateIndex + mecIndex, auxiliaryStateToProbabilityMap[mecIndex]);
                            }
                        }
                    }
                }
                
                // Now we are ready to construct the choices for the auxiliary states.
                for (uint64_t mecIndex = 0; mecIndex < mecDecomposition.size(); ++mecIndex) {
                    storm::storage::MaximalEndComponent const& mec = mecDecomposition[mecIndex];
                    sspMatrixBuilder.newRowGroup(currentChoice);
                    
                    for (auto const& stateChoicesPair : mec) {
                        uint64_t state = stateChoicesPair.first;
                        storm::storage::FlatSet<uint64_t> const& choicesInMec = stateChoicesPair.second;
                        
                        for (uint64_t choice = nondeterministicChoiceIndices[state]; choice < nondeterministicChoiceIndices[state + 1]; ++choice) {
                            
                            // If the choice is not contained in the MEC itself, we have to add a similar distribution to the auxiliary state.
                            if (choicesInMec.find(choice) == choicesInMec.end()) {
                                std::vector<ValueType> auxiliaryStateToProbabilityMap(mecDecomposition.size());
                                b.push_back(storm::utility::zero<ValueType>());
                                
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
                                for (uint64_t targetMecIndex = 0; targetMecIndex < auxiliaryStateToProbabilityMap.size(); ++targetMecIndex) {
                                    if (auxiliaryStateToProbabilityMap[targetMecIndex] != 0) {
                                        sspMatrixBuilder.addNextValue(currentChoice, firstAuxiliaryStateIndex + targetMecIndex, auxiliaryStateToProbabilityMap[targetMecIndex]);
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
                storm::storage::SparseMatrix<ValueType> sspMatrix = sspMatrixBuilder.build(currentChoice, numberOfSspStates, numberOfSspStates);
                
                std::vector<ValueType> x(numberOfSspStates);
                
                // Check for requirements of the solver.
                storm::solver::GeneralMinMaxLinearEquationSolverFactory<ValueType> minMaxLinearEquationSolverFactory;
                storm::solver::MinMaxLinearEquationSolverRequirements requirements = minMaxLinearEquationSolverFactory.getRequirements(underlyingSolverEnvironment, true, true, dir);
                requirements.clearBounds();
                STORM_LOG_THROW(!requirements.hasEnabledCriticalRequirement(), storm::exceptions::UncheckedRequirementException, "Solver requirements " + requirements.getEnabledRequirementsAsString() + " not checked.");

                std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> solver = minMaxLinearEquationSolverFactory.create(underlyingSolverEnvironment, sspMatrix);
                solver->setHasUniqueSolution();
                solver->setHasNoEndComponents();
                solver->setLowerBound(storm::utility::zero<ValueType>());
                solver->setUpperBound(*std::max_element(lraValuesForEndComponents.begin(), lraValuesForEndComponents.end()));
                solver->setRequirementsChecked();
                solver->solveEquations(underlyingSolverEnvironment, dir, x, b);
                
                // Prepare result vector.
                std::vector<ValueType> result(numberOfStates);
                
                // Set the values for states not contained in MECs.
                storm::utility::vector::setVectorValues(result, statesNotContainedInAnyMec, x);
                
                // Set the values for all states in MECs.
                for (auto state : statesInMecs) {
                    result[state] = x[firstAuxiliaryStateIndex + stateToMecIndexMap[state]];
                }
                
                return result;
            }
            
            template <typename ValueType>
            MDPSparseModelCheckingHelperReturnType<ValueType> SparseMarkovAutomatonCslHelper::computeReachabilityTimes(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, bool produceScheduler) {
                
                // Get a reward model representing expected sojourn times
                std::vector<ValueType> rewardValues(transitionMatrix.getRowCount(), storm::utility::zero<ValueType>());
                for (auto const markovianState : markovianStates) {
                    rewardValues[transitionMatrix.getRowGroupIndices()[markovianState]] = storm::utility::one<ValueType>() / exitRateVector[markovianState];
                }
                storm::models::sparse::StandardRewardModel<ValueType> rewardModel(boost::none, std::move(rewardValues));
                
                return SparseMdpPrctlHelper<ValueType>::computeReachabilityRewards(env, dir, transitionMatrix, backwardTransitions, rewardModel, psiStates, false, produceScheduler);
            }

            template<typename ValueType, typename RewardModelType>
            ValueType SparseMarkovAutomatonCslHelper::computeLraForMaximalEndComponent(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, RewardModelType const& rewardModel, storm::storage::MaximalEndComponent const& mec) {
                
                // If the mec only consists of a single state, we compute the LRA value directly
                if (++mec.begin() == mec.end()) {
                    uint64_t state = mec.begin()->first;
                    STORM_LOG_THROW(markovianStates.get(state), storm::exceptions::InvalidOperationException, "Markov Automaton has Zeno behavior. Computation of Long Run Average values not supported.");
                    ValueType result = rewardModel.hasStateRewards() ? rewardModel.getStateReward(state) : storm::utility::zero<ValueType>();
                    if (rewardModel.hasStateActionRewards() || rewardModel.hasTransitionRewards()) {
                        STORM_LOG_ASSERT(mec.begin()->second.size() == 1, "Markovian state has nondeterministic behavior.");
                        uint64_t choice = *mec.begin()->second.begin();
                        result += exitRateVector[state] * rewardModel.getTotalStateActionReward(state, choice, transitionMatrix, storm::utility::zero<ValueType>());
                    }
                    return result;
                }
                
                // Solve MEC with the method specified in the settings
                storm::solver::LraMethod method = env.solver().lra().getNondetLraMethod();
                if ((storm::NumberTraits<ValueType>::IsExact || env.solver().isForceExact()) && env.solver().lra().isNondetLraMethodSetFromDefault() && method != storm::solver::LraMethod::LinearProgramming) {
                    STORM_LOG_INFO("Selecting 'LP' as the solution technique for long-run properties to guarantee exact results. If you want to override this, please explicitly specify a different LRA method.");
                    method = storm::solver::LraMethod::LinearProgramming;
                } else if (env.solver().isForceSoundness() && env.solver().lra().isNondetLraMethodSetFromDefault() && method != storm::solver::LraMethod::ValueIteration) {
                    STORM_LOG_INFO("Selecting 'VI' as the solution technique for long-run properties to guarantee sound results. If you want to override this, please explicitly specify a different LRA method.");
                    method = storm::solver::LraMethod::ValueIteration;
                }
                if (method == storm::solver::LraMethod::LinearProgramming) {
                    return computeLraForMaximalEndComponentLP(env, dir, transitionMatrix, exitRateVector, markovianStates, rewardModel, mec);
                } else if (method == storm::solver::LraMethod::ValueIteration) {
                    return computeLraForMaximalEndComponentVI(env, dir, transitionMatrix, exitRateVector, markovianStates, rewardModel, mec);
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "Unsupported technique.");
                }
            }
            
            template<typename ValueType, typename RewardModelType>
            ValueType SparseMarkovAutomatonCslHelper::computeLraForMaximalEndComponentLP(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, RewardModelType const& rewardModel, storm::storage::MaximalEndComponent const& mec) {
                std::unique_ptr<storm::utility::solver::LpSolverFactory<ValueType>> lpSolverFactory(new storm::utility::solver::LpSolverFactory<ValueType>());
                std::unique_ptr<storm::solver::LpSolver<ValueType>> solver = lpSolverFactory->create("LRA for MEC");
                solver->setOptimizationDirection(invert(dir));
                
                // First, we need to create the variables for the problem.
                std::map<uint64_t, storm::expressions::Variable> stateToVariableMap;
                for (auto const& stateChoicesPair : mec) {
                    std::string variableName = "x" + std::to_string(stateChoicesPair.first);
                    stateToVariableMap[stateChoicesPair.first] = solver->addUnboundedContinuousVariable(variableName);
                }
                storm::expressions::Variable k = solver->addUnboundedContinuousVariable("k", storm::utility::one<ValueType>());
                solver->update();
                
                // Now we encode the problem as constraints.
                std::vector<uint64_t> const& nondeterministicChoiceIndices = transitionMatrix.getRowGroupIndices();
                for (auto const& stateChoicesPair : mec) {
                    uint64_t state = stateChoicesPair.first;
                    
                    // Now, based on the type of the state, create a suitable constraint.
                    if (markovianStates.get(state)) {
                        STORM_LOG_ASSERT(stateChoicesPair.second.size() == 1, "Markovian state " << state << " is not deterministic: It has " << stateChoicesPair.second.size() << " choices.");
                        uint64_t choice = *stateChoicesPair.second.begin();
                        
                        storm::expressions::Expression constraint = stateToVariableMap.at(state);
                        
                        for (auto element : transitionMatrix.getRow(nondeterministicChoiceIndices[state])) {
                            constraint = constraint - stateToVariableMap.at(element.getColumn()) * solver->getManager().rational((element.getValue()));
                        }
                        
                        constraint = constraint + solver->getManager().rational(storm::utility::one<ValueType>() / exitRateVector[state]) * k;
                        
                        storm::expressions::Expression rightHandSide = solver->getManager().rational(rewardModel.getTotalStateActionReward(state, choice, transitionMatrix, (ValueType) (storm::utility::one<ValueType>() / exitRateVector[state])));
                        if (dir == OptimizationDirection::Minimize) {
                            constraint = constraint <= rightHandSide;
                        } else {
                            constraint = constraint >= rightHandSide;
                        }
                        solver->addConstraint("state" + std::to_string(state), constraint);
                    } else {
                        // For probabilistic states, we want to add the constraint x_s <= sum P(s, a, s') * x_s' where a is the current action
                        // and the sum ranges over all states s'.
                        for (auto choice : stateChoicesPair.second) {
                            storm::expressions::Expression constraint = stateToVariableMap.at(state);
                            
                            for (auto element : transitionMatrix.getRow(choice)) {
                                constraint = constraint - stateToVariableMap.at(element.getColumn()) * solver->getManager().rational(element.getValue());
                            }

                            storm::expressions::Expression rightHandSide = solver->getManager().rational(rewardModel.getTotalStateActionReward(state, choice, transitionMatrix, storm::utility::zero<ValueType>()));
                            if (dir == OptimizationDirection::Minimize) {
                                constraint = constraint <= rightHandSide;
                            } else {
                                constraint = constraint >= rightHandSide;
                            }
                            solver->addConstraint("state" + std::to_string(state), constraint);
                        }
                    }
                }
                
                solver->optimize();
                return solver->getContinuousValue(k);
            }
            
            template<typename ValueType, typename RewardModelType>
            ValueType SparseMarkovAutomatonCslHelper::computeLraForMaximalEndComponentVI(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, RewardModelType const& rewardModel, storm::storage::MaximalEndComponent const& mec) {
                
                // Initialize data about the mec
                
                storm::storage::BitVector mecStates(transitionMatrix.getRowGroupCount(), false);
                storm::storage::BitVector mecChoices(transitionMatrix.getRowCount(), false);
                for (auto const& stateChoicesPair : mec) {
                    mecStates.set(stateChoicesPair.first);
                    for (auto const& choice : stateChoicesPair.second) {
                        mecChoices.set(choice);
                    }
                }
                storm::storage::BitVector markovianMecStates = mecStates & markovianStates;
                storm::storage::BitVector probabilisticMecStates = mecStates & ~markovianStates;
                storm::storage::BitVector probabilisticMecChoices = transitionMatrix.getRowFilter(probabilisticMecStates) & mecChoices;
                STORM_LOG_THROW(!markovianMecStates.empty(), storm::exceptions::InvalidOperationException, "Markov Automaton has Zeno behavior. Computation of Long Run Average values not supported.");
                bool hasProbabilisticStates = !probabilisticMecStates.empty();
                // Get the uniformization rate
                
                ValueType uniformizationRate = storm::utility::vector::max_if(exitRateVector, markovianMecStates);
                // To ensure that the model is aperiodic, we need to make sure that every Markovian state gets a self loop.
                // Hence, we increase the uniformization rate a little.
                uniformizationRate *= (storm::utility::one<ValueType>() + storm::utility::convertNumber<ValueType>(env.solver().lra().getAperiodicFactor()));

                // Get the transitions of the submodel, that is
                // * a matrix aMarkovian with all (uniformized) transitions from Markovian mec states to all Markovian mec states.
                // * a matrix aMarkovianToProbabilistic with all (uniformized) transitions from Markovian mec states to all probabilistic mec states.
                // * a matrix aProbabilistic with all transitions from probabilistic mec states to other probabilistic mec states.
                // * a matrix aProbabilisticToMarkovian with all  transitions from probabilistic mec states to all Markovian mec states.
                typename storm::storage::SparseMatrix<ValueType> aMarkovian = transitionMatrix.getSubmatrix(true, markovianMecStates, markovianMecStates, true);
                typename storm::storage::SparseMatrix<ValueType> aMarkovianToProbabilistic, aProbabilistic, aProbabilisticToMarkovian;
                if (hasProbabilisticStates) {
                    aMarkovianToProbabilistic = transitionMatrix.getSubmatrix(true, markovianMecStates, probabilisticMecStates);
                    aProbabilistic = transitionMatrix.getSubmatrix(false, probabilisticMecChoices, probabilisticMecStates);
                    aProbabilisticToMarkovian = transitionMatrix.getSubmatrix(false, probabilisticMecChoices, markovianMecStates);
                }
                
                // The matrices with transitions from Markovian states need to be uniformized.
                uint64_t subState = 0;
                for (auto state : markovianMecStates) {
                    ValueType uniformizationFactor = exitRateVector[state] / uniformizationRate;
                    if (hasProbabilisticStates) {
                        for (auto& entry : aMarkovianToProbabilistic.getRow(subState)) {
                            entry.setValue(entry.getValue() * uniformizationFactor);
                        }
                    }
                    for (auto& entry : aMarkovian.getRow(subState)) {
                        if (entry.getColumn() == subState) {
                            entry.setValue(storm::utility::one<ValueType>() - uniformizationFactor * (storm::utility::one<ValueType>() - entry.getValue()));
                        } else {
                            entry.setValue(entry.getValue() * uniformizationFactor);
                        }
                    }
                    ++subState;
                }

                // Compute the rewards obtained in a single uniformization step
                
                std::vector<ValueType> markovianChoiceRewards;
                markovianChoiceRewards.reserve(aMarkovian.getRowCount());
                for (auto const& state : markovianMecStates) {
                    ValueType stateRewardScalingFactor = storm::utility::one<ValueType>() / uniformizationRate;
                    ValueType actionRewardScalingFactor = exitRateVector[state] / uniformizationRate;
                    assert(transitionMatrix.getRowGroupSize(state) == 1);
                    uint64_t choice = transitionMatrix.getRowGroupIndices()[state];
                    markovianChoiceRewards.push_back(rewardModel.getTotalStateActionReward(state, choice, transitionMatrix, stateRewardScalingFactor, actionRewardScalingFactor));
                }
                
                std::vector<ValueType> probabilisticChoiceRewards;
                if (hasProbabilisticStates) {
                    probabilisticChoiceRewards.reserve(aProbabilistic.getRowCount());
                    for (auto const& state : probabilisticMecStates) {
                        uint64_t groupStart = transitionMatrix.getRowGroupIndices()[state];
                        uint64_t groupEnd = transitionMatrix.getRowGroupIndices()[state + 1];
                        for (uint64_t choice = probabilisticMecChoices.getNextSetIndex(groupStart); choice < groupEnd; choice = probabilisticMecChoices.getNextSetIndex(choice + 1)) {
                            probabilisticChoiceRewards.push_back(rewardModel.getTotalStateActionReward(state, choice, transitionMatrix, storm::utility::zero<ValueType>()));
                        }
                    }
                }
                
                // start the iterations
                
                ValueType precision = storm::utility::convertNumber<ValueType>(env.solver().lra().getPrecision()) / uniformizationRate;
                bool relative = env.solver().lra().getRelativeTerminationCriterion();
                std::vector<ValueType> v(aMarkovian.getRowCount(), storm::utility::zero<ValueType>());
                std::vector<ValueType> w = v;
                std::vector<ValueType> x, b;
                auto solverEnv = env;
                std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> solver;
                if (hasProbabilisticStates) {
                    if (env.solver().isForceSoundness()) {
                        // To get correct results, the inner equation systems are solved exactly.
                        // TODO investigate how an error would propagate
                        solverEnv.solver().setForceExact(true);
                    }
                    
                    x.resize(aProbabilistic.getRowGroupCount(), storm::utility::zero<ValueType>());
                    b = probabilisticChoiceRewards;
                    
                    solver = setUpProbabilisticStatesSolver(solverEnv, dir, aProbabilistic);
                }
                
                uint64_t iter = 0;
                boost::optional<uint64_t> maxIter;
                if (env.solver().lra().isMaximalIterationCountSet()) {
                    maxIter = env.solver().lra().getMaximalIterationCount();
                }
                while (!maxIter.is_initialized() || iter < maxIter.get()) {
                    ++iter;
                    // Compute the expected total rewards for the probabilistic states
                    if (hasProbabilisticStates) {
                        if (solver) {
                            solver->solveEquations(solverEnv, dir, x, b);
                        } else {
                            storm::utility::vector::reduceVectorMinOrMax(dir, b, x, aProbabilistic.getRowGroupIndices());
                        }
                    }
                    // now compute the values for the markovian states. We also keep track of the maximal and minimal difference between two values (for convergence checking)
                    auto vIt = v.begin();
                    uint64_t row = 0;
                    ValueType newValue = markovianChoiceRewards[row] + aMarkovian.multiplyRowWithVector(row, w);
                    if (hasProbabilisticStates) {
                        newValue += aMarkovianToProbabilistic.multiplyRowWithVector(row, x);
                    }
                    ValueType maxDiff = newValue - *vIt;
                    ValueType minDiff = maxDiff;
                    *vIt = newValue;
                    for (++vIt, ++row; row < aMarkovian.getRowCount(); ++vIt, ++row) {
                        newValue = markovianChoiceRewards[row] + aMarkovian.multiplyRowWithVector(row, w);
                        if (hasProbabilisticStates) {
                            newValue += aMarkovianToProbabilistic.multiplyRowWithVector(row, x);
                        }
                        ValueType diff = newValue - *vIt;
                        maxDiff = std::max(maxDiff, diff);
                        minDiff = std::min(minDiff, diff);
                        *vIt = newValue;
                    }

                    // Check for convergence
                    if ((maxDiff - minDiff) <= (relative ? (precision * (v.front() + minDiff)) : precision)) {
                        break;
                    }
                    if (storm::utility::resources::isTerminate()) {
                        break;
                    }
                    
                    // update the rhs of the MinMax equation system
                    ValueType referenceValue = v.front();
                    storm::utility::vector::applyPointwise<ValueType, ValueType>(v, w, [&referenceValue] (ValueType const& v_i) -> ValueType { return v_i - referenceValue; });
                    if (hasProbabilisticStates) {
                        aProbabilisticToMarkovian.multiplyWithVector(w, b);
                        storm::utility::vector::addVectors(b, probabilisticChoiceRewards, b);
                    }
                }
                if (maxIter.is_initialized() && iter == maxIter.get()) {
                    STORM_LOG_WARN("LRA computation did not converge within " << iter << " iterations.");
                } else {
                    STORM_LOG_TRACE("LRA computation converged after " << iter << " iterations.");
                }
                return v.front() * uniformizationRate;
            }
            
            template std::vector<double> SparseMarkovAutomatonCslHelper::computeBoundedUntilProbabilities(Environment const& env, storm::solver::SolveGoal<double>&& goal, storm::storage::SparseMatrix<double> const& transitionMatrix, std::vector<double> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, std::pair<double, double> const& boundsPair);
                
            template MDPSparseModelCheckingHelperReturnType<double> SparseMarkovAutomatonCslHelper::computeUntilProbabilities(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<double> const& transitionMatrix, storm::storage::SparseMatrix<double> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool qualitative, bool produceScheduler);
                
            template MDPSparseModelCheckingHelperReturnType<double> SparseMarkovAutomatonCslHelper::computeReachabilityRewards(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<double> const& transitionMatrix, storm::storage::SparseMatrix<double> const& backwardTransitions, std::vector<double> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::models::sparse::StandardRewardModel<double> const& rewardModel, storm::storage::BitVector const& psiStates, bool produceScheduler);

            template MDPSparseModelCheckingHelperReturnType<double> SparseMarkovAutomatonCslHelper::computeTotalRewards(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<double> const& transitionMatrix, storm::storage::SparseMatrix<double> const& backwardTransitions, std::vector<double> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::models::sparse::StandardRewardModel<double> const& rewardModel, bool produceScheduler);

            template std::vector<double> SparseMarkovAutomatonCslHelper::computeLongRunAverageProbabilities(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<double> const& transitionMatrix, storm::storage::SparseMatrix<double> const& backwardTransitions, std::vector<double> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates);
                
            template std::vector<double> SparseMarkovAutomatonCslHelper::computeLongRunAverageRewards(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<double> const& transitionMatrix, storm::storage::SparseMatrix<double> const& backwardTransitions, std::vector<double> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::models::sparse::StandardRewardModel<double> const& rewardModel);
            
            template MDPSparseModelCheckingHelperReturnType<double> SparseMarkovAutomatonCslHelper::computeReachabilityTimes(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<double> const& transitionMatrix, storm::storage::SparseMatrix<double> const& backwardTransitions, std::vector<double> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, bool produceScheduler);
            
            template double SparseMarkovAutomatonCslHelper::computeLraForMaximalEndComponent(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<double> const& transitionMatrix, std::vector<double> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::models::sparse::StandardRewardModel<double> const& rewardModel, storm::storage::MaximalEndComponent const& mec);
                
            template double SparseMarkovAutomatonCslHelper::computeLraForMaximalEndComponentLP(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<double> const& transitionMatrix, std::vector<double> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::models::sparse::StandardRewardModel<double> const& rewardModel, storm::storage::MaximalEndComponent const& mec);
            
            template double SparseMarkovAutomatonCslHelper::computeLraForMaximalEndComponentVI(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<double> const& transitionMatrix, std::vector<double> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::models::sparse::StandardRewardModel<double> const& rewardModel, storm::storage::MaximalEndComponent const& mec);
            
            template std::vector<storm::RationalNumber> SparseMarkovAutomatonCslHelper::computeBoundedUntilProbabilities(Environment const& env, storm::solver::SolveGoal<storm::RationalNumber>&& goal, storm::storage::SparseMatrix<storm::RationalNumber> const& transitionMatrix, std::vector<storm::RationalNumber> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, std::pair<double, double> const& boundsPair);
                
            template MDPSparseModelCheckingHelperReturnType<storm::RationalNumber> SparseMarkovAutomatonCslHelper::computeUntilProbabilities(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<storm::RationalNumber> const& transitionMatrix, storm::storage::SparseMatrix<storm::RationalNumber> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool qualitative, bool produceScheduler);
                
            template MDPSparseModelCheckingHelperReturnType<storm::RationalNumber> SparseMarkovAutomatonCslHelper::computeReachabilityRewards(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<storm::RationalNumber> const& transitionMatrix, storm::storage::SparseMatrix<storm::RationalNumber> const& backwardTransitions, std::vector<storm::RationalNumber> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::models::sparse::StandardRewardModel<storm::RationalNumber> const& rewardModel, storm::storage::BitVector const& psiStates, bool produceScheduler);

            template MDPSparseModelCheckingHelperReturnType<storm::RationalNumber> SparseMarkovAutomatonCslHelper::computeTotalRewards(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<storm::RationalNumber> const& transitionMatrix, storm::storage::SparseMatrix<storm::RationalNumber> const& backwardTransitions, std::vector<storm::RationalNumber> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::models::sparse::StandardRewardModel<storm::RationalNumber> const& rewardModel, bool produceScheduler);

            template std::vector<storm::RationalNumber> SparseMarkovAutomatonCslHelper::computeLongRunAverageProbabilities(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<storm::RationalNumber> const& transitionMatrix, storm::storage::SparseMatrix<storm::RationalNumber> const& backwardTransitions, std::vector<storm::RationalNumber> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates);
            
            template std::vector<storm::RationalNumber> SparseMarkovAutomatonCslHelper::computeLongRunAverageRewards(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<storm::RationalNumber> const& transitionMatrix, storm::storage::SparseMatrix<storm::RationalNumber> const& backwardTransitions, std::vector<storm::RationalNumber> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::models::sparse::StandardRewardModel<storm::RationalNumber> const& rewardModel);
            
            template MDPSparseModelCheckingHelperReturnType<storm::RationalNumber> SparseMarkovAutomatonCslHelper::computeReachabilityTimes(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<storm::RationalNumber> const& transitionMatrix, storm::storage::SparseMatrix<storm::RationalNumber> const& backwardTransitions, std::vector<storm::RationalNumber> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, bool produceScheduler);
                
            template storm::RationalNumber SparseMarkovAutomatonCslHelper::computeLraForMaximalEndComponent(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<storm::RationalNumber> const& transitionMatrix, std::vector<storm::RationalNumber> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::models::sparse::StandardRewardModel<storm::RationalNumber> const& rewardModel, storm::storage::MaximalEndComponent const& mec);
            
            template storm::RationalNumber SparseMarkovAutomatonCslHelper::computeLraForMaximalEndComponentLP(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<storm::RationalNumber> const& transitionMatrix, std::vector<storm::RationalNumber> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::models::sparse::StandardRewardModel<storm::RationalNumber> const& rewardModel, storm::storage::MaximalEndComponent const& mec);
            
            template storm::RationalNumber SparseMarkovAutomatonCslHelper::computeLraForMaximalEndComponentVI(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<storm::RationalNumber> const& transitionMatrix, std::vector<storm::RationalNumber> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::models::sparse::StandardRewardModel<storm::RationalNumber> const& rewardModel, storm::storage::MaximalEndComponent const& mec);
                
        }
    }
}
