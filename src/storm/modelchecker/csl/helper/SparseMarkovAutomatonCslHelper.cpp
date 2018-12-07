#include "storm/modelchecker/csl/helper/SparseMarkovAutomatonCslHelper.h"

#include "storm/modelchecker/prctl/helper/SparseMdpPrctlHelper.h"

#include "storm/models/sparse/StandardRewardModel.h"

#include "storm/storage/StronglyConnectedComponentDecomposition.h"
#include "storm/storage/MaximalEndComponentDecomposition.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/GeneralSettings.h"
#include "storm/settings/modules/MinMaxEquationSolverSettings.h"

#include "storm/environment/Environment.h"
#include "storm/environment/solver/MinMaxSolverEnvironment.h"

#include "storm/utility/macros.h"
#include "storm/utility/vector.h"
#include "storm/utility/graph.h"
#include "storm/utility/NumberTraits.h"

#include "storm/storage/expressions/Variable.h"
#include "storm/storage/expressions/Expression.h"
#include "storm/storage/expressions/ExpressionManager.h"

#include "storm/solver/MinMaxLinearEquationSolver.h"
#include "storm/solver/LpSolver.h"

#include "storm/exceptions/InvalidStateException.h"
#include "storm/exceptions/InvalidPropertyException.h"
#include "storm/exceptions/InvalidOperationException.h"
#include "storm/exceptions/UncheckedRequirementException.h"

namespace storm {
    namespace modelchecker {
        namespace helper {

            /**
             * Data structure holding result vectors (vLower, vUpper, wUpper) for Unif+.
             */
            template<typename ValueType>
            struct UnifPlusVectors {
                UnifPlusVectors() {
                    // Intentionally empty
                }

                /**
                 * Initialize results vectors. vLowerOld, vUpperOld and wUpper[k=N] are initialized with zeros.
                 */
                UnifPlusVectors(uint64_t steps, uint64_t noStates) : numberOfStates(noStates), steps(steps), resLowerOld(numberOfStates, storm::utility::zero<ValueType>()), resLowerNew(numberOfStates, -1), resUpper(numberOfStates, storm::utility::zero<ValueType>()), wUpperOld(numberOfStates, storm::utility::zero<ValueType>()), wUpperNew(numberOfStates, -1) {
                    // Intentionally left empty
                }

                /**
                 * Prepare new iteration by setting the new result vectors as old result vectors, and initializing the new result vectors with -1 again.
                 */
                void prepareNewIteration() {
                    resLowerOld.swap(resLowerNew);
                    std::fill(resLowerNew.begin(), resLowerNew.end(), -1);
                    wUpperOld.swap(wUpperNew);
                    std::fill(wUpperNew.begin(), wUpperNew.end(), -1);
                }

                uint64_t numberOfStates;
                uint64_t steps;
                std::vector<ValueType> resLowerOld;
                std::vector<ValueType> resLowerNew;
                std::vector<ValueType> resUpper;
                std::vector<ValueType> wUpperOld;
                std::vector<ValueType> wUpperNew;
            };
            
            template<typename ValueType>
            void calculateUnifPlusVector(Environment const& env, uint64_t k, uint64_t state, bool calcLower, ValueType lambda, uint64_t numberOfProbabilisticChoices, std::vector<std::vector<ValueType>> const & relativeReachability, OptimizationDirection dir, UnifPlusVectors<ValueType>& unifVectors, storm::storage::SparseMatrix<ValueType> const& fullTransitionMatrix, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> const& solver, storm::utility::numerical::FoxGlynnResult<ValueType> const& poisson, bool cycleFree) {
                // Set reference to acutal vector
                std::vector<ValueType>& resVectorOld = calcLower ? unifVectors.resLowerOld : unifVectors.wUpperOld;
                std::vector<ValueType>& resVectorNew = calcLower ? unifVectors.resLowerNew : unifVectors.wUpperNew;

                if (resVectorNew[state] != -1) {
                    // Result already calculated.
                    return;
                }
                
                auto numberOfStates = fullTransitionMatrix.getRowGroupCount();
                uint64_t N = unifVectors.steps;
                auto const& rowGroupIndices = fullTransitionMatrix.getRowGroupIndices();
                ValueType res;
                
                // First case, k==N, independent from kind of state.
                if (k == N) {
                    STORM_LOG_ASSERT(false, "Result for k=N was already calculated.");
                    resVectorNew[state] = storm::utility::zero<ValueType>();
                    return;
                }
                
                // Goal state, independent from kind of state.
                if (psiStates[state]) {
                    if (calcLower) {
                        // v lower
                        res = storm::utility::zero<ValueType>();
                        for (uint64_t i = k; i < N; ++i){
                            if (i >= poisson.left && i <= poisson.right) {
                                res += poisson.weights[i - poisson.left];
                            }
                        }
                        resVectorNew[state] = res;
                    } else {
                        // w upper
                        resVectorNew[state] = storm::utility::one<ValueType>();
                    }
                    return;
                }


                // Markovian non-goal state.
                if (markovianStates[state]) {
                    res = storm::utility::zero<ValueType>();
                    for (auto const& element : fullTransitionMatrix.getRow(rowGroupIndices[state])) {
                        uint64_t successor = element.getColumn();
                        if (resVectorOld[successor] == -1) {
                            STORM_LOG_ASSERT(false, "Need to calculate previous result.");
                            calculateUnifPlusVector(env, k+1, successor, calcLower, lambda, numberOfProbabilisticChoices, relativeReachability, dir, unifVectors, fullTransitionMatrix, markovianStates, psiStates, solver, poisson, cycleFree);
                        }
                        res += element.getValue() * resVectorOld[successor];
                    }
                    resVectorNew[state]=res;
                    return;
                }
                
                // Probabilistic non-goal state.
                if (cycleFree) {
                    // If the model is cycle free, do "slight value iteration". (What is that?)
                    res = -1;
                    for (uint64_t i = rowGroupIndices[state]; i < rowGroupIndices[state + 1]; ++i) {
                        auto row = fullTransitionMatrix.getRow(i);
                        ValueType between = storm::utility::zero<ValueType>();
                        for (auto const& element : row) {
                            uint64_t successor = element.getColumn();
                            
                            // This should never happen, right? The model has no cycles, and therefore also no self-loops.
                            if (successor == state) {
                                continue;
                            }
                            
                            if (resVectorNew[successor] == -1) {
                                calculateUnifPlusVector(env, k, successor, calcLower, lambda, numberOfProbabilisticChoices, relativeReachability, dir, unifVectors, fullTransitionMatrix, markovianStates, psiStates, solver, poisson, cycleFree);
                            }
                            between += element.getValue() * resVectorNew[successor];
                        }
                        if (maximize(dir)) {
                            res = storm::utility::max(res, between);
                        } else {
                            if (res != -1) {
                                res = storm::utility::min(res, between);
                            } else {
                                res = between;
                            }
                        }
                    }
                    resVectorNew[state] = res;
                    return;
                }
                
                // If we arrived at this point, the model is not cycle free. Use the solver to solve the underlying equation system.
                uint64_t numberOfProbabilisticStates = numberOfStates - markovianStates.getNumberOfSetBits();
                std::vector<ValueType> b(numberOfProbabilisticChoices, storm::utility::zero<ValueType>());
                std::vector<ValueType> x(numberOfProbabilisticStates, storm::utility::zero<ValueType>());
                
                // Compute right-hand side vector b.
                uint64_t row = 0;
                for (uint64_t i = 0; i < numberOfStates; ++i) {
                    if (markovianStates[i]) {
                        continue;
                    }

                    for (auto j = rowGroupIndices[i]; j < rowGroupIndices[i + 1]; j++) {
                        uint64_t stateCount = 0;
                        res = storm::utility::zero<ValueType>();
                        for (auto const& element : fullTransitionMatrix.getRow(j)) {
                            auto successor = element.getColumn();
                            if (!markovianStates[successor]) {
                                continue;
                            }
                            
                            if (resVectorNew[successor] == -1) {
                                calculateUnifPlusVector(env, k, successor, calcLower, lambda, numberOfProbabilisticStates, relativeReachability, dir, unifVectors, fullTransitionMatrix, markovianStates, psiStates, solver,  poisson, cycleFree);
                            }
                            res += relativeReachability[j][stateCount] * resVectorNew[successor];
                            ++stateCount;
                        }
                        
                        b[row] = res;
                        ++row;
                    }
                }
                
                // Solve the equation system.
                solver->solveEquations(env, dir, x, b);

                // Expand the solution for the probabilistic states to all states.
                storm::utility::vector::setVectorValues(resVectorNew, ~markovianStates, x);
            }

            template <typename ValueType>
            void eliminateProbabilisticSelfLoops(storm::storage::SparseMatrix<ValueType>& transitionMatrix, storm::storage::BitVector const& markovianStates) {
                auto const& rowGroupIndices = transitionMatrix.getRowGroupIndices();

                for (uint64_t i = 0; i < transitionMatrix.getRowGroupCount(); ++i) {
                    if (markovianStates[i]) {
                        continue;
                    }
                    
                    for (uint64_t j = rowGroupIndices[i]; j < rowGroupIndices[i + 1]; j++) {
                        ValueType selfLoop = storm::utility::zero<ValueType>();
                        for (auto const& element: transitionMatrix.getRow(j)){
                            if (element.getColumn() == i) {
                                selfLoop += element.getValue();
                            }
                        }

                        if (storm::utility::isZero(selfLoop)) {
                            continue;
                        }
                        
                        for (auto& element : transitionMatrix.getRow(j)) {
                            if (element.getColumn() != i) {
                                if (!storm::utility::isOne(selfLoop)) {
                                    element.setValue(element.getValue() / (storm::utility::one<ValueType>() - selfLoop));
                                }
                            } else {
                                element.setValue(storm::utility::zero<ValueType>());
                            }
                        }
                    }
                }
            }

            template<typename ValueType>
            std::vector<ValueType> computeBoundedUntilProbabilitiesUnifPlus(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, std::pair<double, double> const& boundsPair) {
                STORM_LOG_TRACE("Using UnifPlus to compute bounded until probabilities.");

                // Obtain bit vectors to identify different kind of states.
                storm::storage::BitVector allStates(markovianStates.size(), true);
                storm::storage::BitVector probabilisticStates = ~markovianStates;

                // Searching for SCCs in probabilistic fragment to decide which algorithm is applied.
                storm::storage::StronglyConnectedComponentDecomposition<ValueType> sccDecomposition(transitionMatrix, probabilisticStates, true, false);
                bool cycleFree = sccDecomposition.empty();

                // Vectors to store computed vectors.
                UnifPlusVectors<ValueType> unifVectors;

                // Transitions from goal states will be ignored. However, we mark them as non-probabilistic to make sure
                // we do not apply the MDP algorithm to them.
                storm::storage::BitVector markovianAndGoalStates = markovianStates | psiStates;
                probabilisticStates &= ~psiStates;

                std::vector<ValueType> mutableExitRates = exitRateVector;
                
                // Extend the transition matrix with diagonal entries so we can change them easily during the uniformization step.
                typename storm::storage::SparseMatrix<ValueType> fullTransitionMatrix = transitionMatrix.getSubmatrix(true, allStates, allStates, true);

                // Eliminate self-loops of probabilistic states. Is this really needed for the "slight value iteration" process?
                eliminateProbabilisticSelfLoops(fullTransitionMatrix, markovianAndGoalStates);
                typename storm::storage::SparseMatrix<ValueType> probMatrix;
                uint64_t numberOfProbabilisticChoices = 0;
                if (!probabilisticStates.empty()) {
                    probMatrix = fullTransitionMatrix.getSubmatrix(true, probabilisticStates, probabilisticStates, true);
                    numberOfProbabilisticChoices = probMatrix.getRowCount();
                }

                // Get row grouping of transition matrix.
                auto const& rowGroupIndices = fullTransitionMatrix.getRowGroupIndices();

                // (1) define/declare horizon, epsilon, kappa, N, lambda, maxNorm
                uint64_t numberOfStates = fullTransitionMatrix.getRowGroupCount();
                // 'Unpack' the bounds to make them more easily accessible.
                double lowerBound = boundsPair.first;
                double upperBound = boundsPair.second;
                // Lower bound > 0 is not implemented!
                STORM_LOG_THROW(lowerBound == 0, storm::exceptions::NotImplementedException, "Support for lower bound > 0 not implemented in Unif+.");
                // Truncation error
                // TODO: make kappa a parameter.
                ValueType kappa = storm::utility::one<ValueType>() / 10;
                // Approximation error
                ValueType epsilon = storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision();
                // Lambda is largest exit rate
                ValueType lambda = exitRateVector[0];
                for (ValueType const& rate : exitRateVector) {
                    lambda = std::max(rate, lambda);
                }
                STORM_LOG_DEBUG("Initial lambda is " << lambda << ".");

                // Compute the relative reachability vectors and create solver for models with SCCs.
                std::vector<std::vector<ValueType>> relativeReachabilities(transitionMatrix.getRowCount());
                std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> solver;
                if (!cycleFree) {
                    for (uint64_t i = 0; i < numberOfStates; i++) {
                        if (markovianAndGoalStates[i]) {
                            continue;
                        }

                        for (auto j = rowGroupIndices[i]; j < rowGroupIndices[i + 1]; ++j) {
                            for (auto const& element : fullTransitionMatrix.getRow(j)) {
                                if (markovianAndGoalStates[element.getColumn()]) {
                                    relativeReachabilities[j].push_back(element.getValue());
                                }
                            }
                        }
                    }

                    // Create solver.
                    storm::solver::GeneralMinMaxLinearEquationSolverFactory<ValueType> minMaxLinearEquationSolverFactory;
                    storm::solver::MinMaxLinearEquationSolverRequirements requirements = minMaxLinearEquationSolverFactory.getRequirements(env, true, dir);
                    requirements.clearBounds();
                    STORM_LOG_THROW(!requirements.hasEnabledCriticalRequirement(), storm::exceptions::UncheckedRequirementException, "Solver requirements " + requirements.getEnabledRequirementsAsString() + " not checked.");
                    
                    if (numberOfProbabilisticChoices > 0) {
                        solver = minMaxLinearEquationSolverFactory.create(env, probMatrix);
                        solver->setHasUniqueSolution();
                        solver->setBounds(storm::utility::zero<ValueType>(), storm::utility::one<ValueType>());
                        solver->setRequirementsChecked();
                        solver->setCachingEnabled(true);
                    }
                }

                ValueType maxNorm = storm::utility::zero<ValueType>();
                // Maximal step size
                uint64_t N;
                // Loop until result is within precision bound.
                do {
                    // (2) update parameter
                    N = storm::utility::ceil(lambda * upperBound * std::exp(2) - storm::utility::log(kappa * epsilon));

                    // (3) uniform  - just applied to Markovian states.
                    for (uint64_t i = 0; i < numberOfStates; i++) {
                        if (!markovianAndGoalStates[i] || psiStates[i]) {
                            continue;
                        }
                        
                        // As the current state is Markovian, its branching probabilities are stored within one row.
                        uint64_t markovianRowIndex = rowGroupIndices[i];

                        if (mutableExitRates[i] == lambda) {
                            // Already uniformized.
                            continue;
                        }

                        auto markovianRow = fullTransitionMatrix.getRow(markovianRowIndex);
                        ValueType oldExitRate = mutableExitRates[i];
                        ValueType newExitRate = lambda;
                        for (auto& v : markovianRow) {
                            if (v.getColumn() == i) {
                                ValueType newSelfLoop = newExitRate - oldExitRate + v.getValue() * oldExitRate;
                                ValueType newRate = newSelfLoop / newExitRate;
                                v.setValue(newRate);
                            } else {
                                ValueType oldProbability = v.getValue();
                                ValueType newProbability = oldProbability * oldExitRate / newExitRate;
                                v.setValue(newProbability);
                            }
                        }
                        mutableExitRates[i] = newExitRate;
                    }

                    // Compute poisson distribution.
                    storm::utility::numerical::FoxGlynnResult<ValueType> foxGlynnResult = storm::utility::numerical::foxGlynn(lambda * upperBound, epsilon * kappa / 100);

                    // Scale the weights so they sum to one.
                    for (auto& element : foxGlynnResult.weights) {
                        element /= foxGlynnResult.totalWeight;
                    }

                    // (4) Define vectors/matrices.
                    // Initialize result vectors and already insert zeros for iteration N
                    unifVectors = UnifPlusVectors<ValueType>(N, numberOfStates);

                    // (5) Compute vectors and maxNorm.
                    // Iteration k = N was already performed by initializing with zeros.

                    // Iterations k < N
                    for (int64_t k = N-1; k >= 0; --k) {
                        if (k < (int64_t)(N-1)) {
                            unifVectors.prepareNewIteration();
                        }
                        for (uint64_t state = 0; state < numberOfStates; ++state) {
                            // Calculate results for lower bound and wUpper
                            calculateUnifPlusVector(env, k, state, true, lambda, numberOfProbabilisticChoices, relativeReachabilities, dir, unifVectors, fullTransitionMatrix, markovianAndGoalStates, psiStates, solver, foxGlynnResult, cycleFree);
                            calculateUnifPlusVector(env, k, state, false, lambda, numberOfProbabilisticChoices, relativeReachabilities, dir, unifVectors, fullTransitionMatrix, markovianAndGoalStates, psiStates, solver, foxGlynnResult, cycleFree);
                            // Calculate result for upper bound
                            uint64_t index = N-1-k;
                            if (index >= foxGlynnResult.left && index <= foxGlynnResult.right) {
                                STORM_LOG_ASSERT(unifVectors.wUpperNew[state] != -1, "wUpper was not computed before.");
                                unifVectors.resUpper[state] += foxGlynnResult.weights[index - foxGlynnResult.left] * unifVectors.wUpperNew[state];
                            }
                        }

                    }

                    // Only iterate over result vector, as the results can only get more precise.
                    maxNorm = storm::utility::zero<ValueType>();
                    for (uint64_t i = 0; i < numberOfStates; i++){
                        ValueType diff = storm::utility::abs(unifVectors.resUpper[i] - unifVectors.resLowerNew[i]);
                        maxNorm = std::max(maxNorm, diff);
                    }

                    // (6) Double lambda.
                    lambda *= 2;
                    STORM_LOG_DEBUG("Increased lambda to " << lambda << ", max diff is " << maxNorm << ".");

                } while (maxNorm > epsilon * (1 - kappa));

                return unifVectors.resLowerNew;
            }

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
                
                // Check for requirements of the solver.
                // The solution is unique as we assume non-zeno MAs.
                storm::solver::GeneralMinMaxLinearEquationSolverFactory<ValueType> minMaxLinearEquationSolverFactory;
                storm::solver::MinMaxLinearEquationSolverRequirements requirements = minMaxLinearEquationSolverFactory.getRequirements(env, true, dir);
                requirements.clearBounds();
                STORM_LOG_THROW(!requirements.hasEnabledCriticalRequirement(), storm::exceptions::UncheckedRequirementException, "Solver requirements " + requirements.getEnabledRequirementsAsString() + " not checked.");
                
                std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> solver = minMaxLinearEquationSolverFactory.create(env, aProbabilistic);
                solver->setHasUniqueSolution();
                solver->setBounds(storm::utility::zero<ValueType>(), storm::utility::one<ValueType>());
                solver->setRequirementsChecked();
                solver->setCachingEnabled(true);
                
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
                        solver->solveEquations(env, dir, probabilisticNonGoalValues, bProbabilistic);
                        
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
                }
                
                if (existProbabilisticStates) {
                    // After the loop, perform one more step of the value iteration for PS states.
                    aProbabilisticToMarkovian.multiplyWithVector(markovianNonGoalValues, bProbabilistic);
                    storm::utility::vector::addVectors(bProbabilistic, bProbabilisticFixed, bProbabilistic);
                    solver->solveEquations(env, dir, probabilisticNonGoalValues, bProbabilistic);
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
                ValueType delta = (2 * storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision()) / (upperBound * maxExitRate * maxExitRate);
                
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
            std::vector<ValueType> SparseMarkovAutomatonCslHelper::computeBoundedUntilProbabilities(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, std::pair<double, double> const& boundsPair) {
                auto const& settings = storm::settings::getModule<storm::settings::modules::MinMaxEquationSolverSettings>();
                if (settings.getMarkovAutomatonBoundedReachabilityMethod() == storm::settings::modules::MinMaxEquationSolverSettings::MarkovAutomatonBoundedReachabilityMethod::Imca) {
                    return computeBoundedUntilProbabilitiesImca(env, dir, transitionMatrix, exitRateVector, markovianStates, psiStates, boundsPair);
                } else {
                    STORM_LOG_ASSERT(settings.getMarkovAutomatonBoundedReachabilityMethod() == storm::settings::modules::MinMaxEquationSolverSettings::MarkovAutomatonBoundedReachabilityMethod::UnifPlus, "Unknown solution method.");
                    if (!storm::utility::isZero(boundsPair.first)) {
                        STORM_LOG_WARN("Using IMCA method because Unif+ does not support a lower bound > 0.");
                        return computeBoundedUntilProbabilitiesImca(env, dir, transitionMatrix, exitRateVector, markovianStates, psiStates, boundsPair);
                    } else {
                        return computeBoundedUntilProbabilitiesUnifPlus(env, dir, transitionMatrix, exitRateVector, markovianStates, psiStates, boundsPair);
                    }
                }
            }
              
            template <typename ValueType, typename std::enable_if<!storm::NumberTraits<ValueType>::SupportsExponential, int>::type>
            std::vector<ValueType> SparseMarkovAutomatonCslHelper::computeBoundedUntilProbabilities(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, std::pair<double, double> const& boundsPair) {
                STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Computing bounded until probabilities is unsupported for this value type.");
            }

            template<typename ValueType>
            std::vector<ValueType> SparseMarkovAutomatonCslHelper::computeUntilProbabilities(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool qualitative) {
                return std::move(storm::modelchecker::helper::SparseMdpPrctlHelper<ValueType>::computeUntilProbabilities(env, dir, transitionMatrix, backwardTransitions, phiStates, psiStates, qualitative, false).values);
            }
            
            template <typename ValueType, typename RewardModelType>
            std::vector<ValueType> SparseMarkovAutomatonCslHelper::computeTotalRewards(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, RewardModelType const& rewardModel) {
                
                // Get a reward model where the state rewards are scaled accordingly
                std::vector<ValueType> stateRewardWeights(transitionMatrix.getRowGroupCount(), storm::utility::zero<ValueType>());
                for (auto const markovianState : markovianStates) {
                    stateRewardWeights[markovianState] = storm::utility::one<ValueType>() / exitRateVector[markovianState];
                }
                std::vector<ValueType> totalRewardVector = rewardModel.getTotalActionRewardVector(transitionMatrix, stateRewardWeights);
                RewardModelType scaledRewardModel(boost::none, std::move(totalRewardVector));
                
                return SparseMdpPrctlHelper<ValueType>::computeTotalRewards(env, dir, transitionMatrix, backwardTransitions, scaledRewardModel, false, false).values;
            }
            
            template <typename ValueType, typename RewardModelType>
            std::vector<ValueType> SparseMarkovAutomatonCslHelper::computeReachabilityRewards(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, RewardModelType const& rewardModel, storm::storage::BitVector const& psiStates) {
                
                // Get a reward model where the state rewards are scaled accordingly
                std::vector<ValueType> stateRewardWeights(transitionMatrix.getRowGroupCount(), storm::utility::zero<ValueType>());
                for (auto const markovianState : markovianStates) {
                    stateRewardWeights[markovianState] = storm::utility::one<ValueType>() / exitRateVector[markovianState];
                }
                std::vector<ValueType> totalRewardVector = rewardModel.getTotalActionRewardVector(transitionMatrix, stateRewardWeights);
                RewardModelType scaledRewardModel(boost::none, std::move(totalRewardVector));
                
                return SparseMdpPrctlHelper<ValueType>::computeReachabilityRewards(env, dir, transitionMatrix, backwardTransitions, scaledRewardModel, psiStates, false, false).values;
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
                    underlyingSolverEnvironment.solver().minMax().setPrecision(env.solver().minMax().getPrecision() / storm::utility::convertNumber<storm::RationalNumber>(2));
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
                        boost::container::flat_set<uint64_t> const& choicesInMec = stateChoicesPair.second;
                        
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
                storm::solver::MinMaxLinearEquationSolverRequirements requirements = minMaxLinearEquationSolverFactory.getRequirements(underlyingSolverEnvironment, true, dir);
                requirements.clearBounds();
                STORM_LOG_THROW(!requirements.hasEnabledCriticalRequirement(), storm::exceptions::UncheckedRequirementException, "Solver requirements " + requirements.getEnabledRequirementsAsString() + " not checked.");

                std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> solver = minMaxLinearEquationSolverFactory.create(underlyingSolverEnvironment, sspMatrix);
                solver->setHasUniqueSolution();
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
            std::vector<ValueType> SparseMarkovAutomatonCslHelper::computeReachabilityTimes(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates) {
                
                // Get a reward model representing expected sojourn times
                std::vector<ValueType> rewardValues(transitionMatrix.getRowCount(), storm::utility::zero<ValueType>());
                for (auto const markovianState : markovianStates) {
                    rewardValues[transitionMatrix.getRowGroupIndices()[markovianState]] = storm::utility::one<ValueType>() / exitRateVector[markovianState];
                }
                storm::models::sparse::StandardRewardModel<ValueType> rewardModel(boost::none, std::move(rewardValues));
                
                return SparseMdpPrctlHelper<ValueType>::computeReachabilityRewards(env, dir, transitionMatrix, backwardTransitions, rewardModel, psiStates, false, false).values;
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
                auto minMaxSettings = storm::settings::getModule<storm::settings::modules::MinMaxEquationSolverSettings>();
                storm::solver::LraMethod method = minMaxSettings.getLraMethod();
                if (storm::NumberTraits<ValueType>::IsExact && minMaxSettings.isLraMethodSetFromDefaultValue() && method != storm::solver::LraMethod::LinearProgramming) {
                    STORM_LOG_INFO("Selecting 'LP' as the solution technique for long-run properties to guarantee exact results. If you want to override this, please explicitly specify a different LRA method.");
                    method = storm::solver::LraMethod::LinearProgramming;
                } else if (env.solver().isForceSoundness() && minMaxSettings.isLraMethodSetFromDefaultValue() && method != storm::solver::LraMethod::ValueIteration) {
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
                uniformizationRate += storm::utility::one<ValueType>(); // Todo: try other values such as *=1.01

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
                
                ValueType precision = storm::utility::convertNumber<ValueType>(env.solver().minMax().getPrecision()) / uniformizationRate;
                bool relative = env.solver().minMax().getRelativeTerminationCriterion();
                std::vector<ValueType> v(aMarkovian.getRowCount(), storm::utility::zero<ValueType>());
                std::vector<ValueType> w = v;
                std::vector<ValueType> x, b;
                std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> solver;
                if (hasProbabilisticStates) {
                    x.resize(aProbabilistic.getRowGroupCount(), storm::utility::zero<ValueType>());
                    b = probabilisticChoiceRewards;
                    
                    // Check for requirements of the solver.
                    // The solution is unique as we assume non-zeno MAs.
                    storm::solver::GeneralMinMaxLinearEquationSolverFactory<ValueType> minMaxLinearEquationSolverFactory;
                    storm::solver::MinMaxLinearEquationSolverRequirements requirements = minMaxLinearEquationSolverFactory.getRequirements(env, true, dir);
                    requirements.clearLowerBounds();
                    STORM_LOG_THROW(!requirements.hasEnabledCriticalRequirement(), storm::exceptions::UncheckedRequirementException, "Solver requirements " + requirements.getEnabledRequirementsAsString() + " not checked.");
    
                    solver = minMaxLinearEquationSolverFactory.create(env, std::move(aProbabilistic));
                    solver->setLowerBound(storm::utility::zero<ValueType>());
                    solver->setHasUniqueSolution(true);
                    solver->setRequirementsChecked(true);
                    solver->setCachingEnabled(true);
                }
                
                while (true) {
                    // Compute the expected total rewards for the probabilistic states
                    if (hasProbabilisticStates) {
                        solver->solveEquations(env, dir, x, b);
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
                    if ((maxDiff - minDiff) <= (relative ? (precision * minDiff) : precision)) {
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
                return v.front() * uniformizationRate;
            
            }
            
            template std::vector<double> SparseMarkovAutomatonCslHelper::computeBoundedUntilProbabilities(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<double> const& transitionMatrix, std::vector<double> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, std::pair<double, double> const& boundsPair);
                
            template std::vector<double> SparseMarkovAutomatonCslHelper::computeUntilProbabilities(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<double> const& transitionMatrix, storm::storage::SparseMatrix<double> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool qualitative);
                
            template std::vector<double> SparseMarkovAutomatonCslHelper::computeReachabilityRewards(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<double> const& transitionMatrix, storm::storage::SparseMatrix<double> const& backwardTransitions, std::vector<double> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::models::sparse::StandardRewardModel<double> const& rewardModel, storm::storage::BitVector const& psiStates);

            template std::vector<double> SparseMarkovAutomatonCslHelper::computeTotalRewards(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<double> const& transitionMatrix, storm::storage::SparseMatrix<double> const& backwardTransitions, std::vector<double> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::models::sparse::StandardRewardModel<double> const& rewardModel);

            template std::vector<double> SparseMarkovAutomatonCslHelper::computeLongRunAverageProbabilities(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<double> const& transitionMatrix, storm::storage::SparseMatrix<double> const& backwardTransitions, std::vector<double> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates);
                
            template std::vector<double> SparseMarkovAutomatonCslHelper::computeLongRunAverageRewards(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<double> const& transitionMatrix, storm::storage::SparseMatrix<double> const& backwardTransitions, std::vector<double> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::models::sparse::StandardRewardModel<double> const& rewardModel);
            
            template std::vector<double> SparseMarkovAutomatonCslHelper::computeReachabilityTimes(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<double> const& transitionMatrix, storm::storage::SparseMatrix<double> const& backwardTransitions, std::vector<double> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates);
            
            template double SparseMarkovAutomatonCslHelper::computeLraForMaximalEndComponent(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<double> const& transitionMatrix, std::vector<double> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::models::sparse::StandardRewardModel<double> const& rewardModel, storm::storage::MaximalEndComponent const& mec);
                
            template double SparseMarkovAutomatonCslHelper::computeLraForMaximalEndComponentLP(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<double> const& transitionMatrix, std::vector<double> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::models::sparse::StandardRewardModel<double> const& rewardModel, storm::storage::MaximalEndComponent const& mec);
            
            template double SparseMarkovAutomatonCslHelper::computeLraForMaximalEndComponentVI(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<double> const& transitionMatrix, std::vector<double> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::models::sparse::StandardRewardModel<double> const& rewardModel, storm::storage::MaximalEndComponent const& mec);
            
            template std::vector<storm::RationalNumber> SparseMarkovAutomatonCslHelper::computeBoundedUntilProbabilities(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<storm::RationalNumber> const& transitionMatrix, std::vector<storm::RationalNumber> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, std::pair<double, double> const& boundsPair);
                
            template std::vector<storm::RationalNumber> SparseMarkovAutomatonCslHelper::computeUntilProbabilities(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<storm::RationalNumber> const& transitionMatrix, storm::storage::SparseMatrix<storm::RationalNumber> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool qualitative);
                
            template std::vector<storm::RationalNumber> SparseMarkovAutomatonCslHelper::computeReachabilityRewards(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<storm::RationalNumber> const& transitionMatrix, storm::storage::SparseMatrix<storm::RationalNumber> const& backwardTransitions, std::vector<storm::RationalNumber> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::models::sparse::StandardRewardModel<storm::RationalNumber> const& rewardModel, storm::storage::BitVector const& psiStates);

            template std::vector<storm::RationalNumber> SparseMarkovAutomatonCslHelper::computeTotalRewards(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<storm::RationalNumber> const& transitionMatrix, storm::storage::SparseMatrix<storm::RationalNumber> const& backwardTransitions, std::vector<storm::RationalNumber> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::models::sparse::StandardRewardModel<storm::RationalNumber> const& rewardModel);

            template std::vector<storm::RationalNumber> SparseMarkovAutomatonCslHelper::computeLongRunAverageProbabilities(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<storm::RationalNumber> const& transitionMatrix, storm::storage::SparseMatrix<storm::RationalNumber> const& backwardTransitions, std::vector<storm::RationalNumber> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates);
            
            template std::vector<storm::RationalNumber> SparseMarkovAutomatonCslHelper::computeLongRunAverageRewards(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<storm::RationalNumber> const& transitionMatrix, storm::storage::SparseMatrix<storm::RationalNumber> const& backwardTransitions, std::vector<storm::RationalNumber> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::models::sparse::StandardRewardModel<storm::RationalNumber> const& rewardModel);
            
            template std::vector<storm::RationalNumber> SparseMarkovAutomatonCslHelper::computeReachabilityTimes(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<storm::RationalNumber> const& transitionMatrix, storm::storage::SparseMatrix<storm::RationalNumber> const& backwardTransitions, std::vector<storm::RationalNumber> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates);
                
            template storm::RationalNumber SparseMarkovAutomatonCslHelper::computeLraForMaximalEndComponent(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<storm::RationalNumber> const& transitionMatrix, std::vector<storm::RationalNumber> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::models::sparse::StandardRewardModel<storm::RationalNumber> const& rewardModel, storm::storage::MaximalEndComponent const& mec);
            
            template storm::RationalNumber SparseMarkovAutomatonCslHelper::computeLraForMaximalEndComponentLP(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<storm::RationalNumber> const& transitionMatrix, std::vector<storm::RationalNumber> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::models::sparse::StandardRewardModel<storm::RationalNumber> const& rewardModel, storm::storage::MaximalEndComponent const& mec);
            
            template storm::RationalNumber SparseMarkovAutomatonCslHelper::computeLraForMaximalEndComponentVI(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<storm::RationalNumber> const& transitionMatrix, std::vector<storm::RationalNumber> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::models::sparse::StandardRewardModel<storm::RationalNumber> const& rewardModel, storm::storage::MaximalEndComponent const& mec);
                
        }
    }
}
