#include "src/modelchecker/multiobjective/helper/SparseMaMultiObjectiveWeightVectorChecker.h"

#include <cmath>

#include "src/adapters/CarlAdapter.h"
#include "src/models/sparse/MarkovAutomaton.h"
#include "src/models/sparse/StandardRewardModel.h"
#include "src/transformer/EndComponentEliminator.h"
#include "src/utility/macros.h"
#include "src/utility/vector.h"

#include "src/exceptions/InvalidOperationException.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            
            template <class SparseMaModelType>
            SparseMaMultiObjectiveWeightVectorChecker<SparseMaModelType>::SparseMaMultiObjectiveWeightVectorChecker(PreprocessorData const& data) : SparseMultiObjectiveWeightVectorChecker<SparseMaModelType>(data) {
                // Set the (discretized) state action rewards.
                this->discreteActionRewards.resize(data.objectives.size());
                for(auto objIndex : this->objectivesWithNoUpperTimeBound) {
                    typename SparseMaModelType::RewardModelType const& rewModel = this->data.preprocessedModel.getRewardModel(this->data.objectives[objIndex].rewardModelName);
                    STORM_LOG_ASSERT(!rewModel.hasTransitionRewards(), "Preprocessed Reward model has transition rewards which is not expected.");
                    this->discreteActionRewards[objIndex] = rewModel.hasStateActionRewards() ? rewModel.getStateActionRewardVector() : std::vector<ValueType>(this->data.preprocessedModel.getTransitionMatrix().getRowCount(), storm::utility::zero<ValueType>());
                    if(rewModel.hasStateRewards()) {
                        // Note that state rewards are earned over time and thus play no role for probabilistic states
                        for(auto markovianState : this->data.getMarkovianStatesOfPreprocessedModel()) {
                            this->discreteActionRewards[objIndex][this->data.preprocessedModel.getTransitionMatrix().getRowGroupIndices()[markovianState]] += rewModel.getStateReward(markovianState) / this->data.preprocessedModel.getExitRate(markovianState);
                        }
                    }
                }
            }
            
            template <class SparseMaModelType>
            void SparseMaMultiObjectiveWeightVectorChecker<SparseMaModelType>::boundedPhase(std::vector<ValueType> const& weightVector, std::vector<ValueType>& weightedRewardVector) {
                
                // Split the preprocessed model into transitions from/to probabilistic/Markovian states.
                SubModel MS = createSubModel(true, weightedRewardVector);
                SubModel PS = createSubModel(false, weightedRewardVector);

                // Apply digitization to Markovian transitions
                ValueType digitizationConstant = getDigitizationConstant();
                digitize(MS, digitizationConstant);
                
                // Get for each occurring (digitized) timeBound the indices of the objectives with that bound.
                TimeBoundMap lowerTimeBounds;
                TimeBoundMap upperTimeBounds;
                digitizeTimeBounds(lowerTimeBounds, upperTimeBounds, digitizationConstant);
                
                // Initialize a minMaxSolver to compute an optimal scheduler (w.r.t. PS) for each epoch
                // The end components that stay in PS will be removed.
                // Note that the end component elimination could be omitted if we forbid zeno behavior
                std::unique_ptr<MinMaxSolverData> minMax = initMinMaxSolverData(PS);
                
                // Store the optimal choices of PS as computed by the minMax solver.
                std::vector<uint_fast64_t> optimalChoicesAtCurrentEpoch(PS.getNumberOfStates(), std::numeric_limits<uint_fast64_t>::max());
                
                // create a linear equation solver for the model induced by the optimal choice vector.
                // the solver will be updated whenever the optimal choice vector has changed.
                LinEqSolverData linEq;
                linEq.b.resize(PS.getNumberOfStates());
                
                // Stores the objectives for which we need to compute values in the current time epoch.
                storm::storage::BitVector consideredObjectives = this->objectivesWithNoUpperTimeBound;
                
                auto lowerTimeBoundIt = lowerTimeBounds.begin();
                auto upperTimeBoundIt = upperTimeBounds.begin();
                uint_fast64_t currentEpoch = std::max(lowerTimeBounds.empty() ? 0 : lowerTimeBoundIt->first - 1, upperTimeBounds.empty() ? 0 : upperTimeBoundIt->first); // consider lowerBound - 1 since we are interested in the first epoch that passes the bound
                while(true) {
                    // Update the objectives that are considered at the current time epoch as well as the (weighted) reward vectors.
                    updateDataToCurrentEpoch(MS, PS, *minMax, consideredObjectives, currentEpoch, weightVector, lowerTimeBoundIt, lowerTimeBounds, upperTimeBoundIt, upperTimeBounds);
                    
                    // Compute the values that can be obtained at probabilistic states in the current time epoch
                    performPSStep(PS, MS, *minMax, linEq, optimalChoicesAtCurrentEpoch,  consideredObjectives);
                    
                    // Compute values that can be obtained at Markovian states after letting one (digitized) time unit pass.
                    // Only perform such a step if there is time left.
                    if(currentEpoch>0) {
                        performMSStep(MS, PS, consideredObjectives);
                        if(currentEpoch % 1000 == 0) {
                            STORM_LOG_DEBUG(currentEpoch << " digitized time steps left. Current weighted value is " << PS.weightedSolutionVector[0]);
                            std::cout << std::endl << currentEpoch << " digitized time steps left. Current weighted value is " << PS.weightedSolutionVector[0] << std::endl;
                        }
                        --currentEpoch;
                    } else {
                        break;
                    }
                }
                
                // compose the results from MS and PS
                storm::utility::vector::setVectorValues(this->weightedResult, MS.states, MS.weightedSolutionVector);
                storm::utility::vector::setVectorValues(this->weightedResult, PS.states, PS.weightedSolutionVector);
                for(uint_fast64_t objIndex = 0; objIndex < this->data.objectives.size(); ++objIndex) {
                    storm::utility::vector::setVectorValues(this->objectiveResults[objIndex], MS.states, MS.objectiveSolutionVectors[objIndex]);
                    storm::utility::vector::setVectorValues(this->objectiveResults[objIndex], PS.states, PS.objectiveSolutionVectors[objIndex]);
                }
            }
            
            
            template <class SparseMaModelType>
            typename SparseMaMultiObjectiveWeightVectorChecker<SparseMaModelType>::SubModel SparseMaMultiObjectiveWeightVectorChecker<SparseMaModelType>::createSubModel(bool createMS, std::vector<ValueType> const& weightedRewardVector) const {
                SubModel result;
                
                storm::storage::BitVector probabilisticStates = ~this->data.getMarkovianStatesOfPreprocessedModel();
                result.states = createMS ? this->data.getMarkovianStatesOfPreprocessedModel() : probabilisticStates;
                result.choices = this->data.preprocessedModel.getTransitionMatrix().getRowIndicesOfRowGroups(result.states);
                STORM_LOG_ASSERT(!createMS || result.states.getNumberOfSetBits() == result.choices.getNumberOfSetBits(), "row groups for Markovian states should consist of exactly one row");
                
                //We need to add diagonal entries for selfloops on Markovian states.
                result.toMS = this->data.preprocessedModel.getTransitionMatrix().getSubmatrix(true, result.states, this->data.getMarkovianStatesOfPreprocessedModel(), createMS);
                result.toPS = this->data.preprocessedModel.getTransitionMatrix().getSubmatrix(true, result.states, probabilisticStates, false);
                STORM_LOG_ASSERT(result.getNumberOfStates() == result.states.getNumberOfSetBits() && result.getNumberOfStates() == result.toMS.getRowGroupCount() && result.getNumberOfStates() == result.toPS.getRowGroupCount(), "Invalid state count for subsystem");
                STORM_LOG_ASSERT(result.getNumberOfChoices() == result.choices.getNumberOfSetBits() && result.getNumberOfChoices() == result.toMS.getRowCount() && result.getNumberOfChoices() == result.toPS.getRowCount(), "Invalid state count for subsystem");
                
                result.weightedRewardVector.resize(result.getNumberOfChoices());
                storm::utility::vector::selectVectorValues(result.weightedRewardVector, result.choices, weightedRewardVector);
                result.objectiveRewardVectors.resize(this->data.objectives.size());
                for(uint_fast64_t objIndex = 0; objIndex < this->data.objectives.size(); ++objIndex) {
                    std::vector<ValueType>& objVector = result.objectiveRewardVectors[objIndex];
                    objVector = std::vector<ValueType>(result.weightedRewardVector.size(), storm::utility::zero<ValueType>());
                    if(this->objectivesWithNoUpperTimeBound.get(objIndex)) {
                        storm::utility::vector::selectVectorValues(objVector, result.choices, this->discreteActionRewards[objIndex]);
                    } else {
                        typename SparseMaModelType::RewardModelType const& rewModel = this->data.preprocessedModel.getRewardModel(this->data.objectives[objIndex].rewardModelName);
                        STORM_LOG_ASSERT(!rewModel.hasTransitionRewards(), "Preprocessed Reward model has transition rewards which is not expected.");
                        STORM_LOG_ASSERT(!rewModel.hasStateRewards(), "State rewards for bounded objectives for MAs are not expected (bounded rewards are not supported).");
                        if(rewModel.hasStateActionRewards()) {
                            storm::utility::vector::selectVectorValues(objVector, result.choices, rewModel.getStateActionRewardVector());
                        }
                    }
                }
                
                result.weightedSolutionVector.resize(result.getNumberOfStates());
                storm::utility::vector::selectVectorValues(result.weightedSolutionVector, result.states, this->weightedResult);
                result.objectiveSolutionVectors.resize(this->data.objectives.size());
                for(uint_fast64_t objIndex = 0; objIndex < this->data.objectives.size(); ++objIndex) {
                    result.objectiveSolutionVectors[objIndex].resize(result.weightedSolutionVector.size());
                    storm::utility::vector::selectVectorValues(result.objectiveSolutionVectors[objIndex], result.states, this->objectiveResults[objIndex]);
                }
                
                result.auxChoiceValues.resize(result.getNumberOfChoices());
                
                return result;
            }
            
            template <class SparseMaModelType>
            template <typename VT, typename std::enable_if<storm::NumberTraits<VT>::SupportsExponential, int>::type>
            VT SparseMaMultiObjectiveWeightVectorChecker<SparseMaModelType>::getDigitizationConstant() const {
                STORM_LOG_DEBUG("Retrieving digitization constant");
                // We need to find a delta such that for each pair of lower and upper bounds it holds that
                // 1 - e^(-maxRate lowerbound) * (1 + maxRate delta) ^ (lowerbound / delta) + 1-e^(-maxRate upperbound) * (1 + maxRate delta) ^ (upperbound / delta) <= maximumLowerUpperBoundGap
                // and lowerbound/delta , upperbound/delta are natural numbers.
                
                // Initialize some data for fast and easy access
                VT const maxRate = this->data.preprocessedModel.getMaximalExitRate();
                std::vector<std::pair<VT, VT>> boundPairs;
                std::vector<std::pair<VT, VT>> eToPowerOfMinusMaxRateTimesBound;
                for(auto const& obj : this->data.objectives) {
                    if(obj.lowerTimeBound || obj.upperTimeBound) {
                        boundPairs.emplace_back(obj.lowerTimeBound ? (*obj.lowerTimeBound) : storm::utility::zero<VT>(),
                                                obj.upperTimeBound ? (*obj.upperTimeBound) : storm::utility::zero<VT>());
                        eToPowerOfMinusMaxRateTimesBound.emplace_back(std::exp(-maxRate * boundPairs.back().first),
                                                                      std::exp(-maxRate * boundPairs.back().second));
                    }
                }
                VT smallestNonZeroBound = storm::utility::zero<VT>();
                for (auto const& bounds : boundPairs) {
                    if(!storm::utility::isZero(bounds.first)) {
                        smallestNonZeroBound = storm::utility::isZero(smallestNonZeroBound) ? bounds.first : std::min(smallestNonZeroBound, bounds.first);
                    } else if (!storm::utility::isZero(bounds.second)) {
                        smallestNonZeroBound = storm::utility::isZero(smallestNonZeroBound) ? bounds.second : std::min(smallestNonZeroBound, bounds.second);
                    }
                }
                if(storm::utility::isZero(smallestNonZeroBound)) {
                    // All time bounds are zero which means that any delta>0 is valid.
                    // This includes the case where there are no time bounds
                    return storm::utility::one<VT>();
                }
                
                // We brute-force a delta, since a direct computation is apparently not easy.
                // Also note that the number of times this loop runs is a lower bound for the number of minMaxSolver invocations.
                // Hence, this brute-force approach will most likely not be a bottleneck.
                uint_fast64_t smallestStepBound = 1;
                VT delta = smallestNonZeroBound / smallestStepBound;
                while(true) {
                    bool deltaValid = true;
                    for(auto const& bounds : boundPairs) {
                        if(bounds.first/delta  != std::floor(bounds.first/delta) ||
                           bounds.second/delta != std::floor(bounds.second/delta)) {
                            deltaValid = false;
                            break;
                        }
                    }
                    if(deltaValid) {
                        for(uint_fast64_t i = 0; i<boundPairs.size(); ++i) {
                            VT precisionOfObj = storm::utility::one<VT>() - (eToPowerOfMinusMaxRateTimesBound[i].first * storm::utility::pow(storm::utility::one<VT>() + maxRate * delta, boundPairs[i].first / delta) );
                            precisionOfObj += storm::utility::one<VT>() - (eToPowerOfMinusMaxRateTimesBound[i].second * storm::utility::pow(storm::utility::one<VT>() + maxRate * delta, boundPairs[i].second / delta) );
                            if(precisionOfObj > this->maximumLowerUpperBoundGap) {
                                deltaValid = false;
                                break;
                            }
                        }
                    }
                    if(deltaValid) {
                        break;
                    }
                    ++smallestStepBound;
                    STORM_LOG_ASSERT(delta>smallestNonZeroBound / smallestStepBound, "Digitization constant is expected to become smaller in every iteration");
                    delta = smallestNonZeroBound / smallestStepBound;
                }
                STORM_LOG_DEBUG("Found digitization constant: " << delta << ". At least " << smallestStepBound << " digitization steps will be necessarry");
                return delta;
            }
            
            template <class SparseMaModelType>
            template <typename VT, typename std::enable_if<!storm::NumberTraits<VT>::SupportsExponential, int>::type>
            VT SparseMaMultiObjectiveWeightVectorChecker<SparseMaModelType>::getDigitizationConstant() const {
                  STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Computing bounded probabilities of MAs is unsupported for this value type.");
            }
            
            template <class SparseMaModelType>
            template <typename VT, typename std::enable_if<storm::NumberTraits<VT>::SupportsExponential, int>::type>
            void SparseMaMultiObjectiveWeightVectorChecker<SparseMaModelType>::digitize(SubModel& MS, VT const& digitizationConstant) const {
                std::vector<VT> rateVector(MS.getNumberOfChoices());
                storm::utility::vector::selectVectorValues(rateVector, MS.states, this->data.preprocessedModel.getExitRates());
                for(uint_fast64_t row = 0; row < rateVector.size(); ++row) {
                    VT const eToMinusRateTimesDelta = std::exp(-rateVector[row] * digitizationConstant);
                    for(auto& entry : MS.toMS.getRow(row)) {
                        entry.setValue((storm::utility::one<VT>() - eToMinusRateTimesDelta) * entry.getValue());
                        if(entry.getColumn() == row) {
                            entry.setValue(entry.getValue() + eToMinusRateTimesDelta);
                        }
                    }
                    for(auto& entry : MS.toPS.getRow(row)) {
                        entry.setValue((storm::utility::one<VT>() - eToMinusRateTimesDelta) * entry.getValue());
                    }
                    MS.weightedRewardVector[row] *= storm::utility::one<VT>() - eToMinusRateTimesDelta;
                    for(auto& objVector : MS.objectiveRewardVectors) {
                        objVector[row] *= storm::utility::one<VT>() - eToMinusRateTimesDelta;
                    }
                }
            }
            
            template <class SparseMaModelType>
            template <typename VT, typename std::enable_if<!storm::NumberTraits<VT>::SupportsExponential, int>::type>
            void SparseMaMultiObjectiveWeightVectorChecker<SparseMaModelType>::digitize(SubModel& subModel, VT const& digitizationConstant) const {
                STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Computing bounded probabilities of MAs is unsupported for this value type.");
            }

            template <class SparseMaModelType>
            template <typename VT, typename std::enable_if<storm::NumberTraits<VT>::SupportsExponential, int>::type>
            void SparseMaMultiObjectiveWeightVectorChecker<SparseMaModelType>::digitizeTimeBounds(TimeBoundMap& lowerTimeBounds, TimeBoundMap& upperTimeBounds, VT const& digitizationConstant) {
                
                VT const maxRate = this->data.preprocessedModel.getMaximalExitRate();
                for(uint_fast64_t objIndex = 0; objIndex < this->data.objectives.size(); ++objIndex) {
                    auto const& obj = this->data.objectives[objIndex];
                    if(obj.lowerTimeBound) {
                        uint_fast64_t digitizedBound = storm::utility::convertNumber<uint_fast64_t>((*obj.lowerTimeBound)/digitizationConstant);
                        auto timeBoundIt = lowerTimeBounds.insert(std::make_pair(digitizedBound, storm::storage::BitVector(this->data.objectives.size(), false))).first;
                        timeBoundIt->second.set(objIndex);
                        VT digitizationError = storm::utility::one<VT>();
                        digitizationError -= std::exp(-maxRate * (*obj.lowerTimeBound)) * storm::utility::pow(storm::utility::one<VT>() + maxRate * digitizationConstant, digitizedBound);
                        this->offsetsToLowerBound[objIndex] = -digitizationError;
                    } else {
                        this->offsetsToLowerBound[objIndex] = storm::utility::zero<VT>();
                    }
                    if(obj.upperTimeBound) {
                        uint_fast64_t digitizedBound = storm::utility::convertNumber<uint_fast64_t>((*obj.upperTimeBound)/digitizationConstant);
                        auto timeBoundIt = upperTimeBounds.insert(std::make_pair(digitizedBound, storm::storage::BitVector(this->data.objectives.size(), false))).first;
                        timeBoundIt->second.set(objIndex);
                        VT digitizationError = storm::utility::one<VT>();
                        digitizationError -= std::exp(-maxRate * (*obj.upperTimeBound)) * storm::utility::pow(storm::utility::one<VT>() + maxRate * digitizationConstant, digitizedBound);
                        this->offsetsToUpperBound[objIndex] = digitizationError;
                    } else {
                        this->offsetsToUpperBound[objIndex] = storm::utility::zero<VT>();
                    }
                    STORM_LOG_ASSERT(this->offsetsToUpperBound[objIndex] - this->offsetsToLowerBound[objIndex] <= this->maximumLowerUpperBoundGap, "Precision not sufficient.");
                }
            } 
            
            template <class SparseMaModelType>
            template <typename VT, typename std::enable_if<!storm::NumberTraits<VT>::SupportsExponential, int>::type>
            void SparseMaMultiObjectiveWeightVectorChecker<SparseMaModelType>::digitizeTimeBounds(TimeBoundMap& lowerTimeBounds, TimeBoundMap& upperTimeBounds, VT const& digitizationConstant) {
                STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Computing bounded probabilities of MAs is unsupported for this value type.");
            }
            
            template <class SparseMaModelType>
            std::unique_ptr<typename SparseMaMultiObjectiveWeightVectorChecker<SparseMaModelType>::MinMaxSolverData> SparseMaMultiObjectiveWeightVectorChecker<SparseMaModelType>::initMinMaxSolverData(SubModel const& PS) const {
                std::unique_ptr<MinMaxSolverData> result(new MinMaxSolverData());
                
                storm::storage::BitVector choicesStayingInPS(PS.getNumberOfChoices(), false);
                for(uint_fast64_t choice = 0; choice < PS.toPS.getRowCount(); ++choice) {
                    if(storm::utility::isOne(PS.toPS.getRowSum(choice))) {
                        choicesStayingInPS.set(choice);
                    }
                }
                auto ecEliminatorResult = storm::transformer::EndComponentEliminator<ValueType>::transform(PS.toPS, choicesStayingInPS & storm::utility::vector::filterZero(PS.weightedRewardVector), storm::storage::BitVector(PS.getNumberOfStates(), true));
                result->matrix = std::move(ecEliminatorResult.matrix);
                result->toPSChoiceMapping = std::move(ecEliminatorResult.newToOldRowMapping);
                result->fromPSStateMapping = std::move(ecEliminatorResult.oldToNewStateMapping);
                
                result->b.resize(result->matrix.getRowCount());
                result->x.resize(result->matrix.getRowGroupCount());
                for(uint_fast64_t state=0; state < result->fromPSStateMapping.size(); ++state) {
                    if(result->fromPSStateMapping[state] < result->x.size()) {
                        result->x[result->fromPSStateMapping[state]] = PS.weightedSolutionVector[state];
                    }
                }
                
                storm::solver::GeneralMinMaxLinearEquationSolverFactory<ValueType> minMaxSolverFactory;
                result->solver = minMaxSolverFactory.create(result->matrix);
                result->solver->setOptimizationDirection(storm::solver::OptimizationDirection::Maximize);
                result->solver->setTrackScheduler(true);
                result->solver->allocateAuxMemory(storm::solver::MinMaxLinearEquationSolverOperation::SolveEquations);
                
                return result;
            }

            template <class SparseMaModelType>
            void SparseMaMultiObjectiveWeightVectorChecker<SparseMaModelType>::updateDataToCurrentEpoch(SubModel& MS, SubModel& PS, MinMaxSolverData& minMax, storm::storage::BitVector& consideredObjectives, uint_fast64_t const& currentEpoch, std::vector<ValueType> const& weightVector, TimeBoundMap::iterator& lowerTimeBoundIt, TimeBoundMap const& lowerTimeBounds, TimeBoundMap::iterator& upperTimeBoundIt, TimeBoundMap const& upperTimeBounds) {
                
                //For lower time bounds we need to react when the currentEpoch passed the bound
                // Hence, we substract 1 from the lower time bounds.
                if(lowerTimeBoundIt != lowerTimeBounds.end() && currentEpoch == lowerTimeBoundIt->first - 1) {
                    for(auto objIndex : lowerTimeBoundIt->second) {
                        // No more reward is earned for this objective.
                        storm::utility::vector::addScaledVector(MS.weightedRewardVector, MS.objectiveRewardVectors[objIndex], -weightVector[objIndex]);
                        storm::utility::vector::addScaledVector(PS.weightedRewardVector, PS.objectiveRewardVectors[objIndex], -weightVector[objIndex]);
                        MS.objectiveRewardVectors[objIndex] = std::vector<ValueType>(MS.objectiveRewardVectors[objIndex].size(), storm::utility::zero<ValueType>());
                        PS.objectiveRewardVectors[objIndex] = std::vector<ValueType>(PS.objectiveRewardVectors[objIndex].size(), storm::utility::zero<ValueType>());
                    }
                    ++lowerTimeBoundIt;
                }
                
                if(upperTimeBoundIt != upperTimeBounds.end() && currentEpoch == upperTimeBoundIt->first) {
                    consideredObjectives |= upperTimeBoundIt->second;
                    for(auto objIndex : upperTimeBoundIt->second) {
                        // This objective now plays a role in the weighted sum
                        storm::utility::vector::addScaledVector(MS.weightedRewardVector, MS.objectiveRewardVectors[objIndex], weightVector[objIndex]);
                        storm::utility::vector::addScaledVector(PS.weightedRewardVector, PS.objectiveRewardVectors[objIndex], weightVector[objIndex]);
                    }
                    ++upperTimeBoundIt;
                }
                
                // Update the solver data
                PS.toMS.multiplyWithVector(MS.weightedSolutionVector, PS.auxChoiceValues);
                storm::utility::vector::addVectors(PS.auxChoiceValues, PS.weightedRewardVector, PS.auxChoiceValues);
                storm::utility::vector::selectVectorValues(minMax.b, minMax.toPSChoiceMapping, PS.auxChoiceValues);
            }
            
            template <class SparseMaModelType>
            void SparseMaMultiObjectiveWeightVectorChecker<SparseMaModelType>::performPSStep(SubModel& PS, SubModel const& MS, MinMaxSolverData& minMax, LinEqSolverData& linEq, std::vector<uint_fast64_t>& optimalChoicesAtCurrentEpoch,  storm::storage::BitVector const& consideredObjectives) const {
                
                // compute a choice vector for the probabilistic states that is optimal w.r.t. the weighted reward vector
                std::vector<uint_fast64_t> newOptimalChoices(PS.getNumberOfStates());
                minMax.solver->solveEquations(minMax.x, minMax.b);
                this->transformReducedSolutionToOriginalModel(minMax.matrix, minMax.x, minMax.solver->getScheduler()->getChoices(), minMax.toPSChoiceMapping, minMax.fromPSStateMapping, PS.toPS, PS.weightedSolutionVector, newOptimalChoices);
                
                // check whether the linEqSolver needs to be updated, i.e., whether the scheduler has changed
                if(newOptimalChoices != optimalChoicesAtCurrentEpoch) {
                    std::cout << "X";
                    optimalChoicesAtCurrentEpoch.swap(newOptimalChoices);
                    linEq.solver = nullptr;
                    storm::storage::SparseMatrix<ValueType> linEqMatrix = PS.toPS.selectRowsFromRowGroups(optimalChoicesAtCurrentEpoch, true);
                    linEqMatrix.convertToEquationSystem();
                    linEq.solver = linEq.factory.create(std::move(linEqMatrix));
                } else {
                    std::cout << " ";
                }
                for(auto objIndex : consideredObjectives) {
                    PS.toMS.multiplyWithVector(MS.objectiveSolutionVectors[objIndex], PS.auxChoiceValues);
                    storm::utility::vector::addVectors(PS.auxChoiceValues, PS.objectiveRewardVectors[objIndex], PS.auxChoiceValues);
                    storm::utility::vector::selectVectorValues(linEq.b, optimalChoicesAtCurrentEpoch, PS.toPS.getRowGroupIndices(), PS.auxChoiceValues);
                    linEq.solver->solveEquations(PS.objectiveSolutionVectors[objIndex], linEq.b);
                }
            }

            template <class SparseMaModelType>
            void SparseMaMultiObjectiveWeightVectorChecker<SparseMaModelType>::performMSStep(SubModel& MS, SubModel const& PS, storm::storage::BitVector const& consideredObjectives) const {
                
                MS.toMS.multiplyWithVector(MS.weightedSolutionVector, MS.auxChoiceValues);
                storm::utility::vector::addVectors(MS.weightedRewardVector, MS.auxChoiceValues, MS.weightedSolutionVector);
                MS.toPS.multiplyWithVector(PS.weightedSolutionVector, MS.auxChoiceValues);
                storm::utility::vector::addVectors(MS.weightedSolutionVector, MS.auxChoiceValues, MS.weightedSolutionVector);
                
                for(auto objIndex : consideredObjectives) {
                    MS.toMS.multiplyWithVector(MS.objectiveSolutionVectors[objIndex], MS.auxChoiceValues);
                    storm::utility::vector::addVectors(MS.objectiveRewardVectors[objIndex], MS.auxChoiceValues, MS.objectiveSolutionVectors[objIndex]);
                    MS.toPS.multiplyWithVector(PS.objectiveSolutionVectors[objIndex], MS.auxChoiceValues);
                    storm::utility::vector::addVectors(MS.objectiveSolutionVectors[objIndex], MS.auxChoiceValues, MS.objectiveSolutionVectors[objIndex]);
                }
            }
            
            
            template class SparseMaMultiObjectiveWeightVectorChecker<storm::models::sparse::MarkovAutomaton<double>>;
            template double SparseMaMultiObjectiveWeightVectorChecker<storm::models::sparse::MarkovAutomaton<double>>::getDigitizationConstant<double>() const;
            template void SparseMaMultiObjectiveWeightVectorChecker<storm::models::sparse::MarkovAutomaton<double>>::digitize<double>(SparseMaMultiObjectiveWeightVectorChecker<storm::models::sparse::MarkovAutomaton<double>>::SubModel& subModel, double const& digitizationConstant) const;
            template void SparseMaMultiObjectiveWeightVectorChecker<storm::models::sparse::MarkovAutomaton<double>>::digitizeTimeBounds<double>(SparseMaMultiObjectiveWeightVectorChecker<storm::models::sparse::MarkovAutomaton<double>>::TimeBoundMap& lowerTimeBounds, SparseMaMultiObjectiveWeightVectorChecker<storm::models::sparse::MarkovAutomaton<double>>::TimeBoundMap& upperTimeBounds, double const& digitizationConstant);
#ifdef STORM_HAVE_CARL
            template class SparseMaMultiObjectiveWeightVectorChecker<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>;
            template storm::RationalNumber SparseMaMultiObjectiveWeightVectorChecker<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>::getDigitizationConstant<storm::RationalNumber>() const;
            template void SparseMaMultiObjectiveWeightVectorChecker<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>::digitize<storm::RationalNumber>(SparseMaMultiObjectiveWeightVectorChecker<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>::SubModel& subModel, storm::RationalNumber const& digitizationConstant) const;
            template void SparseMaMultiObjectiveWeightVectorChecker<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>::digitizeTimeBounds<storm::RationalNumber>(SparseMaMultiObjectiveWeightVectorChecker<storm::models::sparse::MarkovAutomaton<double>>::TimeBoundMap& lowerTimeBounds, SparseMaMultiObjectiveWeightVectorChecker<storm::models::sparse::MarkovAutomaton<double>>::TimeBoundMap& upperTimeBounds, storm::RationalNumber const& digitizationConstant);
#endif
            
        }
    }
}
