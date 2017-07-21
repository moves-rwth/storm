#include "storm/modelchecker/multiobjective/rewardbounded/MultiDimensionalRewardUnfolding.h"


namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            template<typename ValueType>
            MultiDimensionalRewardUnfolding<ValueType>::MultiDimensionalRewardUnfolding(storm::models::sparse::Mdp<ValueType> const& model, std::vector<storm::modelchecker::multiobjective::Objective<ValueType>> const& objectives, storm::storage::BitVector const& possibleECActions, storm::storage::BitVector const& allowedBottomStates) : model(model), objectives(objectives), possibleECActions(possibleECActions), allowedBottomStates(allowedBottomStates) {
            
            
            
            }
    
            template<typename ValueType>
            typename MultiDimensionalRewardUnfolding<ValueType>::Epoch MultiDimensionalRewardUnfolding<ValueType>::getStartEpoch() {
                return Epoch();
            }
    
            template<typename ValueType>
            std::vector<typename MultiDimensionalRewardUnfolding<ValueType>::Epoch> MultiDimensionalRewardUnfolding<ValueType>::getEpochComputationOrder(Epoch const& startEpoch) {
                return std::vector<Epoch>();
            }
    
            template<typename ValueType>
            typename MultiDimensionalRewardUnfolding<ValueType>::EpochModel const& MultiDimensionalRewardUnfolding<ValueType>::getModelForEpoch(Epoch const& epoch) {
                return EpochModel();
            }
    
            template<typename ValueType>
            void MultiDimensionalRewardUnfolding<ValueType>::setEpochSolution(Epoch const& epoch, EpochSolution const& solution) {
        
            }
    
            template<typename ValueType>
            void MultiDimensionalRewardUnfolding<ValueType>::setEpochSolution(Epoch const& epoch, EpochSolution&& solution) {
        
            }
    
            template<typename ValueType>
            typename MultiDimensionalRewardUnfolding<ValueType>::EpochSolution const& MultiDimensionalRewardUnfolding<ValueType>::getEpochSolution(Epoch const& epoch) {
                return EpochSolution();
            }
    
            template<typename ValueType>
            typename MultiDimensionalRewardUnfolding<ValueType>::EpochClass MultiDimensionalRewardUnfolding<ValueType>::getClassOfEpoch(Epoch const& epoch) {
                return 0;
            }
    
    
            /*
            template<class SparseModelType>
            void SparseMdpPcaaWeightVectorChecker<SparseModelType>::discretizeRewardBounds() {
                
                std::map<std::string, uint_fast64_t> rewardModelNameToDimension;
                objIndexToRewardDimensionMapping = std::vector<uint_fast64_t>(this->objectives.size(), std::numeric_limits<uint_fast64_t>::max());
                
                // Collect the reward models that occur as reward bound
                uint_fast64_t dimensionCount = 0;
                for (uint_fast64_t objIndex = 0; objIndex < this->objectives.size(); ++objIndex) {
                    auto const& obj = this->objectives[objIndex];
                    if (obj.timeBoundReference && obj.timeBoundReference->isRewardBound()) {
                        auto insertionRes = rewardModelNameToDimension.insert(std::make_pair(obj.timeBoundReference->getRewardName(), dimensionCount));
                        objIndexToRewardDimensionMapping[objIndex] = insertionRes.first->second;
                        if (insertionRes.second) {
                            ++dimensionCount;
                        }
                    }
                }

                // Now discretize each reward
                discretizedActionRewards.reserve(dimensionCount);
                discretizedRewardBounds = std::vector<uint_fast64_t>(this->objectives.size(), 0);
                for (auto const& rewardName : rewardModelNameToDimension) {
                    STORM_LOG_THROW(this->model.hasRewardModel(rewardName.first), storm::exceptions::IllegalArgumentException, "No reward model with name '" << rewardName.first << "' found.");
                    auto const& rewardModel = this->model.getRewardModel(rewardName.first);
                    STORM_LOG_THROW(!rewardModel.hasTransitionRewards(), storm::exceptions::NotSupportedException, "Transition rewards are currently not supported.");
                    std::vector<typename RewardModelType::ValueType> actionRewards = rewardModel.getTotalRewardVector(this->model.getTransitionMatrix());
                    auto discretizedRewardsAndFactor = storm::utility::vector::toIntegralVector<typename RewardModelType::ValueType, uint_fast64_t>(actionRewards);
   
                    discretizedActionRewards.push_back(std::move(discretizedRewardsAndFactor.first));
                    // Find all objectives that reffer to this reward model and apply the discretization factor to the reward bounds
                    for (uint_fast64_t objIndex = 0; objIndex < objIndexToRewardDimensionMapping.size(); ++objIndex) {
                        if (objIndexToRewardDimensionMapping[objIndex] == rewardName.second) {
                            auto const& obj = this->objectives[objIndex];
                            STORM_LOG_THROW(!obj.lowerTimeBound, storm::exceptions::NotSupportedException, "Lower time bounds are not supported.");
                            STORM_LOG_THROW(obj.upperTimeBound, storm::exceptions::UnexpectedException, "Got formula with a bound reference but no bound.");
                            STORM_LOG_THROW(!obj.upperTimeBound->getBound().containsVariables(), storm::exceptions::UnexpectedException, "The upper bound of a reward bounded formula still contains variables.");
                            typename RewardModelType::ValueType discretizedBound = storm::utility::convertNumber<typename RewardModelType::ValueType>(obj.upperTimeBound->getBound().evaluateAsRational());
                            discretizedBound /= discretizedRewardsAndFactor.second;
                            if (obj.upperTimeBound->isStrict() && discretizedBound == storm::utility::floor(discretizedBound)) {
                                discretizedBound = storm::utility::floor(discretizedBound) - storm::utility::one<ValueType>();
                            } else {
                                discretizedBound = storm::utility::floor(discretizedBound);
                            }
                            discretizedRewardBounds[objIndex] = storm::utility::convertNumber<uint_fast64_t>(discretizedBound);
                        }
                    }
                }
            }
             */
            
            template class MultiDimensionalRewardUnfolding<double>;
            template class MultiDimensionalRewardUnfolding<storm::RationalNumber>;
            
        }
    }
}