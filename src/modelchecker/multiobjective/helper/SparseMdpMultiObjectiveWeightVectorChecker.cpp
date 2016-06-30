#include "src/modelchecker/multiobjective/helper/SparseMdpMultiObjectiveWeightVectorChecker.h"

#include "src/adapters/CarlAdapter.h"
#include "src/models/sparse/Mdp.h"
#include "src/models/sparse/StandardRewardModel.h"
#include "src/utility/macros.h"
#include "src/utility/vector.h"


namespace storm {
    namespace modelchecker {
        namespace helper {
            
            template <class SparseMdpModelType>
            SparseMdpMultiObjectiveWeightVectorChecker<SparseMdpModelType>::SparseMdpMultiObjectiveWeightVectorChecker(PreprocessorData const& data) : SparseMultiObjectiveWeightVectorChecker<SparseMdpModelType>(data) {
                // set the state action rewards
                for(uint_fast64_t objIndex = 0; objIndex < data.objectives.size(); ++objIndex) {
                    STORM_LOG_ASSERT(!this->data.preprocessedModel.getRewardModel(this->data.objectives[objIndex].rewardModelName).hasTransitionRewards(), "Reward model has transition rewards which is not expected.");
                    this->discreteActionRewards[objIndex] = this->data.preprocessedModel.getRewardModel(this->data.objectives[objIndex].rewardModelName).getTotalRewardVector(this->data.preprocessedModel.getTransitionMatrix());
                }
            }
            
            template <class SparseMdpModelType>
            void SparseMdpMultiObjectiveWeightVectorChecker<SparseMdpModelType>::boundedPhase(std::vector<ValueType> const& weightVector, std::vector<ValueType>& weightedRewardVector) {
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
            }
            
            template class SparseMdpMultiObjectiveWeightVectorChecker<storm::models::sparse::Mdp<double>>;
#ifdef STORM_HAVE_CARL
            template class SparseMdpMultiObjectiveWeightVectorChecker<storm::models::sparse::Mdp<storm::RationalNumber>>;
#endif
            
        }
    }
}
