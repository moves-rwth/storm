#include "SparseNondeterministicInfiniteHorizonHelper.h"

#include "storm/solver/MinMaxLinearEquationSolver.h"
#include "storm/solver/Multiplier.h"
#include "storm/solver/LpSolver.h"

#include "storm/utility/graph.h"
#include "storm/utility/SignalHandler.h"
#include "storm/utility/solver.h"
#include "storm/utility/vector.h"

#include "storm/environment/solver/LongRunAverageSolverEnvironment.h"
#include "storm/environment/solver/MinMaxSolverEnvironment.h"

#include "storm/exceptions/UnmetRequirementException.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
        
            template <typename ValueType>
            SparseNondeterministicInfiniteHorizonHelper<ValueType>::SparseNondeterministicInfiniteHorizonHelper(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions) : _transitionMatrix(transitionMatrix), _backwardTransitions(backwardTransitions), _markovianStates(nullptr), _exitRates(nullptr), _produceScheduler(false) {
                // Intentionally left empty.
            }
            
            template <typename ValueType>
            SparseNondeterministicInfiniteHorizonHelper<ValueType>::SparseNondeterministicInfiniteHorizonHelper(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& markovianStates, std::vector<ValueType> const& exitRates) : _transitionMatrix(transitionMatrix), _backwardTransitions(backwardTransitions), _markovianStates(&markovianStates), _exitRates(&exitRates), _produceScheduler(false) {
                // Intentionally left empty.
            }
            
            template <typename ValueType>
            std::vector<ValueType> SparseNondeterministicInfiniteHorizonHelper<ValueType>::computeLongRunAverageProbabilities(Environment const& env, storm::storage::BitVector const& psiStates) {
                return computeLongRunAverageValues(env,
                            [&psiStates] (uint64_t stateIndex) { return psiStates.get(stateIndex) ? storm::utility::one<ValueType>() : storm::utility::zero<ValueType>(); },
                            [] (uint64_t) { return storm::utility::zero<ValueType>(); }
                    );
            }
            
            template <typename ValueType>
            std::vector<ValueType> SparseNondeterministicInfiniteHorizonHelper<ValueType>::computeLongRunAverageRewards(Environment const& env, storm::models::sparse::StandardRewardModel<ValueType> const& rewardModel) {
                std::function<ValueType(uint64_t stateIndex)> stateRewardsGetter;
                if (rewardModel.hasStateRewards()) {
                    stateRewardsGetter = [&rewardModel] (uint64_t stateIndex) { return rewardModel.getStateReward(stateIndex); };
                } else {
                    stateRewardsGetter = [] (uint64_t) { return storm::utility::zero<ValueType>(); };
                }
                std::function<ValueType(uint64_t globalChoiceIndex)> actionRewardsGetter;
                if (rewardModel.hasStateActionRewards() || rewardModel.hasTransitionRewards()) {
                    if (rewardModel.hasTransitionRewards()) {
                        actionRewardsGetter = [&] (uint64_t globalChoiceIndex) { return rewardModel.getStateActionAndTransitionReward(globalChoiceIndex, this->_transitionMatrix); };
                    } else {
                        actionRewardsGetter = [&] (uint64_t globalChoiceIndex) { return rewardModel.getStateActionReward(globalChoiceIndex); };
                    }
                } else {
                    stateRewardsGetter = [] (uint64_t) { return storm::utility::zero<ValueType>(); };
                }
                
                return computeLongRunAverageValues(env, stateRewardsGetter, actionRewardsGetter);
            }
            
            template <typename ValueType>
            std::vector<ValueType> SparseNondeterministicInfiniteHorizonHelper<ValueType>::computeLongRunAverageValues(Environment const& env, std::vector<ValueType> const* stateValues, std::vector<ValueType> const* actionValues) {
                std::function<ValueType(uint64_t stateIndex)> stateValuesGetter;
                if (stateValues) {
                    stateValuesGetter = [&stateValues] (uint64_t stateIndex) { return (*stateValues)[stateIndex]; };
                } else {
                    stateValuesGetter = [] (uint64_t) { return storm::utility::zero<ValueType>(); };
                }
                std::function<ValueType(uint64_t actionIndex)> actionValuesGetter;
                if (actionValues) {
                    actionValuesGetter = [&actionValues] (uint64_t globalChoiceIndex) { return (*actionValues)[globalChoiceIndex]; };
                } else {
                    actionValuesGetter = [] (uint64_t) { return storm::utility::zero<ValueType>(); };
                }
                
                return computeLongRunAverageValues(env, stateValuesGetter, actionValuesGetter);

            }
            
            template <typename ValueType>
            std::vector<ValueType> SparseNondeterministicInfiniteHorizonHelper<ValueType>::computeLongRunAverageValues(Environment const& env, std::function<ValueType(uint64_t stateIndex)> const& stateRewardsGetter,  std::function<ValueType(uint64_t globalChoiceIndex)> const& actionRewardsGetter) {
                
                 // Prepare an environment for the underlying solvers
                auto underlyingSolverEnvironment = env;
                if (env.solver().isForceSoundness()) {
                    // For sound computations, the error in the MECS plus the error in the remaining system should not exceed the user defined precsion.
                    underlyingSolverEnvironment.solver().minMax().setPrecision(env.solver().lra().getPrecision() / storm::utility::convertNumber<storm::RationalNumber>(2));
                    underlyingSolverEnvironment.solver().minMax().setRelativeTerminationCriterion(env.solver().lra().getRelativeTerminationCriterion());
                    underlyingSolverEnvironment.solver().lra().setPrecision(env.solver().lra().getPrecision() / storm::utility::convertNumber<storm::RationalNumber>(2));
                }
                
                // If requested, allocate memory for the choices made
                if (isProduceSchedulerSet()) {
                    if (!_producedOptimalChoices.is_initialized()) {
                        _producedOptimalChoices.emplace();
                    }
                    _producedOptimalChoices->resize(_transitionMatrix.getRowGroupCount());
                }
                
                // Start by decomposing the Model into its MECs.
                storm::storage::MaximalEndComponentDecomposition<ValueType> mecDecomposition(_transitionMatrix, _backwardTransitions);

                // Compute the long-run average for all end components in isolation.
                std::vector<ValueType> mecLraValues;
                mecLraValues.reserve(mecDecomposition.size());
                for (auto const& mec : mecDecomposition) {
                    mecLraValues.push_back(computeLraForMec(underlyingSolverEnvironment, stateRewardsGetter, actionRewardsGetter, mec));
                }
                
                // Solve the resulting SSP where end components are collapsed into single auxiliary states
                return buildAndSolveSsp(underlyingSolverEnvironment, mecDecomposition, mecLraValues);
            }
            
            
            template <typename ValueType>
            void SparseNondeterministicInfiniteHorizonHelper<ValueType>::setProduceScheduler(bool value) {
                _produceScheduler = value;
            }
            
            template <typename ValueType>
            bool SparseNondeterministicInfiniteHorizonHelper<ValueType>::isProduceSchedulerSet() const {
                return _produceScheduler;
            }
            
            template <typename ValueType>
            std::vector<uint64_t> const& SparseNondeterministicInfiniteHorizonHelper<ValueType>::getProducedOptimalChoices() const {
                STORM_LOG_ASSERT(isProduceSchedulerSet(), "Trying to get the produced optimal choices although no scheduler was requested.");
                STORM_LOG_ASSERT(_producedOptimalChoices.is_initialized(), "Trying to get the produced optimal choices but none were available. Was there a computation call before?");
                return _producedOptimalChoices.get();
            }
            
            template <typename ValueType>
            std::vector<uint64_t>& SparseNondeterministicInfiniteHorizonHelper<ValueType>::getProducedOptimalChoices() {
                STORM_LOG_ASSERT(isProduceSchedulerSet(), "Trying to get the produced optimal choices although no scheduler was requested.");
                STORM_LOG_ASSERT(_producedOptimalChoices.is_initialized(), "Trying to get the produced optimal choices but none were available. Was there a computation call before?");
                return _producedOptimalChoices.get();
            }
            
            template <typename ValueType>
            storm::storage::Scheduler<ValueType> SparseNondeterministicInfiniteHorizonHelper<ValueType>::extractScheduler() const {
                auto const& optimalChoices = getProducedOptimalChoices();
                storm::storage::Scheduler<ValueType> scheduler(optimalChoices.size());
                for (uint64_t state = 0; state < optimalChoices.size(); ++state) {
                        scheduler.setChoice(optimalChoices[state], state);
                }
                return scheduler;
            }
            
            template <typename ValueType>
            bool SparseNondeterministicInfiniteHorizonHelper<ValueType>::isContinuousTime() const {
                STORM_LOG_ASSERT((_markovianStates == nullptr) == (_exitRates == nullptr), "Inconsistent information given: Have Markovian states but no exit rates (or vice versa)." );
                return _markovianStates != nullptr;
            }
    
            template <typename ValueType>
            ValueType SparseNondeterministicInfiniteHorizonHelper<ValueType>::computeLraForMec(Environment const& env, std::function<ValueType(uint64_t stateIndex)> const& stateRewardsGetter,  std::function<ValueType(uint64_t globalChoiceIndex)> const& actionRewardsGetter, storm::storage::MaximalEndComponent const& mec) {
                
                // If the mec only consists of a single state, we compute the LRA value directly
                if (mec.size() == 1) {
                    uint64_t state = mec.begin()->first;
                    auto choiceIt = mec.begin()->second.begin();
                    if (isContinuousTime()) {
                        // Singleton MECs have to consist of a Markovian state because of the non-Zenoness assumption. Then, there is just one possible choice.
                        STORM_LOG_THROW(_markovianStates->get(state), storm::exceptions::InvalidOperationException, "Markov Automaton has Zeno behavior. Computation of Long Run Average values not supported.");
                        STORM_LOG_ASSERT(mec.begin()->second.size() == 1, "Markovian state has Nondeterministic behavior.");
                        if (isProduceSchedulerSet()) {
                            _producedOptimalChoices.get()[state] = 0;
                        }
                        return stateRewardsGetter(state) + (*_exitRates)[state] * actionRewardsGetter(*choiceIt);
                    } else {
                        // Find the choice with the highest/lowest reward
                        ValueType bestValue = actionRewardsGetter(*choiceIt);
                        uint64_t bestChoice = *choiceIt;
                        for (++choiceIt; choiceIt != mec.begin()->second.end(); ++choiceIt) {
                            ValueType currentValue = actionRewardsGetter(*choiceIt);
                            if ((this->minimize() &&  currentValue < bestValue) || (this->maximize() && currentValue > bestValue)) {
                                bestValue = std::move(currentValue);
                                bestChoice = *choiceIt;
                            }
                        }
                        if (isProduceSchedulerSet()) {
                            _producedOptimalChoices.get()[state] = bestChoice - _transitionMatrix.getRowGroupIndices()[state];
                        }
                        return bestValue + stateRewardsGetter(state);
                    }
                }
                
                // Solve nontrivial MEC with the method specified in the settings
                storm::solver::LraMethod method = env.solver().lra().getNondetLraMethod();
                if ((storm::NumberTraits<ValueType>::IsExact || env.solver().isForceExact()) && env.solver().lra().isNondetLraMethodSetFromDefault() && method != storm::solver::LraMethod::LinearProgramming) {
                    STORM_LOG_INFO("Selecting 'LP' as the solution technique for long-run properties to guarantee exact results. If you want to override this, please explicitly specify a different LRA method.");
                    method = storm::solver::LraMethod::LinearProgramming;
                } else if (env.solver().isForceSoundness() && env.solver().lra().isNondetLraMethodSetFromDefault() && method != storm::solver::LraMethod::ValueIteration) {
                    STORM_LOG_INFO("Selecting 'VI' as the solution technique for long-run properties to guarantee sound results. If you want to override this, please explicitly specify a different LRA method.");
                    method = storm::solver::LraMethod::ValueIteration;
                }
                STORM_LOG_ERROR_COND(!isProduceSchedulerSet() || method == storm::solver::LraMethod::ValueIteration, "Scheduler generation not supported for the chosen LRA method. Try value-iteration.");
                if (method == storm::solver::LraMethod::LinearProgramming) {
                    return computeLraForMecLp(env, stateRewardsGetter, actionRewardsGetter, mec);
                } else if (method == storm::solver::LraMethod::ValueIteration) {
                    return computeLraForMecVi(env, stateRewardsGetter, actionRewardsGetter, mec);
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "Unsupported technique.");
                }
            }
    
            /*!
             * Abstract helper class that performs a single iteration of the value iteration method
             */
            template <typename ValueType>
            class LraViHelper {
            public:
                LraViHelper(storm::storage::MaximalEndComponent const& mec, storm::storage::SparseMatrix<ValueType> const& transitionMatrix) : _mec(mec), _transitionMatrix(transitionMatrix) {
                    // Intentionally left empty
                }
                virtual ~LraViHelper() = default;

                /*!
                 * performs a single iteration step.
                 * If a choices vector is given, the optimal choices will be inserted at the appropriate states.
                 * Note that these choices will be inserted w.r.t. the original model states/choices, i.e. the size of the vector should match the state-count of the input model
                 * @return the current estimate of the LRA value
                 */
                virtual void iterate(Environment const& env, storm::solver::OptimizationDirection const& dir, std::vector<uint64_t>* choices = nullptr) = 0;
                
                struct ConvergenceCheckResult {
                    bool isPrecisionAchieved;
                    ValueType currentValue;
                };
                
                /*!
                 * Checks whether the curently computed value achieves the desired precision
                 */
                virtual ConvergenceCheckResult checkConvergence(bool relative, ValueType precision) = 0;
                
                /*!
                 * Must be called between two calls of iterate.
                 */
                virtual void prepareNextIteration(Environment const& env) = 0;
                
            protected:
                
                /*!
                 *
                 * @param xPrevious the 'old' values
                 * @param xCurrent the 'new' values
                 * @param threshold the threshold
                 * @param relative whether the relative difference should be considered
                 * @return The first component is true if the (relative) difference between the maximal and the minimal entry-wise change of the two value vectors is below or equal to the provided threshold.
                 *          In this case, the second component is the average of the maximal and the minimal change.
                 *          If the threshold is exceeded, the computation is aborted early and the second component is only an approximation of the averages.
                 */
                std::pair<bool, ValueType> checkMinMaxDiffBelowThreshold(std::vector<ValueType> const& xPrevious, std::vector<ValueType> const& xCurrent, ValueType const& threshold, bool relative) const {
                    STORM_LOG_ASSERT(xPrevious.size() == xCurrent.size(), "Unexpected Dimension Mismatch");
                    STORM_LOG_ASSERT(threshold > storm::utility::zero<ValueType>(), "Did not expect a non-positive threshold.");
                    auto x1It = xPrevious.begin();
                    auto x1Ite = xPrevious.end();
                    auto x2It = xCurrent.begin();
                    ValueType maxDiff = (*x2It - *x1It);
                    ValueType minDiff = maxDiff;
                    bool result = true;
                    // The difference between maxDiff and minDiff is zero at this point. Thus, it doesn't make sense to check the threshold now.
                    for (++x1It, ++x2It; x1It != x1Ite; ++x1It, ++x2It) {
                        ValueType diff = (*x2It - *x1It);
                        // Potentially update maxDiff or minDiff
                        bool skipCheck = false;
                        if (maxDiff < diff) {
                            maxDiff = diff;
                        } else if (minDiff > diff) {
                            minDiff = diff;
                        } else {
                            skipCheck = true;
                        }
                        // Check convergence
                        if (!skipCheck && (maxDiff - minDiff) > (relative ? (threshold * minDiff) : threshold)) {
                            result = false;
                            break;
                        }
                    }
                    ValueType avgDiff = (maxDiff + minDiff) / (storm::utility::convertNumber<ValueType>(2.0));
                    return {result, avgDiff};
                }
                
                storm::storage::MaximalEndComponent const& _mec;
                storm::storage::SparseMatrix<ValueType> const& _transitionMatrix;
            };
    
            /*!
             * Abstract helper class that performs a single iteration of the value iteration method for MDP
             * @see Ashok et al.: Value Iteration for Long-Run Average Reward in Markov Decision Processes (CAV'17), https://doi.org/10.1007/978-3-319-63387-9_10
             */
            template <typename ValueType>
            class MdpLraViHelper : public LraViHelper<ValueType> {
            public:
                
                MdpLraViHelper(storm::storage::MaximalEndComponent const& mec, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::function<ValueType(uint64_t stateIndex)> const& stateRewardsGetter,  std::function<ValueType(uint64_t globalChoiceIndex)> const& actionRewardsGetter, ValueType const& aperiodicFactor) : LraViHelper<ValueType>(mec, transitionMatrix), _x1(mec.size(), storm::utility::zero<ValueType>()), _x2(_x1), _x1IsCurrent(true) {
                    
                    // We add a selfloop to each state (which is necessary for convergence)
                    // Very roughly, this selfloop avoids that the values can flip around like this: [1, 0] -> [0, 1] -> [1, 0] -> ...
                    ValueType selfLoopProb = aperiodicFactor;
                    // Introducing the selfloop also requires the rewards to be scaled by the following factor.
                    _scalingFactor = storm::utility::one<ValueType>() - selfLoopProb;
                    
                    uint64_t numMecStates = this->_mec.size();
                    boost::container::flat_map<uint64_t, uint64_t> toSubModelStateMapping;
                    toSubModelStateMapping.reserve(numMecStates);
                    uint64_t currState = 0;
                    uint64_t numMecChoices = 0;
                    for (auto const& stateChoices : this->_mec) {
                        toSubModelStateMapping.emplace(stateChoices.first, currState);
                        ++currState;
                        numMecChoices += stateChoices.second.size();
                    }
                    assert(currState == numMecStates);
                    
                    // Get a transition matrix that only considers the states and choices within the MEC
                    storm::storage::SparseMatrixBuilder<ValueType> mecTransitionBuilder(numMecChoices, numMecStates, 0, true, true, numMecStates);
                    _choiceValues.reserve(numMecChoices);
                    uint64_t currRow = 0;
                    for (auto const& stateChoices : this->_mec) {
                        auto const& mecState = stateChoices.first;
                        auto const& mecChoices = stateChoices.second;
                        mecTransitionBuilder.newRowGroup(currRow);
                        for (auto const& choice : mecChoices) {
                            bool insertedDiagElement = false;
                            for (auto const& entry : this->_transitionMatrix.getRow(choice)) {
                                uint64_t column = toSubModelStateMapping[entry.getColumn()];
                                if (!insertedDiagElement && entry.getColumn() > mecState) {
                                    mecTransitionBuilder.addNextValue(currRow, toSubModelStateMapping[mecState], selfLoopProb);
                                    insertedDiagElement = true;
                                }
                                if (!insertedDiagElement && entry.getColumn() == mecState) {
                                    mecTransitionBuilder.addNextValue(currRow, column, selfLoopProb + _scalingFactor * entry.getValue());
                                    insertedDiagElement = true;
                                } else {
                                    mecTransitionBuilder.addNextValue(currRow, column,  _scalingFactor * entry.getValue());
                                }
                            }
                            if (!insertedDiagElement) {
                                mecTransitionBuilder.addNextValue(currRow, toSubModelStateMapping[mecState], selfLoopProb);
                            }
                            
                            // Compute the rewards obtained for this choice
                            _choiceValues.push_back(_scalingFactor * (stateRewardsGetter(mecState) + actionRewardsGetter(choice)));
                            
                            ++currRow;
                        }
                    }
                    
                    _mecTransitions = mecTransitionBuilder.build();
                    
                    STORM_LOG_ASSERT(_mecTransitions.isProbabilistic(), "The MEC-Matrix is not probabilistic.");
                    STORM_LOG_ASSERT(_mecTransitions.getRowGroupCount() == _x1.size(), "Unexpected size mismatch for created matrix.");
                    STORM_LOG_ASSERT(_x1.size() == _x2.size(), "Unexpected size mismatch for created matrix.");
                }
                
                virtual void iterate(Environment const& env, storm::solver::OptimizationDirection const& dir, std::vector<uint64_t>* choices = nullptr) override {
                    // Initialize a multipler if it does not exist, yet
                    if (!_multiplier) {
                        _multiplier = storm::solver::MultiplierFactory<ValueType>().create(env, _mecTransitions);
                    }
                    
                    if (choices == nullptr) {
                        // Perform a simple matrix-vector multiplication
                        _multiplier->multiplyAndReduce(env, dir, xCurrent(), &_choiceValues, xPrevious());
                    } else {
                        // Perform a simple matrix-vector multiplication but also keep track of the choices within the _mecTransitions
                        std::vector<uint64_t> mecChoices(_mecTransitions.getRowGroupCount());
                        _multiplier->multiplyAndReduce(env, dir, xCurrent(), &_choiceValues, xPrevious(), &mecChoices);
                        // Transform the local choices (within this mec) to global indices
                        uint64_t mecState = 0;
                        for (auto const& stateChoices : this->_mec) {
                            uint64_t mecChoice = mecChoices[mecState];
                            STORM_LOG_ASSERT(mecChoice < stateChoices.second.size(), "The selected choice does not seem to exist.");
                            uint64_t globalChoiceIndex = *(stateChoices.second.begin() + mecChoice);
                            (*choices)[stateChoices.first] = globalChoiceIndex - this->_transitionMatrix.getRowGroupIndices()[stateChoices.first];
                            ++mecState;
                        }
                    }
                    
                    // Swap current and previous x vectors
                    _x1IsCurrent = !_x1IsCurrent;
                    
                }
                
                virtual typename LraViHelper<ValueType>::ConvergenceCheckResult checkConvergence(bool relative, ValueType precision) override {
                    typename LraViHelper<ValueType>::ConvergenceCheckResult res;
                    std::tie(res.isPrecisionAchieved, res.currentValue) = this->checkMinMaxDiffBelowThreshold(xPrevious(), xCurrent(), precision, relative);
                    res.currentValue /= _scalingFactor; // "Undo" the scaling of the rewards
                    return res;
                }
                
                virtual void prepareNextIteration(Environment const&) override {
                    // To avoid large (and numerically unstable) x-values, we substract a reference value.
                    ValueType referenceValue = xCurrent().front();
                    storm::utility::vector::applyPointwise<ValueType, ValueType>(xCurrent(), xCurrent(), [&referenceValue] (ValueType const& x_i) -> ValueType { return x_i - referenceValue; });
                }
                
            private:
                
                std::vector<ValueType>& xCurrent() {
                    return _x1IsCurrent ? _x1 : _x2;
                }
                
                std::vector<ValueType>& xPrevious() {
                    return _x1IsCurrent ? _x2 : _x1;
                }
                
                storm::storage::SparseMatrix<ValueType> _mecTransitions;
                std::vector<ValueType> _x1, _x2, _choiceValues;
                bool _x1IsCurrent;
                std::unique_ptr<storm::solver::Multiplier<ValueType>> _multiplier;
                ValueType _scalingFactor;
            };
            
            /*!
             * Abstract helper class that performs a single iteration of the value iteration method for MA
             * @see Butkova, Wimmer, Hermanns: Long-Run Rewards for Markov Automata (TACAS'17), https://doi.org/10.1007/978-3-662-54580-5_11
             */
            template <typename ValueType>
            class MaLraViHelper : public LraViHelper<ValueType> {
            public:
                
                MaLraViHelper(storm::storage::MaximalEndComponent const& mec, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::BitVector const& markovianStates, std::vector<ValueType> const& exitRates, std::function<ValueType(uint64_t stateIndex)> const& stateRewardsGetter,  std::function<ValueType(uint64_t globalChoiceIndex)> const& actionRewardsGetter, ValueType const& aperiodicFactor) : LraViHelper<ValueType>(mec, transitionMatrix), _markovianStates(markovianStates), _Msx1IsCurrent(false) {
                    
                    // Run through the Mec and collect some data:
                    // We consider two submodels, one consisting of the Markovian MEC states and one consisting of the probabilistic MEC states.
                    // For this, we create a state index map that point from state indices of the input model to indices of the corresponding submodel of that state.
                    boost::container::flat_map<uint64_t, uint64_t> toSubModelStateMapping;
                    // We also obtain state and choices counts of the two submodels
                    uint64_t numPsSubModelStates(0), numPsSubModelChoices(0);
                    uint64_t numMsSubModelStates(0); // The number of choices coincide
                    // We will need to uniformize the Markovian MEC states by introducing a selfloop.
                    // For this, we need to find a uniformization rate which will be a little higher (given by aperiodicFactor) than the maximum rate occurring in the MEC.
                    _uniformizationRate = storm::utility::zero<ValueType>();
                    // Now run over the MEC and collect the required data.
                    for (auto const& stateChoices : this->_mec) {
                        uint64_t const& mecState = stateChoices.first;
                        if (_markovianStates.get(mecState)) {
                            toSubModelStateMapping.emplace(mecState, numMsSubModelStates);
                            ++numMsSubModelStates;
                            STORM_LOG_ASSERT(stateChoices.second.size() == 1, "Markovian state has multiple MEC choices.");
                            _uniformizationRate = std::max(_uniformizationRate, exitRates[mecState]);
                        } else {
                            toSubModelStateMapping.emplace(mecState, numPsSubModelStates);
                            ++numPsSubModelStates;
                            numPsSubModelChoices += stateChoices.second.size();
                        }
                    }
                    assert(numPsSubModelStates + numMsSubModelStates == mec.size());
                    STORM_LOG_THROW(numMsSubModelStates > 0, storm::exceptions::InvalidOperationException, "Markov Automaton has Zeno behavior. Computation of Long Run Average values not supported.");

                    _hasProbabilisticStates = numPsSubModelStates > 0;
                    
                    // We make sure that every Markovian state gets a selfloop to make the model aperiodic
                    _uniformizationRate *= storm::utility::one<ValueType>() + aperiodicFactor;

                    // Now build the Markovian and the Probabilistic submodels.
                    // In addition, we also need the transitions between the two.
                    storm::storage::SparseMatrixBuilder<ValueType> msTransitionsBuilder(numMsSubModelStates, numMsSubModelStates);
                    _MsChoiceValues.reserve(numMsSubModelStates);
                    storm::storage::SparseMatrixBuilder<ValueType> msToPsTransitionsBuilder, psTransitionsBuilder, psToMsTransitionsBuilder;
                    if (_hasProbabilisticStates) {
                        msToPsTransitionsBuilder = storm::storage::SparseMatrixBuilder<ValueType>(numMsSubModelStates, numPsSubModelStates);
                        psTransitionsBuilder = storm::storage::SparseMatrixBuilder<ValueType>(numPsSubModelChoices, numPsSubModelStates, 0, true, true, numPsSubModelStates);
                        psToMsTransitionsBuilder = storm::storage::SparseMatrixBuilder<ValueType>(numPsSubModelChoices, numMsSubModelStates, 0, true, true, numPsSubModelStates);
                        _PsChoiceValues.reserve(numPsSubModelChoices);
                    }
                    uint64_t currMsRow = 0;
                    uint64_t currPsRow = 0;
                    for (auto const& stateChoices : this->_mec) {
                        uint64_t const& mecState = stateChoices.first;
                        auto const& mecChoices = stateChoices.second;
                        if (!_hasProbabilisticStates || _markovianStates.get(mecState)) {
                            // The currently processed state is Markovian.
                            // We need to uniformize!
                            ValueType uniformizationFactor = exitRates[mecState] / _uniformizationRate;
                            ValueType selfLoopProb = storm::utility::one<ValueType>() - uniformizationFactor;
                            STORM_LOG_ASSERT(mecChoices.size() == 1, "Unexpected number of choices at Markovian state.");
                            for (auto const& mecChoice : mecChoices) {
                                bool insertedDiagElement = false;
                                for (auto const& entry : this->_transitionMatrix.getRow(mecChoice)) {
                                    uint64_t subModelColumn = toSubModelStateMapping[entry.getColumn()];
                                    if (!_hasProbabilisticStates || _markovianStates.get(entry.getColumn())) {
                                        // We have a transition from a Markovian state to a Markovian state
                                        STORM_LOG_ASSERT(subModelColumn < numMsSubModelStates, "Invalid state for Markovian submodel");
                                        if (!insertedDiagElement && subModelColumn > currMsRow) {
                                            // We passed the diagonal entry, so add it now before moving on to the next entry
                                            msTransitionsBuilder.addNextValue(currMsRow, currMsRow, selfLoopProb);
                                            insertedDiagElement = true;
                                        }
                                        if (!insertedDiagElement && subModelColumn == currMsRow) {
                                            // The current entry is the diagonal (selfloop) entry
                                            msTransitionsBuilder.addNextValue(currMsRow, subModelColumn, selfLoopProb + uniformizationFactor * entry.getValue());
                                            insertedDiagElement = true;
                                        } else {
                                            // The diagonal element either has been inserted already or still lies in front
                                            msTransitionsBuilder.addNextValue(currMsRow, subModelColumn,  uniformizationFactor * entry.getValue());
                                        }
                                    } else {
                                        // We have a transition from a Markovian to a probabilistic state
                                        STORM_LOG_ASSERT(subModelColumn < numPsSubModelStates, "Invalid state for probabilistic submodel");
                                        msToPsTransitionsBuilder.addNextValue(currMsRow, subModelColumn, uniformizationFactor * entry.getValue());
                                    }
                                }
                                // If the diagonal entry for the MS matrix still has not been set, we do that now
                                if (!insertedDiagElement) {
                                    msTransitionsBuilder.addNextValue(currMsRow, currMsRow, selfLoopProb);
                                }
                                // Compute the rewards obtained for this choice.
                                _MsChoiceValues.push_back(stateRewardsGetter(mecState) / _uniformizationRate + actionRewardsGetter(mecChoice) * exitRates[mecState] / _uniformizationRate);
                                ++currMsRow;
                            }
                        } else {
                            // The currently processed state is probabilistic
                            psTransitionsBuilder.newRowGroup(currPsRow);
                            psToMsTransitionsBuilder.newRowGroup(currPsRow);
                            for (auto const& mecChoice : mecChoices) {
                                for (auto const& entry : this->_transitionMatrix.getRow(mecChoice)) {
                                    uint64_t subModelColumn = toSubModelStateMapping[entry.getColumn()];
                                    if (_markovianStates.get(entry.getColumn())) {
                                        // We have a transition from a probabilistic state to a Markovian state
                                        STORM_LOG_ASSERT(subModelColumn < numMsSubModelStates, "Invalid state for Markovian submodel");
                                        psToMsTransitionsBuilder.addNextValue(currPsRow, subModelColumn, entry.getValue());
                                    } else {
                                        // We have a transition from a probabilistic to a probabilistic state
                                        STORM_LOG_ASSERT(subModelColumn < numPsSubModelStates, "Invalid state for probabilistic submodel");
                                        psTransitionsBuilder.addNextValue(currPsRow, subModelColumn, entry.getValue());
                                    }
                                }
                                // Compute the rewards obtained for this choice.
                                // State rewards do not count here since no time passes in probabilistic states.
                                _PsChoiceValues.push_back(actionRewardsGetter(mecChoice));
                                ++currPsRow;
                            }
                        }
                    }
                    _MsTransitions = msTransitionsBuilder.build();
                    if (_hasProbabilisticStates) {
                        _MsToPsTransitions = msToPsTransitionsBuilder.build();
                        _PsTransitions = psTransitionsBuilder.build();
                        _PsToMsTransitions = psToMsTransitionsBuilder.build();
                    }
                }
                
                void initializeIterations(Environment const& env, storm::solver::OptimizationDirection const& dir) {
                    _Msx1.resize(_MsTransitions.getRowGroupCount(), storm::utility::zero<ValueType>());
                    _Msx2 = _Msx1;
                    _MsMultiplier = storm::solver::MultiplierFactory<ValueType>().create(env, _MsTransitions);
                    if (_hasProbabilisticStates) {
                        if (_PsTransitions.getNonzeroEntryCount() > 0) {
                            // Set-up a solver for transitions within PS states
                            _PsSolverEnv = env;
                            if (env.solver().isForceSoundness()) {
                                // To get correct results, the inner equation systems are solved exactly.
                                // TODO investigate how an error would propagate
                                _PsSolverEnv.solver().setForceExact(true);
                            }
                            storm::solver::GeneralMinMaxLinearEquationSolverFactory<ValueType> factory;
                            bool isAcyclic = !storm::utility::graph::hasCycle(_PsTransitions);
                            if (isAcyclic) {
                                STORM_LOG_INFO("Probabilistic transitions are acyclic.");
                                _PsSolverEnv.solver().minMax().setMethod(storm::solver::MinMaxMethod::Acyclic);
                            }
                            _PsSolver = factory.create(_PsSolverEnv, _PsTransitions);
                            _PsSolver->setHasUniqueSolution(true); // Assume non-zeno MA
                            _PsSolver->setHasNoEndComponents(true); // assume non-zeno MA
                            _PsSolver->setCachingEnabled(true);
                            _PsSolver->setRequirementsChecked(true);
                            auto req = _PsSolver->getRequirements(_PsSolverEnv, dir);
                            req.clearUniqueSolution();
                            if (isAcyclic) {
                                req.clearAcyclic();
                            }
                            // Computing a priori lower/upper bounds is not particularly easy, as there might be selfloops with high probabilities
                            // Which accumulate a lot of reward. Moreover, the right-hand-side of the equation system changes dynamically.
                            STORM_LOG_THROW(!req.hasEnabledCriticalRequirement(), storm::exceptions::UnmetRequirementException, "The solver requirement " << req.getEnabledRequirementsAsString() << " has not been checked.");
                        }
                        
                        // Set up multipliers for transitions connecting Markovian and probabilistic states
                        _MsToPsMultiplier = storm::solver::MultiplierFactory<ValueType>().create(env, _MsToPsTransitions);
                        _PsToMsMultiplier = storm::solver::MultiplierFactory<ValueType>().create(env, _PsToMsTransitions);

                        // Set-up vectors for storing intermediate results for PS states.
                        _Psx.resize(_PsTransitions.getRowGroupCount(), storm::utility::zero<ValueType>());
                        _Psb = _PsChoiceValues;
                    }
                    
                }
                
                void setInputModelChoices(std::vector<uint64_t>& choices, std::vector<uint64_t> const& localMecChoices, bool setChoiceZeroToMarkovianStates) {
                    // Transform the local choices (within this mec) to choice indices for the input model
                    uint64_t mecState = 0;
                    for (auto const& stateChoices : this->_mec) {
                        if (setChoiceZeroToMarkovianStates && _markovianStates.get(stateChoices.first)) {
                            choices[stateChoices.first] = 0;
                        } else {
                            uint64_t mecChoice = localMecChoices[mecState];
                            STORM_LOG_ASSERT(mecChoice < stateChoices.second.size(), "The selected choice does not seem to exist.");
                            uint64_t globalChoiceIndex = *(stateChoices.second.begin() + mecChoice);
                            choices[stateChoices.first] = globalChoiceIndex - this->_transitionMatrix.getRowGroupIndices()[stateChoices.first];
                            ++mecState;
                        }
                    }
                    STORM_LOG_ASSERT(mecState == localMecChoices.size(), "Did not traverse all mec states.");
                }
                
                virtual void iterate(Environment const& env, storm::solver::OptimizationDirection const& dir, std::vector<uint64_t>* choices = nullptr) override {
                    // Initialize value vectors, multiplers, and solver if this has not been done, yet
                    if (!_MsMultiplier) {
                        initializeIterations(env, dir);
                    }
                    
                    // Compute new x values for the Markovian states
                    // Flip what is current and what is previous
                    _Msx1IsCurrent = !_Msx1IsCurrent;
                    // At this point, xPrevious() points to what has been computed in the previous call of iterate (initially, this is the 0-vector).
                    // The result of this computation will be stored in xCurrent()
                    
                    // Compute the values obtained by a single uniformization step between Markovian states only
                    _MsMultiplier->multiply(env, xPrevious(), &_MsChoiceValues, xCurrent());
                    if (_hasProbabilisticStates) {
                        // Add the values obtained by taking a single uniformization step that leads to a Probabilistic state followed by arbitrarily many probabilistic steps.
                        // First compute the total values when taking arbitrarily many probabilistic transitions (in no time)
                        if (_PsSolver) {
                            // We might need to track the optimal choices.
                            if (choices == nullptr) {
                                _PsSolver->solveEquations(_PsSolverEnv, dir, _Psx, _Psb);
                            } else {
                                _PsSolver->setTrackScheduler();
                                _PsSolver->solveEquations(_PsSolverEnv, dir, _Psx, _Psb);
                                setInputModelChoices(*choices, _PsSolver->getSchedulerChoices(), true);
                            }
                        } else {
                            STORM_LOG_ASSERT(_PsTransitions.getNonzeroEntryCount() == 0, "If no solver was initialized, an empty matrix would have been expected.");
                            if (choices == nullptr) {
                                storm::utility::vector::reduceVectorMinOrMax(dir, _Psb, _Psx, _PsTransitions.getRowGroupIndices());
                            } else {
                                std::vector<uint64_t> psMecChoices(_PsTransitions.getRowGroupCount());
                                storm::utility::vector::reduceVectorMinOrMax(dir, _Psb, _Psx, _PsTransitions.getRowGroupIndices(), &psMecChoices);
                                setInputModelChoices(*choices, _PsSolver->getSchedulerChoices(), true);
                            }
                        }
                        // Now add the (weighted) values of the probabilistic states to the values of the Markovian states.
                        _MsToPsMultiplier->multiply(env, _Psx, &xCurrent(), xCurrent());
                    }
                }
                
                virtual typename LraViHelper<ValueType>::ConvergenceCheckResult checkConvergence(bool relative, ValueType precision) override {
                    typename LraViHelper<ValueType>::ConvergenceCheckResult res;
                    // All values are scaled according to the uniformizationRate.
                    // We need to 'revert' this scaling when computing the absolute precision.
                    // However, for relative precision, the scaling cancels out.
                    ValueType threshold = relative ? precision : ValueType(precision / _uniformizationRate);
                    std::tie(res.isPrecisionAchieved, res.currentValue) = this->checkMinMaxDiffBelowThreshold(xPrevious(), xCurrent(), threshold, relative);
                    res.currentValue *= _uniformizationRate; // "Undo" the scaling of the values
                    return res;
                }
                
                virtual void prepareNextIteration(Environment const& env) override {
                    // To avoid large (and numerically unstable) x-values, we substract a reference value.
                    ValueType referenceValue = xCurrent().front();
                    storm::utility::vector::applyPointwise<ValueType, ValueType>(xCurrent(), xCurrent(), [&referenceValue] (ValueType const& x_i) -> ValueType { return x_i - referenceValue; });
                    if (_hasProbabilisticStates) {
                        // Update the RHS of the equation system for the probabilistic states by taking the new values of Markovian states into account.
                        _PsToMsMultiplier->multiply(env, xCurrent(), &_PsChoiceValues, _Psb);
                    }
                }
                
            private:
                
                std::vector<ValueType>& xCurrent() {
                    return _Msx1IsCurrent ? _Msx1 : _Msx2;
                }
                
                std::vector<ValueType>& xPrevious() {
                    return _Msx1IsCurrent ? _Msx2 : _Msx1;
                }
                
                storm::storage::BitVector const& _markovianStates;
                bool _hasProbabilisticStates;
                ValueType _uniformizationRate;
                storm::storage::SparseMatrix<ValueType> _MsTransitions, _MsToPsTransitions, _PsTransitions, _PsToMsTransitions;
                std::vector<ValueType> _Msx1, _Msx2, _MsChoiceValues;
                bool _Msx1IsCurrent;
                std::vector<ValueType> _Psx, _Psb, _PsChoiceValues;
                std::unique_ptr<storm::solver::Multiplier<ValueType>> _MsMultiplier, _MsToPsMultiplier, _PsToMsMultiplier;
                std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> _PsSolver;
                Environment _PsSolverEnv;
            };
            
            template <typename ValueType>
            ValueType SparseNondeterministicInfiniteHorizonHelper<ValueType>::computeLraForMecVi(Environment const& env, std::function<ValueType(uint64_t stateIndex)> const& stateRewardsGetter,  std::function<ValueType(uint64_t globalChoiceIndex)> const& actionRewardsGetter, storm::storage::MaximalEndComponent const& mec) {

                // Collect some parameters of the computation.
                ValueType aperiodicFactor = storm::utility::convertNumber<ValueType>(env.solver().lra().getAperiodicFactor());
                ValueType precision = storm::utility::convertNumber<ValueType>(env.solver().lra().getPrecision()) / aperiodicFactor;
                bool relative = env.solver().lra().getRelativeTerminationCriterion();
                boost::optional<uint64_t> maxIter;
                if (env.solver().lra().isMaximalIterationCountSet()) {
                    maxIter = env.solver().lra().getMaximalIterationCount();
                }
                auto dir = this->getOptimizationDirection();
                
                // Create an object for the iterations
                std::shared_ptr<LraViHelper<ValueType>> iterationHelper;
                if (isContinuousTime()) {
                    iterationHelper = std::make_shared<MaLraViHelper<ValueType>>(mec, _transitionMatrix, *_markovianStates, *_exitRates, stateRewardsGetter, actionRewardsGetter, aperiodicFactor);
                } else {
                    iterationHelper = std::make_shared<MdpLraViHelper<ValueType>>(mec, _transitionMatrix, stateRewardsGetter, actionRewardsGetter, aperiodicFactor);
                }
                
                // start the iterations
                ValueType result = storm::utility::zero<ValueType>();
                uint64_t iter = 0;
                while (!maxIter.is_initialized() || iter < maxIter.get()) {
                    ++iter;
                    iterationHelper->iterate(env, dir);
                    // Check if we are done
                    auto convergenceCheckResult = iterationHelper->checkConvergence(relative, precision);
                    result = convergenceCheckResult.currentValue;
                    if (convergenceCheckResult.isPrecisionAchieved) {
                        break;
                    }
                    if (storm::utility::resources::isTerminate()) {
                        break;
                    }
                    
                    iterationHelper->prepareNextIteration(env);
                    
                }
                if (maxIter.is_initialized() && iter == maxIter.get()) {
                    STORM_LOG_WARN("LRA computation did not converge within " << iter << " iterations.");
                } else if (storm::utility::resources::isTerminate()) {
                    STORM_LOG_WARN("LRA computation aborted after " << iter << " iterations.");
                } else {
                    STORM_LOG_TRACE("LRA computation converged after " << iter << " iterations.");
                }
                
                if (isProduceSchedulerSet()) {
                    // We will be doing one more iteration step and track scheduler choices this time.
                    iterationHelper->prepareNextIteration(env);
                    iterationHelper->iterate(env, dir, &_producedOptimalChoices.get());
                }
                return result;
            }
            
            template <typename ValueType>
            ValueType SparseNondeterministicInfiniteHorizonHelper<ValueType>::computeLraForMecLp(Environment const& env, std::function<ValueType(uint64_t stateIndex)> const& stateRewardsGetter,  std::function<ValueType(uint64_t globalChoiceIndex)> const& actionRewardsGetter, storm::storage::MaximalEndComponent const& mec) {
                std::shared_ptr<storm::solver::LpSolver<ValueType>> solver = storm::utility::solver::getLpSolver<ValueType>("LRA for MEC");
                solver->setOptimizationDirection(invert(this->getOptimizationDirection()));
                
                // First, we need to create the variables for the problem.
                std::map<uint_fast64_t, storm::expressions::Variable> stateToVariableMap;
                for (auto const& stateChoicesPair : mec) {
                    std::string variableName = "h" + std::to_string(stateChoicesPair.first);
                    stateToVariableMap[stateChoicesPair.first] = solver->addUnboundedContinuousVariable(variableName);
                }
                storm::expressions::Variable lambda = solver->addUnboundedContinuousVariable("L", 1);
                solver->update();
                
                // Now we encode the problem as constraints.
                for (auto const& stateChoicesPair : mec) {
                    uint_fast64_t state = stateChoicesPair.first;
                    
                    // Now, based on the type of the state, create a suitable constraint.
                    for (auto choice : stateChoicesPair.second) {
                        storm::expressions::Expression constraint = -lambda;
                        
                        for (auto const& element : _transitionMatrix.getRow(choice)) {
                            constraint = constraint + stateToVariableMap.at(element.getColumn()) * solver->getConstant(element.getValue());
                        }
                        constraint = solver->getConstant(stateRewardsGetter(state) + actionRewardsGetter(choice)) + constraint;
                        
                        if (this->minimize()) {
                            constraint = stateToVariableMap.at(state) <= constraint;
                        } else {
                            constraint = stateToVariableMap.at(state) >= constraint;
                        }
                        solver->addConstraint("state" + std::to_string(state) + "," + std::to_string(choice), constraint);
                    }
                }
                
                solver->optimize();
                return solver->getContinuousValue(lambda);
            }
            
            /*!
             * Auxiliary function that adds the entries of the Ssp Matrix for a single choice (i.e., row)
             * Transitions that lead to a MEC state will be redirected to a new auxiliary state (there is one aux. state for each MEC).
             * Transitions that don't lead to a MEC state are copied (taking a state index mapping into account).
             */
            template <typename ValueType>
            void addSspMatrixChoice(uint64_t const& inputMatrixChoice, storm::storage::SparseMatrix<ValueType> const& inputTransitionMatrix, std::vector<uint64_t> const& inputToSspStateMap, uint64_t const& numberOfStatesNotInMecs, uint64_t const& currentSspChoice, storm::storage::SparseMatrixBuilder<ValueType>& sspMatrixBuilder) {
            
                // As there could be multiple transitions to the same MEC, we accumulate them in this map before adding them to the matrix builder.
                std::map<uint64_t, ValueType> auxiliaryStateToProbabilityMap;
                
                for (auto const& transition : inputTransitionMatrix.getRow(inputMatrixChoice)) {
                    if (!storm::utility::isZero(transition.getValue())) {
                        auto const& sspTransitionTarget = inputToSspStateMap[transition.getColumn()];
                        // Since the auxiliary MEC states are appended at the end of the matrix, we can use this check to
                        // decide whether the transition leads to a MEC state or not
                        if (sspTransitionTarget < numberOfStatesNotInMecs) {
                            // If the target state is not contained in a MEC, we can copy over the entry.
                            sspMatrixBuilder.addNextValue(currentSspChoice, sspTransitionTarget, transition.getValue());
                        } else {
                            // If the target state is contained in MEC i, we need to add the probability to the corresponding field in the vector
                            // so that we are able to write the cumulative probability to the MEC into the matrix.
                            auto insertionRes = auxiliaryStateToProbabilityMap.emplace(sspTransitionTarget, transition.getValue());
                            if (!insertionRes.second) {
                                // sspTransitionTarget already existed in the map, i.e., there already was a transition to that MEC.
                                // Hence, we add up the probabilities.
                                insertionRes.first->second += transition.getValue();
                            }
                        }
                    }
                }
                
                // Now insert all (cumulative) probability values that target a MEC.
                for (auto const& mecToProbEntry : auxiliaryStateToProbabilityMap) {
                    sspMatrixBuilder.addNextValue(currentSspChoice, mecToProbEntry.first, mecToProbEntry.second);
                }
            }
            
            template <typename ValueType>
            std::vector<ValueType> SparseNondeterministicInfiniteHorizonHelper<ValueType>::buildAndSolveSsp(Environment const& env, storm::storage::MaximalEndComponentDecomposition<ValueType> const& mecDecomposition, std::vector<ValueType> const& mecLraValues) {
                
                // Let's improve readability a bit
                uint64_t numberOfStates = _transitionMatrix.getRowGroupCount();
                auto const& nondeterministicChoiceIndices = _transitionMatrix.getRowGroupIndices();
                
                // For fast transition rewriting, we build a mapping from the input state indices to the state indices of a new transition matrix
                // which redirects all transitions leading to a former MEC state to a new auxiliary state.
                // There will be one auxiliary state for each MEC. These states will be appended to the end of the matrix.
                
                // First gather the states that are part of a MEC
                // and create a mapping from states that lie in a MEC to the corresponding MEC index.
                storm::storage::BitVector statesInMecs(numberOfStates);
                std::vector<uint64_t> inputToSspStateMap(numberOfStates, std::numeric_limits<uint64_t>::max());
                for (uint64_t currentMecIndex = 0; currentMecIndex < mecDecomposition.size(); ++currentMecIndex) {
                    for (auto const& stateChoicesPair : mecDecomposition[currentMecIndex]) {
                        statesInMecs.set(stateChoicesPair.first);
                        inputToSspStateMap[stateChoicesPair.first] = currentMecIndex;
                    }
                }
                // Now take care of the non-mec states. Note that the order of these states will be preserved.
                uint64_t numberOfStatesNotInMecs = 0;
                storm::storage::BitVector statesNotContainedInAnyMec = ~statesInMecs;
                for (auto const& nonMecState : statesNotContainedInAnyMec) {
                    inputToSspStateMap[nonMecState] = numberOfStatesNotInMecs;
                    ++numberOfStatesNotInMecs;
                }
                // Finalize the mapping for the mec states which now still assigns mec states to to their Mec index.
                // To make sure that they point to the auxiliary states (located at the end of the SspMatrix), we need to shift them by the
                // number of states that are not in a mec.
                for (auto const& mecState : statesInMecs) {
                    inputToSspStateMap[mecState] += numberOfStatesNotInMecs;
                }
                
                // For scheduler extraction, we will need to create a mapping between choices at the auxiliary states and the
                // corresponding choices in the original model.
                std::vector<std::pair<uint_fast64_t, uint_fast64_t>> sspMecExitChoicesToOriginalMap;
                
                // The next step is to create the SSP matrix and the right-hand side of the SSP.
                std::vector<ValueType> rhs;
                uint64_t numberOfSspStates = numberOfStatesNotInMecs + mecDecomposition.size();
                typename storm::storage::SparseMatrixBuilder<ValueType> sspMatrixBuilder(0, numberOfSspStates , 0, false, true, numberOfSspStates);
                // If the source state of a transition is not contained in any MEC, we copy its choices (and perform the necessary modifications).
                uint64_t currentSspChoice = 0;
                for (auto const& nonMecState : statesNotContainedInAnyMec) {
                    sspMatrixBuilder.newRowGroup(currentSspChoice);
                    
                    for (uint64_t choice = nondeterministicChoiceIndices[nonMecState]; choice < nondeterministicChoiceIndices[nonMecState + 1]; ++choice, ++currentSspChoice) {
                        rhs.push_back(storm::utility::zero<ValueType>());
                        addSspMatrixChoice(choice, _transitionMatrix, inputToSspStateMap, numberOfStatesNotInMecs, currentSspChoice, sspMatrixBuilder);
                    }
                }
                // Now we construct the choices for the auxiliary states which reflect former MEC states.
                for (uint64_t mecIndex = 0; mecIndex < mecDecomposition.size(); ++mecIndex) {
                    storm::storage::MaximalEndComponent const& mec = mecDecomposition[mecIndex];
                    sspMatrixBuilder.newRowGroup(currentSspChoice);
                    for (auto const& stateChoicesPair : mec) {
                        uint64_t const& mecState = stateChoicesPair.first;
                        auto const& choicesInMec = stateChoicesPair.second;
                        for (uint64_t choice = nondeterministicChoiceIndices[mecState]; choice < nondeterministicChoiceIndices[mecState + 1]; ++choice) {
                            // If the choice is not contained in the MEC itself, we have to add a similar distribution to the auxiliary state.
                            if (choicesInMec.find(choice) == choicesInMec.end()) {
                                rhs.push_back(storm::utility::zero<ValueType>());
                                addSspMatrixChoice(choice, _transitionMatrix, inputToSspStateMap, numberOfStatesNotInMecs, currentSspChoice, sspMatrixBuilder);
                                if (isProduceSchedulerSet()) {
                                    // Later we need to be able to map this choice back to the original input model
                                    sspMecExitChoicesToOriginalMap.emplace_back(mecState, choice - nondeterministicChoiceIndices[mecState]);
                                }
                                ++currentSspChoice;
                            }
                        }
                    }
                    // For each auxiliary state, there is the option to achieve the reward value of the LRA associated with the MEC.
                    rhs.push_back(mecLraValues[mecIndex]);
                    if (isProduceSchedulerSet()) {
                        // Insert some invalid values so we can later detect that this choice is not an exit choice
                        sspMecExitChoicesToOriginalMap.emplace_back(std::numeric_limits<uint_fast64_t>::max(), std::numeric_limits<uint_fast64_t>::max());
                    }
                    ++currentSspChoice;
                }
                storm::storage::SparseMatrix<ValueType> sspMatrix = sspMatrixBuilder.build(currentSspChoice, numberOfSspStates, numberOfSspStates);
                
                // Set-up a solver
                storm::solver::GeneralMinMaxLinearEquationSolverFactory<ValueType> minMaxLinearEquationSolverFactory;
                storm::solver::MinMaxLinearEquationSolverRequirements requirements = minMaxLinearEquationSolverFactory.getRequirements(env, true, true, this->getOptimizationDirection(), false, this->isProduceSchedulerSet());
                requirements.clearBounds();
                STORM_LOG_THROW(!requirements.hasEnabledCriticalRequirement(), storm::exceptions::UnmetRequirementException, "Solver requirements " + requirements.getEnabledRequirementsAsString() + " not checked.");
                std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> solver = minMaxLinearEquationSolverFactory.create(env, sspMatrix);
                solver->setHasUniqueSolution();
                solver->setHasNoEndComponents();
                solver->setTrackScheduler(isProduceSchedulerSet());
                auto lowerUpperBounds = std::minmax_element(mecLraValues.begin(), mecLraValues.end());
                solver->setLowerBound(*lowerUpperBounds.first);
                solver->setUpperBound(*lowerUpperBounds.second);
                solver->setRequirementsChecked();
                
                // Solve the equation system
                std::vector<ValueType> x(numberOfSspStates);
                solver->solveEquations(env, this->getOptimizationDirection(), x, rhs);

                // Prepare scheduler (if requested)
                if (isProduceSchedulerSet() && solver->hasScheduler()) {
                    // Translate result for ssp matrix to original model
                    auto const& sspChoices = solver->getSchedulerChoices();
                    // We first take care of non-mec states
                    storm::utility::vector::setVectorValues(_producedOptimalChoices.get(), statesNotContainedInAnyMec, sspChoices);
                    // Secondly, we consider MEC states. There are 3 cases for each MEC state:
                    // 1. The SSP choices encode that we want to stay in the MEC
                    // 2. The SSP choices encode that we want to leave the MEC and
                    //      a) we take an exit (non-MEC) choice at the given state
                    //      b) we have to take a MEC choice at the given state in a way that eventually an exit state of the MEC is reached
                    uint64_t exitChoiceOffset = sspMatrix.getRowGroupIndices()[numberOfStatesNotInMecs];
                    for (auto const& mec : mecDecomposition) {
                        // Get the sspState of this MEC (using one representative mec state)
                        auto const& sspState = inputToSspStateMap[mec.begin()->first];
                        uint64_t sspChoiceIndex = sspMatrix.getRowGroupIndices()[sspState] + sspChoices[sspState];
                        // Obtain the state and choice of the original model to which the selected choice corresponds.
                        auto const& originalStateChoice = sspMecExitChoicesToOriginalMap[sspChoiceIndex - exitChoiceOffset];
                        // Check if we are in Case 1 or 2
                        if (originalStateChoice.first == std::numeric_limits<uint_fast64_t>::max()) {
                            // The optimal choice is to stay in this mec (Case 1)
                            // In this case, no further operations are necessary. The scheduler has already been set to the optimal choices during the call of computeLraForMec.
                            STORM_LOG_ASSERT(sspMatrix.getRow(sspState, sspChoices[sspState]).getNumberOfEntries() == 0, "Expected empty row at choice that stays in MEC.");
                        } else {
                            // The best choice is to leave this MEC via the selected state and choice. (Case 2)
                            // Set the exit choice (Case 2.a)
                            _producedOptimalChoices.get()[originalStateChoice.first] = originalStateChoice.second;
                            // The remaining states in this MEC need to reach the state with the exit choice with probability 1. (Case 2.b)
                            // Perform a backwards search from the exit state, only using MEC choices
                            // We start by setting an invalid choice to all remaining mec states (so that we can easily detect them as unprocessed)
                            for (auto const& stateActions : mec) {
                                if (stateActions.first != originalStateChoice.first) {
                                    _producedOptimalChoices.get()[stateActions.first] = std::numeric_limits<uint64_t>::max();
                                }
                            }
                            // Now start a backwards DFS
                            std::vector<uint64_t> stack = {originalStateChoice.first};
                            while (!stack.empty()) {
                                uint64_t currentState = stack.back();
                                stack.pop_back();
                                for (auto const& backwardsTransition : _backwardTransitions.getRowGroup(currentState)) {
                                    uint64_t predecessorState = backwardsTransition.getColumn();
                                    if (mec.containsState(predecessorState)) {
                                        auto& selectedPredChoice = _producedOptimalChoices.get()[predecessorState];
                                        if (selectedPredChoice == std::numeric_limits<uint64_t>::max()) {
                                            // We don't already have a choice for this predecessor.
                                            // We now need to check whether there is a *MEC* choice leading to currentState
                                            for (auto const& predChoice : mec.getChoicesForState(predecessorState)) {
                                                for (auto const& forwardTransition : _transitionMatrix.getRow(predChoice)) {
                                                    if (forwardTransition.getColumn() == currentState && !storm::utility::isZero(forwardTransition.getValue())) {
                                                        // Playing this choice (infinitely often) will lead to current state (infinitely often)!
                                                        selectedPredChoice = predChoice - nondeterministicChoiceIndices[predecessorState];
                                                        stack.push_back(predecessorState);
                                                        break;
                                                    }
                                                }
                                                if (selectedPredChoice != std::numeric_limits<uint64_t>::max()) {
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                } else {
                    STORM_LOG_ERROR_COND(!isProduceSchedulerSet(), "Requested to produce a scheduler, but no scheduler was generated.");
                }
                
                // Prepare result vector.
                // For efficiency reasons, we re-use the memory of our rhs for this!
                std::vector<ValueType> result = std::move(rhs);
                result.resize(numberOfStates);
                result.shrink_to_fit();
                storm::utility::vector::selectVectorValues(result, inputToSspStateMap, x);
                return result;
            }
            
            template class SparseNondeterministicInfiniteHorizonHelper<double>;
            template class SparseNondeterministicInfiniteHorizonHelper<storm::RationalNumber>;
        }
    }
}