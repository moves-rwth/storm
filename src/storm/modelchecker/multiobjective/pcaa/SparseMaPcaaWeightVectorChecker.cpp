#include "storm/modelchecker/multiobjective/pcaa/SparseMaPcaaWeightVectorChecker.h"

#include <cmath>

#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/utility/macros.h"
#include "storm/utility/vector.h"
#include "storm/solver/GmmxxLinearEquationSolver.h"
#include "storm/logic/Formulas.h"

#include "storm/exceptions/InvalidOperationException.h"
#include "storm/exceptions/InvalidPropertyException.h"
#include "storm/exceptions/UnexpectedException.h"

namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            
            template <class SparseMaModelType>
            SparseMaPcaaWeightVectorChecker<SparseMaModelType>::SparseMaPcaaWeightVectorChecker(SparseMaModelType const& model,
                                                                                                std::vector<Objective<ValueType>> const& objectives,
                                                                                                storm::storage::BitVector const& possibleECActions,
                                                                                                storm::storage::BitVector const& possibleBottomStates) :
            SparsePcaaWeightVectorChecker<SparseMaModelType>(model, objectives, possibleECActions, possibleBottomStates) {
                // Set the (discretized) state action rewards.
                this->discreteActionRewards.resize(objectives.size());
                for (auto objIndex : this->objectivesWithNoUpperTimeBound) {
                    auto const& formula = *objectives[objIndex].formula;
                    STORM_LOG_THROW(formula.isRewardOperatorFormula() && formula.asRewardOperatorFormula().hasRewardModelName(), storm::exceptions::UnexpectedException, "Unexpected type of operator formula: " << formula);
                    STORM_LOG_THROW(formula.getSubformula().isTotalRewardFormula() || (formula.getSubformula().isCumulativeRewardFormula() && formula.getSubformula().asCumulativeRewardFormula().isTimeBounded()), storm::exceptions::UnexpectedException, "Unexpected type of sub-formula: " << formula.getSubformula());
                    STORM_LOG_WARN_COND(!formula.getSubformula().isCumulativeRewardFormula() || (objectives[objIndex].originalFormula->isProbabilityOperatorFormula() && objectives[objIndex].originalFormula->asProbabilityOperatorFormula().getSubformula().isBoundedUntilFormula()), "Objective " << objectives[objIndex].originalFormula << " was simplified to a cumulative reward formula. Correctness of the algorithm is unknown for this type of property.");
                    typename SparseMaModelType::RewardModelType const& rewModel = this->model.getRewardModel(formula.asRewardOperatorFormula().getRewardModelName());
                    STORM_LOG_ASSERT(!rewModel.hasTransitionRewards(), "Preprocessed Reward model has transition rewards which is not expected.");
                    this->discreteActionRewards[objIndex] = rewModel.hasStateActionRewards() ? rewModel.getStateActionRewardVector() : std::vector<ValueType>(this->model.getTransitionMatrix().getRowCount(), storm::utility::zero<ValueType>());
                    if (rewModel.hasStateRewards()) {
                        // Note that state rewards are earned over time and thus play no role for probabilistic states
                        for (auto markovianState : this->model.getMarkovianStates()) {
                            this->discreteActionRewards[objIndex][this->model.getTransitionMatrix().getRowGroupIndices()[markovianState]] += rewModel.getStateReward(markovianState) / this->model.getExitRate(markovianState);
                        }
                    }
                }
            }
            
            template <class SparseMaModelType>
            void SparseMaPcaaWeightVectorChecker<SparseMaModelType>::boundedPhase(std::vector<ValueType> const& weightVector, std::vector<ValueType>& weightedRewardVector) {
                
                // Split the preprocessed model into transitions from/to probabilistic/Markovian states.
                SubModel MS = createSubModel(true, weightedRewardVector);
                SubModel PS = createSubModel(false, weightedRewardVector);

                // Apply digitization to Markovian transitions
                ValueType digitizationConstant = getDigitizationConstant(weightVector);
                digitize(MS, digitizationConstant);
                
                // Get for each occurring (digitized) timeBound the indices of the objectives with that bound.
                TimeBoundMap upperTimeBounds;
                digitizeTimeBounds(upperTimeBounds, digitizationConstant);
                
                // Initialize a minMaxSolver to compute an optimal scheduler (w.r.t. PS) for each epoch
                // No EC elimination is necessary as we assume non-zenoness
                std::unique_ptr<MinMaxSolverData> minMax = initMinMaxSolver(PS);
                
                // create a linear equation solver for the model induced by the optimal choice vector.
                // the solver will be updated whenever the optimal choice vector has changed.
                std::unique_ptr<LinEqSolverData> linEq = initLinEqSolver(PS);
                
                // Store the optimal choices of PS as computed by the minMax solver.
                std::vector<uint_fast64_t> optimalChoicesAtCurrentEpoch(PS.getNumberOfStates(), std::numeric_limits<uint_fast64_t>::max());
                
                // Stores the objectives for which we need to compute values in the current time epoch.
                storm::storage::BitVector consideredObjectives = this->objectivesWithNoUpperTimeBound;
                
                auto upperTimeBoundIt = upperTimeBounds.begin();
                uint_fast64_t currentEpoch = upperTimeBounds.empty() ? 0 : upperTimeBoundIt->first;
                while (true) {
                    // Update the objectives that are considered at the current time epoch as well as the (weighted) reward vectors.
                    updateDataToCurrentEpoch(MS, PS, *minMax, consideredObjectives, currentEpoch, weightVector, upperTimeBoundIt, upperTimeBounds);
                    
                    // Compute the values that can be obtained at probabilistic states in the current time epoch
                    performPSStep(PS, MS, *minMax, *linEq, optimalChoicesAtCurrentEpoch,  consideredObjectives, weightVector);
                    
                    // Compute values that can be obtained at Markovian states after letting one (digitized) time unit pass.
                    // Only perform such a step if there is time left.
                    if (currentEpoch>0) {
                        performMSStep(MS, PS, consideredObjectives, weightVector);
                        --currentEpoch;
                    } else {
                        break;
                    }
                }
                
                // compose the results from MS and PS
                storm::utility::vector::setVectorValues(this->weightedResult, MS.states, MS.weightedSolutionVector);
                storm::utility::vector::setVectorValues(this->weightedResult, PS.states, PS.weightedSolutionVector);
                for (uint_fast64_t objIndex = 0; objIndex < this->objectives.size(); ++objIndex) {
                    storm::utility::vector::setVectorValues(this->objectiveResults[objIndex], MS.states, MS.objectiveSolutionVectors[objIndex]);
                    storm::utility::vector::setVectorValues(this->objectiveResults[objIndex], PS.states, PS.objectiveSolutionVectors[objIndex]);
                }
            }
            
            
            template <class SparseMaModelType>
            typename SparseMaPcaaWeightVectorChecker<SparseMaModelType>::SubModel SparseMaPcaaWeightVectorChecker<SparseMaModelType>::createSubModel(bool createMS, std::vector<ValueType> const& weightedRewardVector) const {
                SubModel result;
                
                storm::storage::BitVector probabilisticStates = ~this->model.getMarkovianStates();
                result.states = createMS ? this->model.getMarkovianStates() : probabilisticStates;
                result.choices = this->model.getTransitionMatrix().getRowFilter(result.states);
                STORM_LOG_ASSERT(!createMS || result.states.getNumberOfSetBits() == result.choices.getNumberOfSetBits(), "row groups for Markovian states should consist of exactly one row");
                
                //We need to add diagonal entries for selfloops on Markovian states.
                result.toMS = this->model.getTransitionMatrix().getSubmatrix(true, result.states, this->model.getMarkovianStates(), createMS);
                result.toPS = this->model.getTransitionMatrix().getSubmatrix(true, result.states, probabilisticStates, false);
                STORM_LOG_ASSERT(result.getNumberOfStates() == result.states.getNumberOfSetBits() && result.getNumberOfStates() == result.toMS.getRowGroupCount() && result.getNumberOfStates() == result.toPS.getRowGroupCount(), "Invalid state count for subsystem");
                STORM_LOG_ASSERT(result.getNumberOfChoices() == result.choices.getNumberOfSetBits() && result.getNumberOfChoices() == result.toMS.getRowCount() && result.getNumberOfChoices() == result.toPS.getRowCount(), "Invalid choice count for subsystem");
                
                result.weightedRewardVector.resize(result.getNumberOfChoices());
                storm::utility::vector::selectVectorValues(result.weightedRewardVector, result.choices, weightedRewardVector);
                result.objectiveRewardVectors.resize(this->objectives.size());
                for (uint_fast64_t objIndex = 0; objIndex < this->objectives.size(); ++objIndex) {
                    std::vector<ValueType>& objVector = result.objectiveRewardVectors[objIndex];
                    objVector = std::vector<ValueType>(result.weightedRewardVector.size(), storm::utility::zero<ValueType>());
                    if (this->objectivesWithNoUpperTimeBound.get(objIndex)) {
                        storm::utility::vector::selectVectorValues(objVector, result.choices, this->discreteActionRewards[objIndex]);
                    } else {
                       typename SparseMaModelType::RewardModelType const& rewModel = this->model.getRewardModel(this->objectives[objIndex].formula->asRewardOperatorFormula().getRewardModelName());
                        STORM_LOG_ASSERT(!rewModel.hasTransitionRewards(), "Preprocessed Reward model has transition rewards which is not expected.");
                        STORM_LOG_ASSERT(!rewModel.hasStateRewards(), "State rewards for bounded objectives for MAs are not expected (bounded rewards are not supported).");
                        if (rewModel.hasStateActionRewards()) {
                            storm::utility::vector::selectVectorValues(objVector, result.choices, rewModel.getStateActionRewardVector());
                        }
                    }
                }
                
                result.weightedSolutionVector.resize(result.getNumberOfStates());
                storm::utility::vector::selectVectorValues(result.weightedSolutionVector, result.states, this->weightedResult);
                result.objectiveSolutionVectors.resize(this->objectives.size());
                for (uint_fast64_t objIndex = 0; objIndex < this->objectives.size(); ++objIndex) {
                    result.objectiveSolutionVectors[objIndex].resize(result.weightedSolutionVector.size());
                    storm::utility::vector::selectVectorValues(result.objectiveSolutionVectors[objIndex], result.states, this->objectiveResults[objIndex]);
                }
                
                result.auxChoiceValues.resize(result.getNumberOfChoices());
                
                return result;
            }
            
            template <class SparseMaModelType>
            template <typename VT, typename std::enable_if<storm::NumberTraits<VT>::SupportsExponential, int>::type>
            VT SparseMaPcaaWeightVectorChecker<SparseMaModelType>::getDigitizationConstant(std::vector<ValueType> const& weightVector) const {
                STORM_LOG_DEBUG("Retrieving digitization constant");
                // We need to find a delta such that for each objective it holds that lowerbound/delta , upperbound/delta are natural numbers and
                // sum_{obj_i} (
                //   If obj_i has a lower and an upper bound:
                //     weightVector_i * (1 - e^(-maxRate lowerbound) * (1 + maxRate delta) ^ (lowerbound / delta) + 1-e^(-maxRate upperbound) * (1 + maxRate delta) ^ (upperbound / delta) + (1-e^(-maxRate delta)))
                //   If there is only an upper bound:
                //     weightVector_i * ( 1-e^(-maxRate upperbound) * (1 + maxRate delta) ^ (upperbound / delta))
                // ) <= this->maximumLowerUpperDistance
                
                // Initialize some data for fast and easy access
                VT const maxRate = this->model.getMaximalExitRate();
                std::vector<VT> timeBounds;
                std::vector<VT> eToPowerOfMinusMaxRateTimesBound;
                VT smallestNonZeroBound = storm::utility::zero<VT>();
                for (auto const& obj : this->objectives) {
                    if (obj.formula->getSubformula().isCumulativeRewardFormula()) {
                        timeBounds.push_back(obj.formula->getSubformula().asCumulativeRewardFormula().template getBound<VT>());
                        STORM_LOG_THROW(!storm::utility::isZero(timeBounds.back()), storm::exceptions::InvalidPropertyException, "Got zero-valued upper time bound. This is not suppoted.");
                        eToPowerOfMinusMaxRateTimesBound.push_back(std::exp(-maxRate * timeBounds.back()));
                        smallestNonZeroBound = storm::utility::isZero(smallestNonZeroBound) ? timeBounds.back() : std::min(smallestNonZeroBound, timeBounds.back());
                    } else {
                        timeBounds.push_back(storm::utility::zero<VT>());
                        eToPowerOfMinusMaxRateTimesBound.push_back(storm::utility::zero<VT>());
                    }
                }
                if (storm::utility::isZero(smallestNonZeroBound)) {
                    // There are no time bounds. In this case, one is a valid digitization constant.
                    return storm::utility::one<VT>();
                }
                VT goalPrecisionTimesNorm = this->weightedPrecision * storm::utility::sqrt(storm::utility::vector::dotProduct(weightVector, weightVector));
                
                // We brute-force a delta, since a direct computation is apparently not easy.
                // Also note that the number of times this loop runs is a lower bound for the number of minMaxSolver invocations.
                // Hence, this brute-force approach will most likely not be a bottleneck.
                storm::storage::BitVector objectivesWithTimeBound = ~this->objectivesWithNoUpperTimeBound;
                uint_fast64_t smallestStepBound = 1;
                VT delta = smallestNonZeroBound / smallestStepBound;
                while(true) {
                    bool deltaValid = true;
                    for (auto const& objIndex : objectivesWithTimeBound) {
                        auto const& timeBound = timeBounds[objIndex];
                        if (timeBound/delta != std::floor(timeBound/delta)) {
                            deltaValid = false;
                            break;
                        }
                    }
                    if (deltaValid) {
                        VT weightedPrecisionForCurrentDelta = storm::utility::zero<VT>();
                        for (uint_fast64_t objIndex = 0; objIndex < this->objectives.size(); ++objIndex) {
                            VT precisionOfObj = storm::utility::zero<VT>();
                            if (objectivesWithTimeBound.get(objIndex)) {
                                precisionOfObj += storm::utility::one<VT>() - (eToPowerOfMinusMaxRateTimesBound[objIndex] * storm::utility::pow(storm::utility::one<VT>() + maxRate * delta, timeBounds[objIndex] / delta) );
                            }
                            weightedPrecisionForCurrentDelta += weightVector[objIndex] * precisionOfObj;
                        }
                        deltaValid &= weightedPrecisionForCurrentDelta <= goalPrecisionTimesNorm;
                    }
                    if (deltaValid) {
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
            VT SparseMaPcaaWeightVectorChecker<SparseMaModelType>::getDigitizationConstant(std::vector<ValueType> const& weightVector) const {
                  STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Computing bounded probabilities of MAs is unsupported for this value type.");
            }
            
            template <class SparseMaModelType>
            template <typename VT, typename std::enable_if<storm::NumberTraits<VT>::SupportsExponential, int>::type>
            void SparseMaPcaaWeightVectorChecker<SparseMaModelType>::digitize(SubModel& MS, VT const& digitizationConstant) const {
                std::vector<VT> rateVector(MS.getNumberOfChoices());
                storm::utility::vector::selectVectorValues(rateVector, MS.states, this->model.getExitRates());
                for (uint_fast64_t row = 0; row < rateVector.size(); ++row) {
                    VT const eToMinusRateTimesDelta = std::exp(-rateVector[row] * digitizationConstant);
                    for (auto& entry : MS.toMS.getRow(row)) {
                        entry.setValue((storm::utility::one<VT>() - eToMinusRateTimesDelta) * entry.getValue());
                        if (entry.getColumn() == row) {
                            entry.setValue(entry.getValue() + eToMinusRateTimesDelta);
                        }
                    }
                    for (auto& entry : MS.toPS.getRow(row)) {
                        entry.setValue((storm::utility::one<VT>() - eToMinusRateTimesDelta) * entry.getValue());
                    }
                    MS.weightedRewardVector[row] *= storm::utility::one<VT>() - eToMinusRateTimesDelta;
                    for (auto& objVector : MS.objectiveRewardVectors) {
                        objVector[row] *= storm::utility::one<VT>() - eToMinusRateTimesDelta;
                    }
                }
            }
            
            template <class SparseMaModelType>
            template <typename VT, typename std::enable_if<!storm::NumberTraits<VT>::SupportsExponential, int>::type>
            void SparseMaPcaaWeightVectorChecker<SparseMaModelType>::digitize(SubModel& subModel, VT const& digitizationConstant) const {
                STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Computing bounded probabilities of MAs is unsupported for this value type.");
            }

            template <class SparseMaModelType>
            template <typename VT, typename std::enable_if<storm::NumberTraits<VT>::SupportsExponential, int>::type>
            void SparseMaPcaaWeightVectorChecker<SparseMaModelType>::digitizeTimeBounds(TimeBoundMap& upperTimeBounds, VT const& digitizationConstant) {
                
                VT const maxRate = this->model.getMaximalExitRate();
                for (uint_fast64_t objIndex = 0; objIndex < this->objectives.size(); ++objIndex) {
                    auto const& obj = this->objectives[objIndex];
                    VT errorTowardsZero = storm::utility::zero<VT>();
                    VT errorAwayFromZero = storm::utility::zero<VT>();
                    if (obj.formula->getSubformula().isCumulativeRewardFormula()) {
                        VT timeBound = obj.formula->getSubformula().asCumulativeRewardFormula().template getBound<VT>();
                        uint_fast64_t digitizedBound = storm::utility::convertNumber<uint_fast64_t>(timeBound/digitizationConstant);
                        auto timeBoundIt = upperTimeBounds.insert(std::make_pair(digitizedBound, storm::storage::BitVector(this->objectives.size(), false))).first;
                        timeBoundIt->second.set(objIndex);
                        VT digitizationError = storm::utility::one<VT>();
                        digitizationError -= std::exp(-maxRate * timeBound) * storm::utility::pow(storm::utility::one<VT>() + maxRate * digitizationConstant, digitizedBound);
                        errorAwayFromZero += digitizationError;
                    }
                    if (storm::solver::maximize(obj.formula->getOptimalityType())) {
                        this->offsetsToUnderApproximation[objIndex] = -errorTowardsZero;
                        this->offsetsToOverApproximation[objIndex] = errorAwayFromZero;
                    } else {
                        this->offsetsToUnderApproximation[objIndex] = errorAwayFromZero;
                        this->offsetsToOverApproximation[objIndex] = -errorTowardsZero;
                    }
                }
            } 
            
            template <class SparseMaModelType>
            template <typename VT, typename std::enable_if<!storm::NumberTraits<VT>::SupportsExponential, int>::type>
            void SparseMaPcaaWeightVectorChecker<SparseMaModelType>::digitizeTimeBounds(TimeBoundMap& upperTimeBounds, VT const& digitizationConstant) {
                STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Computing bounded probabilities of MAs is unsupported for this value type.");
            }
            
            template <class SparseMaModelType>
            std::unique_ptr<typename SparseMaPcaaWeightVectorChecker<SparseMaModelType>::MinMaxSolverData> SparseMaPcaaWeightVectorChecker<SparseMaModelType>::initMinMaxSolver(SubModel const& PS) const {
                std::unique_ptr<MinMaxSolverData> result(new MinMaxSolverData());
                storm::solver::GeneralMinMaxLinearEquationSolverFactory<ValueType> minMaxSolverFactory;
                result->solver = minMaxSolverFactory.create(PS.toPS);
                result->solver->setOptimizationDirection(storm::solver::OptimizationDirection::Maximize);
                result->solver->setTrackScheduler(true);
                result->solver->setCachingEnabled(true);
                
                result->b.resize(PS.getNumberOfChoices());
                
                return result;
            }

            template <class SparseMaModelType>
            template <typename VT, typename std::enable_if<storm::NumberTraits<VT>::SupportsExponential, int>::type>
            std::unique_ptr<typename SparseMaPcaaWeightVectorChecker<SparseMaModelType>::LinEqSolverData> SparseMaPcaaWeightVectorChecker<SparseMaModelType>::initLinEqSolver(SubModel const& PS) const {
                std::unique_ptr<LinEqSolverData> result(new LinEqSolverData());
                auto factory = std::make_unique<storm::solver::GmmxxLinearEquationSolverFactory<ValueType>>();
                // We choose Jacobi since we call the solver very frequently on 'easy' inputs (note that jacobi without preconditioning has very little overhead).
                factory->getSettings().setSolutionMethod(storm::solver::GmmxxLinearEquationSolverSettings<ValueType>::SolutionMethod::Jacobi);
                factory->getSettings().setPreconditioner(storm::solver::GmmxxLinearEquationSolverSettings<ValueType>::Preconditioner::None);
                result->factory = std::move(factory);
                result->b.resize(PS.getNumberOfStates());
                return result;
            }
              
            template <class SparseMaModelType>
            template <typename VT, typename std::enable_if<!storm::NumberTraits<VT>::SupportsExponential, int>::type>
            std::unique_ptr<typename SparseMaPcaaWeightVectorChecker<SparseMaModelType>::LinEqSolverData> SparseMaPcaaWeightVectorChecker<SparseMaModelType>::initLinEqSolver(SubModel const& PS) const {
                STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Computing bounded probabilities of MAs is unsupported for this value type.");
            }
            
            template <class SparseMaModelType>
            void SparseMaPcaaWeightVectorChecker<SparseMaModelType>::updateDataToCurrentEpoch(SubModel& MS, SubModel& PS, MinMaxSolverData& minMax, storm::storage::BitVector& consideredObjectives, uint_fast64_t const& currentEpoch, std::vector<ValueType> const& weightVector, TimeBoundMap::iterator& upperTimeBoundIt, TimeBoundMap const& upperTimeBounds) {
                
                if (upperTimeBoundIt != upperTimeBounds.end() && currentEpoch == upperTimeBoundIt->first) {
                    consideredObjectives |= upperTimeBoundIt->second;
                    for (auto objIndex : upperTimeBoundIt->second) {
                        // This objective now plays a role in the weighted sum
                        ValueType factor = storm::solver::minimize(this->objectives[objIndex].formula->getOptimalityType()) ? -weightVector[objIndex] : weightVector[objIndex];
                        storm::utility::vector::addScaledVector(MS.weightedRewardVector, MS.objectiveRewardVectors[objIndex], factor);
                        storm::utility::vector::addScaledVector(PS.weightedRewardVector, PS.objectiveRewardVectors[objIndex], factor);
                    }
                    ++upperTimeBoundIt;
                }
                
                // Update the solver data
                PS.toMS.multiplyWithVector(MS.weightedSolutionVector, minMax.b);
                storm::utility::vector::addVectors(minMax.b, PS.weightedRewardVector, minMax.b);
            }
            
            template <class SparseMaModelType>
            void SparseMaPcaaWeightVectorChecker<SparseMaModelType>::performPSStep(SubModel& PS, SubModel const& MS, MinMaxSolverData& minMax, LinEqSolverData& linEq, std::vector<uint_fast64_t>& optimalChoicesAtCurrentEpoch,  storm::storage::BitVector const& consideredObjectives, std::vector<ValueType> const& weightVector) const {
                // compute a choice vector for the probabilistic states that is optimal w.r.t. the weighted reward vector
                minMax.solver->solveEquations(PS.weightedSolutionVector, minMax.b);
                auto const& newChoices = minMax.solver->getSchedulerChoices();
                if (consideredObjectives.getNumberOfSetBits() == 1 && storm::utility::isOne(weightVector[*consideredObjectives.begin()])) {
                    // In this case there is no need to perform the computation on the individual objectives
                    optimalChoicesAtCurrentEpoch = newChoices;
                    PS.objectiveSolutionVectors[*consideredObjectives.begin()] = PS.weightedSolutionVector;
                    if (storm::solver::minimize(this->objectives[*consideredObjectives.begin()].formula->getOptimalityType())) {
                        storm::utility::vector::scaleVectorInPlace(PS.objectiveSolutionVectors[*consideredObjectives.begin()], -storm::utility::one<ValueType>());
                    }
                } else {
                    // check whether the linEqSolver needs to be updated, i.e., whether the scheduler has changed
                    if (linEq.solver == nullptr || newChoices != optimalChoicesAtCurrentEpoch) {
                        optimalChoicesAtCurrentEpoch = newChoices;
                        linEq.solver = nullptr;
                        storm::storage::SparseMatrix<ValueType> linEqMatrix = PS.toPS.selectRowsFromRowGroups(optimalChoicesAtCurrentEpoch, true);
                        linEqMatrix.convertToEquationSystem();
                        linEq.solver = linEq.factory->create(std::move(linEqMatrix));
                        linEq.solver->setCachingEnabled(true);
                    }
                    
                    // Get the results for the individual objectives.
                    // Note that we do not consider an estimate for each objective (as done in the unbounded phase) since the results from the previous epoch are already pretty close
                    for (auto objIndex : consideredObjectives) {
                        auto const& objectiveRewardVectorPS = PS.objectiveRewardVectors[objIndex];
                        auto const& objectiveSolutionVectorMS = MS.objectiveSolutionVectors[objIndex];
                        // compute rhs of equation system, i.e., PS.toMS * x + Rewards
                        // To safe some time, only do this for the obtained optimal choices
                        auto itGroupIndex = PS.toPS.getRowGroupIndices().begin();
                        auto itChoiceOffset = optimalChoicesAtCurrentEpoch.begin();
                        for (auto& bValue : linEq.b) {
                            uint_fast64_t row = (*itGroupIndex) + (*itChoiceOffset);
                            bValue = objectiveRewardVectorPS[row];
                            for (auto const& entry : PS.toMS.getRow(row)){
                                bValue += entry.getValue() * objectiveSolutionVectorMS[entry.getColumn()];
                            }
                            ++itGroupIndex;
                            ++itChoiceOffset;
                        }
                        linEq.solver->solveEquations(PS.objectiveSolutionVectors[objIndex], linEq.b);
                    }
                }
            }

            template <class SparseMaModelType>
            void SparseMaPcaaWeightVectorChecker<SparseMaModelType>::performMSStep(SubModel& MS, SubModel const& PS, storm::storage::BitVector const& consideredObjectives, std::vector<ValueType> const& weightVector) const {
                
                MS.toMS.multiplyWithVector(MS.weightedSolutionVector, MS.auxChoiceValues);
                storm::utility::vector::addVectors(MS.weightedRewardVector, MS.auxChoiceValues, MS.weightedSolutionVector);
                MS.toPS.multiplyWithVector(PS.weightedSolutionVector, MS.auxChoiceValues);
                storm::utility::vector::addVectors(MS.weightedSolutionVector, MS.auxChoiceValues, MS.weightedSolutionVector);
                if (consideredObjectives.getNumberOfSetBits() == 1 && storm::utility::isOne(weightVector[*consideredObjectives.begin()])) {
                    // In this case there is no need to perform the computation on the individual objectives
                    MS.objectiveSolutionVectors[*consideredObjectives.begin()] = MS.weightedSolutionVector;
                    if (storm::solver::minimize(this->objectives[*consideredObjectives.begin()].formula->getOptimalityType())) {
                        storm::utility::vector::scaleVectorInPlace(MS.objectiveSolutionVectors[*consideredObjectives.begin()], -storm::utility::one<ValueType>());
                    }
                } else {
                    for (auto objIndex : consideredObjectives) {
                        MS.toMS.multiplyWithVector(MS.objectiveSolutionVectors[objIndex], MS.auxChoiceValues);
                        storm::utility::vector::addVectors(MS.objectiveRewardVectors[objIndex], MS.auxChoiceValues, MS.objectiveSolutionVectors[objIndex]);
                        MS.toPS.multiplyWithVector(PS.objectiveSolutionVectors[objIndex], MS.auxChoiceValues);
                        storm::utility::vector::addVectors(MS.objectiveSolutionVectors[objIndex], MS.auxChoiceValues, MS.objectiveSolutionVectors[objIndex]);
                    }
                }
            }
            
            
            template class SparseMaPcaaWeightVectorChecker<storm::models::sparse::MarkovAutomaton<double>>;
            template double SparseMaPcaaWeightVectorChecker<storm::models::sparse::MarkovAutomaton<double>>::getDigitizationConstant<double>(std::vector<double> const& direction) const;
            template void SparseMaPcaaWeightVectorChecker<storm::models::sparse::MarkovAutomaton<double>>::digitize<double>(SparseMaPcaaWeightVectorChecker<storm::models::sparse::MarkovAutomaton<double>>::SubModel& subModel, double const& digitizationConstant) const;
            template void SparseMaPcaaWeightVectorChecker<storm::models::sparse::MarkovAutomaton<double>>::digitizeTimeBounds<double>( SparseMaPcaaWeightVectorChecker<storm::models::sparse::MarkovAutomaton<double>>::TimeBoundMap& upperTimeBounds, double const& digitizationConstant);
            template std::unique_ptr<typename SparseMaPcaaWeightVectorChecker<storm::models::sparse::MarkovAutomaton<double>>::LinEqSolverData>  SparseMaPcaaWeightVectorChecker<storm::models::sparse::MarkovAutomaton<double>>::initLinEqSolver<double>( SparseMaPcaaWeightVectorChecker<storm::models::sparse::MarkovAutomaton<double>>::SubModel const& PS ) const;
#ifdef STORM_HAVE_CARL
            template class SparseMaPcaaWeightVectorChecker<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>;
            template storm::RationalNumber SparseMaPcaaWeightVectorChecker<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>::getDigitizationConstant<storm::RationalNumber>(std::vector<storm::RationalNumber> const& direction) const;
            template void SparseMaPcaaWeightVectorChecker<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>::digitize<storm::RationalNumber>(SparseMaPcaaWeightVectorChecker<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>::SubModel& subModel, storm::RationalNumber const& digitizationConstant) const;
            template void SparseMaPcaaWeightVectorChecker<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>::digitizeTimeBounds<storm::RationalNumber>(SparseMaPcaaWeightVectorChecker<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>::TimeBoundMap& upperTimeBounds, storm::RationalNumber const& digitizationConstant);
            template std::unique_ptr<typename SparseMaPcaaWeightVectorChecker<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>::LinEqSolverData>  SparseMaPcaaWeightVectorChecker<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>::initLinEqSolver<storm::RationalNumber>( SparseMaPcaaWeightVectorChecker<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>::SubModel const& PS ) const;
#endif
            
        }
    }
}
