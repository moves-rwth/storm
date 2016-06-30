#include "src/modelchecker/multiobjective/helper/SparseMaMultiObjectiveWeightVectorChecker.h"

#include "src/adapters/CarlAdapter.h"
#include "src/models/sparse/MarkovAutomaton.h"
#include "src/models/sparse/StandardRewardModel.h"
#include "src/utility/macros.h"
#include "src/utility/vector.h"


namespace storm {
    namespace modelchecker {
        namespace helper {
            
            template <class SparseMaModelType>
            SparseMaMultiObjectiveWeightVectorChecker<SparseMaModelType>::SparseMaMultiObjectiveWeightVectorChecker(PreprocessorData const& data) : SparseMultiObjectiveWeightVectorChecker<SparseMaModelType>(data) {
                // Set the (discretized) state action rewards.
                this->discreteActionRewards.resize(data.objectives.size());
                for(auto objIndex : this->unboundedObjectives) {
                    STORM_LOG_ASSERT(!this->data.preprocessedModel.getRewardModel(this->data.objectives[objIndex].rewardModelName).hasTransitionRewards(), "Preprocessed Reward model has transition rewards which is not expected.");
                    this->discreteActionRewards[objIndex] = this->data.preprocessedModel.getRewardModel(this->data.objectives[objIndex].rewardModelName).getStateActionRewardVector();
                    if(this->data.preprocessedModel.getRewardModel(this->data.objectives[objIndex].rewardModelName).hasStateRewards()) {
                        auto const& stateRewards = this->data.preprocessedModel.getRewardModel(this->data.objectives[objIndex].rewardModelName).getStateRewardVector();
                        for(auto markovianState : this->data.getMarkovianStatesOfPreprocessedModel()) {
                            this->discreteActionRewards[objIndex][this->data.preprocessedModel.getTransitionMatrix().getRowGroupIndices()[markovianState]] += stateRewards[markovianState] / this->data.preprocessedModel.getExitRate(markovianState);
                        }
                    }
                    
                }
            }
            
            template <class SparseMaModelType>
            void SparseMaMultiObjectiveWeightVectorChecker<SparseMaModelType>::boundedPhase(std::vector<ValueType> const& weightVector, std::vector<ValueType>& weightedRewardVector) {
                STORM_LOG_ERROR("BOUNDED OBJECTIVES FOR MARKOV AUTOMATA NOT YET IMPLEMENTED");
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
                        std::vector<ValueType> objectiveRewards = getObjectiveRewardAsDiscreteActionRewards(objIndex);
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
            
            template class SparseMaMultiObjectiveWeightVectorChecker<storm::models::sparse::MarkovAutomaton<double>>;
#ifdef STORM_HAVE_CARL
            template class SparseMaMultiObjectiveWeightVectorChecker<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>;
#endif
            
        }
    }
}
