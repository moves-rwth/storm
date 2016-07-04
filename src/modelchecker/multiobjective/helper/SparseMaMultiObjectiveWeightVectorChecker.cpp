#include "src/modelchecker/multiobjective/helper/SparseMaMultiObjectiveWeightVectorChecker.h"

#include <cmath>

#include "src/adapters/CarlAdapter.h"
#include "src/models/sparse/MarkovAutomaton.h"
#include "src/models/sparse/StandardRewardModel.h"
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
                for(auto objIndex : this->unboundedObjectives) {
                    typename SparseMaModelType::RewardModelType const& rewModel = this->data.preprocessedModel.getRewardModel(this->data.objectives[objIndex].rewardModelName);
                    STORM_LOG_ASSERT(!rewModel.hasTransitionRewards(), "Preprocessed Reward model has transition rewards which is not expected.");
                    this->discreteActionRewards[objIndex] = rewModel.hasStateActionRewards() ? rewModel.getStateActionRewardVector() : std::vector<ValueType>(this->data.preprocessedModel.getTransitionMatrix().getRowCount(), storm::utility::zero<ValueType>());
                    if(rewModel.hasStateRewards()) {
                        for(auto markovianState : this->data.getMarkovianStatesOfPreprocessedModel()) {
                            this->discreteActionRewards[objIndex][this->data.preprocessedModel.getTransitionMatrix().getRowGroupIndices()[markovianState]] += rewModel.getStateReward(markovianState) / this->data.preprocessedModel.getExitRate(markovianState);
                        }
                    }
                    
                }

            }
            
            template <class SparseMaModelType>
            void SparseMaMultiObjectiveWeightVectorChecker<SparseMaModelType>::boundedPhase(std::vector<ValueType> const& weightVector, std::vector<ValueType>& weightedRewardVector) {
 
                
                // Set the digitization constant
                ValueType digitizationConstant = getDigitizationConstant();
                std::cout << "Got delta: " << digitizationConstant << std::endl;
                
      /*
                // Allocate some memory so this does not need to happen for each time epoch
                std::vector<uint_fast64_t> optimalChoicesInCurrentEpoch(this->data.preprocessedModel.getNumberOfStates());
                std::vector<ValueType> choiceValues(weightedRewardVector.size());
                std::vector<ValueType> temporaryResult(this->data.preprocessedModel.getNumberOfStates());
                // Get for each occurring timeBound the indices of the objectives with that bound.
                std::map<uint_fast64_t, storm::storage::BitVector, std::greater<uint_fast64_t>> timeBounds;
                storm::storage::BitVector boundedObjectives = ~this->unboundedObjectives;
                for(uint_fast64_t objIndex : boundedObjectives) {
                    uint_fast64_t timeBound = boost::get<uint_fast64_t>(this->data.objectives[objIndex].timeBounds.get());
                    auto timeBoundIt = timeBounds.insert(std::make_pair(timeBound, storm::storage::BitVector(this->data.objectives.size(), false))).first;
                    timeBoundIt->second.set(objIndex);
                    // There is no error for the values of these objectives.
                    this->offsetsToLowerBound[objIndex] = storm::utility::zero<ValueType>();
                    this->offsetsToUpperBound[objIndex] = storm::utility::zero<ValueType>();
                }
                storm::storage::BitVector objectivesAtCurrentEpoch = this->unboundedObjectives;
                auto timeBoundIt = timeBounds.begin();
                for(uint_fast64_t currentEpoch = timeBoundIt->first; currentEpoch > 0; --currentEpoch) {
                    if(timeBoundIt != timeBounds.end() && currentEpoch == timeBoundIt->first) {
                        objectivesAtCurrentEpoch |= timeBoundIt->second;
                        for(auto objIndex : timeBoundIt->second) {
                            storm::utility::vector::addScaledVector(weightedRewardVector, this->discreteActionRewards[objIndex], weightVector[objIndex]);
                        }
                        ++timeBoundIt;
                    }
                    
                    // Get values and scheduler for weighted sum of objectives
                    this->data.preprocessedModel.getTransitionMatrix().multiplyWithVector(this->weightedResult, choiceValues);
                    storm::utility::vector::addVectors(choiceValues, weightedRewardVector, choiceValues);
                    storm::utility::vector::reduceVectorMax(choiceValues, this->weightedResult, this->data.preprocessedModel.getTransitionMatrix().getRowGroupIndices(), &optimalChoicesInCurrentEpoch);
                    
                    // get values for individual objectives
                    // TODO we could compute the result for one of the objectives from the weighted result, the given weight vector, and the remaining objective results.
                    for(auto objIndex : objectivesAtCurrentEpoch) {
                        std::vector<ValueType>& objectiveResult = this->objectiveResults[objIndex];
                        std::vector<ValueType> objectiveRewards = this->discreteActionRewards[objIndex];
                        auto rowGroupIndexIt = this->data.preprocessedModel.getTransitionMatrix().getRowGroupIndices().begin();
                        auto optimalChoiceIt = optimalChoicesInCurrentEpoch.begin();
                        for(ValueType& stateValue : temporaryResult){
                            uint_fast64_t row = (*rowGroupIndexIt) + (*optimalChoiceIt);
                            ++rowGroupIndexIt;
                            ++optimalChoiceIt;
                            stateValue = objectiveRewards[row];
                            for(auto const& entry : this->data.preprocessedModel.getTransitionMatrix().getRow(row)) {
                                stateValue += entry.getValue() * objectiveResult[entry.getColumn()];
                            }
                        }
                        objectiveResult.swap(temporaryResult);
                    }
                }
*/
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
                std::vector<std::pair<VT, VT>> lowerUpperBounds;
                std::vector<std::pair<VT, VT>> eToPowerOfMinusMaxRateTimesBound;
                for(auto const& obj : this->data.objectives) {
                    if(obj.timeBounds) {
                        if(obj.timeBounds->which() == 0) {
                            lowerUpperBounds.emplace_back(storm::utility::zero<VT>(), boost::get<uint_fast64_t>(*obj.timeBounds));
                            eToPowerOfMinusMaxRateTimesBound.emplace_back(storm::utility::one<VT>(), std::exp(-maxRate * lowerUpperBounds.back().second));
                        } else {
                            auto const& pair = boost::get<std::pair<double, double>>(*obj.timeBounds);
                            lowerUpperBounds.emplace_back(storm::utility::convertNumber<VT>(pair.first), storm::utility::convertNumber<VT>(pair.second));
                            eToPowerOfMinusMaxRateTimesBound.emplace_back(std::exp(-maxRate * lowerUpperBounds.back().first), std::exp(-maxRate * lowerUpperBounds.back().second));
                        }
                    }
                }
                VT smallestNonZeroBound = storm::utility::zero<VT>();
                for (auto const& bounds : lowerUpperBounds) {
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
                    for(auto const& bounds : lowerUpperBounds) {
                        if(bounds.first/delta  != std::floor(bounds.first/delta) ||
                           bounds.second/delta != std::floor(bounds.second/delta)) {
                            deltaValid = false;
                            break;
                        }
                    }
                    if(deltaValid) {
                        for(uint_fast64_t i = 0; i<lowerUpperBounds.size(); ++i) {
                            VT precisionOfObj = storm::utility::one<VT>() - (eToPowerOfMinusMaxRateTimesBound[i].first * storm::utility::pow(storm::utility::one<VT>() + maxRate * delta, lowerUpperBounds[i].first / delta) );
                            precisionOfObj += storm::utility::one<VT>() - (eToPowerOfMinusMaxRateTimesBound[i].second * storm::utility::pow(storm::utility::one<VT>() + maxRate * delta, lowerUpperBounds[i].second / delta) );
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

            template class SparseMaMultiObjectiveWeightVectorChecker<storm::models::sparse::MarkovAutomaton<double>>;
            template double SparseMaMultiObjectiveWeightVectorChecker<storm::models::sparse::MarkovAutomaton<double>>::getDigitizationConstant<double>() const;
#ifdef STORM_HAVE_CARL
            template storm::RationalNumber SparseMaMultiObjectiveWeightVectorChecker<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>::getDigitizationConstant<storm::RationalNumber>() const;
            template class SparseMaMultiObjectiveWeightVectorChecker<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>;
#endif
            
        }
    }
}
