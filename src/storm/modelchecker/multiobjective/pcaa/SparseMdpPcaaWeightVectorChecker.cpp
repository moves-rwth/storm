#include "storm/modelchecker/multiobjective/pcaa/SparseMdpPcaaWeightVectorChecker.h"

#include "storm/adapters/CarlAdapter.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/utility/macros.h"
#include "storm/utility/vector.h"
#include "storm/exceptions/InvalidPropertyException.h"


namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            template <class SparseMdpModelType>
            SparseMdpPcaaWeightVectorChecker<SparseMdpModelType>::SparseMdpPcaaWeightVectorChecker(SparseMdpModelType const& model,
                                                                                                   std::vector<Objective<ValueType>> const& objectives,
                                                                                                   storm::storage::BitVector const& possibleECActions,
                                                                                                   storm::storage::BitVector const& possibleBottomStates) :
                SparsePcaaWeightVectorChecker<SparseMdpModelType>(model, objectives, possibleECActions, possibleBottomStates) {
                // set the state action rewards
                for (uint_fast64_t objIndex = 0; objIndex < this->objectives.size(); ++objIndex) {
                    typename SparseMdpModelType::RewardModelType const& rewModel = this->model.getRewardModel(*this->objectives[objIndex].rewardModelName);
                    STORM_LOG_ASSERT(!rewModel.hasTransitionRewards(), "Reward model has transition rewards which is not expected.");
                    this->discreteActionRewards[objIndex] = rewModel.getTotalRewardVector(this->model.getTransitionMatrix());
                }
            }
            
            template <class SparseMdpModelType>
            void SparseMdpPcaaWeightVectorChecker<SparseMdpModelType>::boundedPhase(std::vector<ValueType> const& weightVector, std::vector<ValueType>& weightedRewardVector) {
                // Allocate some memory so this does not need to happen for each time epoch
                std::vector<uint_fast64_t> optimalChoicesInCurrentEpoch(this->model.getNumberOfStates());
                std::vector<ValueType> choiceValues(weightedRewardVector.size());
                std::vector<ValueType> temporaryResult(this->model.getNumberOfStates());
                // Get for each occurring timeBound the indices of the objectives with that bound.
                std::map<uint_fast64_t, storm::storage::BitVector, std::greater<uint_fast64_t>> stepBounds;
                for (uint_fast64_t objIndex = 0; objIndex < this->objectives.size(); ++objIndex) {
                    auto const& obj = this->objectives[objIndex];
                    STORM_LOG_THROW(!obj.lowerTimeBound, storm::exceptions::InvalidPropertyException, "Lower step bounds are not supported by this model checker");
                    if (obj.upperTimeBound) {
                        STORM_LOG_THROW(!obj.upperTimeBound->getBound().containsVariables(), storm::exceptions::InvalidPropertyException, "The step bound '" << obj.upperTimeBound->getBound() << " contains undefined variables");
                        uint_fast64_t stepBound = (uint_fast64_t) obj.upperTimeBound->getBound().evaluateAsInt();
                        if (obj.upperTimeBound->isStrict()) {
                            --stepBound;
                        }
                        auto stepBoundIt = stepBounds.insert(std::make_pair(stepBound, storm::storage::BitVector(this->objectives.size(), false))).first;
                        stepBoundIt->second.set(objIndex);
                        
                        // There is no error for the values of these objectives.
                        this->offsetsToLowerBound[objIndex] = storm::utility::zero<ValueType>();
                        this->offsetsToUpperBound[objIndex] = storm::utility::zero<ValueType>();
                    }
                }
                
                // Stores the objectives for which we need to compute values in the current time epoch.
                storm::storage::BitVector consideredObjectives = this->objectivesWithNoUpperTimeBound;
                
                auto stepBoundIt = stepBounds.begin();
                uint_fast64_t currentEpoch = stepBounds.empty() ? 0 : stepBoundIt->first;
                
                while (currentEpoch > 0) {
                    
                    if (stepBoundIt != stepBounds.end() && currentEpoch == stepBoundIt->first) {
                        consideredObjectives |= stepBoundIt->second;
                        for(auto objIndex : stepBoundIt->second) {
                            // This objective now plays a role in the weighted sum
                            ValueType factor = storm::solver::minimize(this->objectives[objIndex].optimizationDirection) ? -weightVector[objIndex] : weightVector[objIndex];
                            storm::utility::vector::addScaledVector(weightedRewardVector, this->discreteActionRewards[objIndex], factor);
                        }
                        ++stepBoundIt;
                    }
                    
                    // Get values and scheduler for weighted sum of objectives
                    this->model.getTransitionMatrix().multiplyWithVector(this->weightedResult, choiceValues);
                    storm::utility::vector::addVectors(choiceValues, weightedRewardVector, choiceValues);
                    storm::utility::vector::reduceVectorMax(choiceValues, this->weightedResult, this->model.getTransitionMatrix().getRowGroupIndices(), &optimalChoicesInCurrentEpoch);
                    
                    // get values for individual objectives
                    // TODO we could compute the result for one of the objectives from the weighted result, the given weight vector, and the remaining objective results.
                    for (auto objIndex : consideredObjectives) {
                        std::vector<ValueType>& objectiveResult = this->objectiveResults[objIndex];
                        std::vector<ValueType> const& objectiveRewards = this->discreteActionRewards[objIndex];
                        auto rowGroupIndexIt = this->model.getTransitionMatrix().getRowGroupIndices().begin();
                        auto optimalChoiceIt = optimalChoicesInCurrentEpoch.begin();
                        for(ValueType& stateValue : temporaryResult){
                            uint_fast64_t row = (*rowGroupIndexIt) + (*optimalChoiceIt);
                            ++rowGroupIndexIt;
                            ++optimalChoiceIt;
                            stateValue = objectiveRewards[row];
                            for(auto const& entry : this->model.getTransitionMatrix().getRow(row)) {
                                stateValue += entry.getValue() * objectiveResult[entry.getColumn()];
                            }
                        }
                        objectiveResult.swap(temporaryResult);
                    }
                    --currentEpoch;
                }
            }
            
            template class SparseMdpPcaaWeightVectorChecker<storm::models::sparse::Mdp<double>>;
#ifdef STORM_HAVE_CARL
            template class SparseMdpPcaaWeightVectorChecker<storm::models::sparse::Mdp<storm::RationalNumber>>;
#endif
            
        }
    }
}
