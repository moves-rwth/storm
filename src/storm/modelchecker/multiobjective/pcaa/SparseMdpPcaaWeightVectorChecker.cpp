#include "storm/modelchecker/multiobjective/pcaa/SparseMdpPcaaWeightVectorChecker.h"

#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/utility/macros.h"
#include "storm/utility/vector.h"
#include "storm/logic/Formulas.h"
#include "storm/exceptions/InvalidPropertyException.h"
#include "storm/exceptions/IllegalArgumentException.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/UnexpectedException.h"


namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            template <class SparseMdpModelType>
            SparseMdpPcaaWeightVectorChecker<SparseMdpModelType>::SparseMdpPcaaWeightVectorChecker(SparseMdpModelType const& model,
                                                                                                   std::vector<Objective<ValueType>> const& objectives,
                                                                                                   storm::storage::BitVector const& possibleECActions,
                                                                                                   storm::storage::BitVector const& possibleBottomStates) :
                SparsePcaaWeightVectorChecker<SparseMdpModelType>(model, objectives, possibleECActions, possibleBottomStates) {
                // set the state action rewards. Also do some sanity checks on the objectives.
                for (uint_fast64_t objIndex = 0; objIndex < this->objectives.size(); ++objIndex) {
                    auto const& formula = *objectives[objIndex].formula;
                    STORM_LOG_THROW(formula.isRewardOperatorFormula() && formula.asRewardOperatorFormula().hasRewardModelName(), storm::exceptions::UnexpectedException, "Unexpected type of operator formula: " << formula);
                    STORM_LOG_THROW(formula.getSubformula().isCumulativeRewardFormula() || formula.getSubformula().isTotalRewardFormula(), storm::exceptions::UnexpectedException, "Unexpected type of sub-formula: " << formula.getSubformula());
                    typename SparseMdpModelType::RewardModelType const& rewModel = this->model.getRewardModel(formula.asRewardOperatorFormula().getRewardModelName());
                    STORM_LOG_THROW(!rewModel.hasTransitionRewards(), storm::exceptions::NotSupportedException, "Reward model has transition rewards which is not expected.");
                    this->discreteActionRewards[objIndex] = rewModel.getTotalRewardVector(this->model.getTransitionMatrix());
                }
            }
            
            template <class SparseMdpModelType>
            void SparseMdpPcaaWeightVectorChecker<SparseMdpModelType>::boundedPhase(std::vector<ValueType> const& weightVector, std::vector<ValueType>& weightedRewardVector) {
                // Currently, only step bounds are considered.
                // TODO: Check whether reward bounded objectives occur.
                bool containsRewardBoundedObjectives = false;
                
                if (containsRewardBoundedObjectives) {
                    boundedPhaseWithRewardBounds(weightVector, weightedRewardVector);
                } else {
                    boundedPhaseOnlyStepBounds(weightVector, weightedRewardVector);
                }
            }

            template <class SparseMdpModelType>
            void SparseMdpPcaaWeightVectorChecker<SparseMdpModelType>::boundedPhaseOnlyStepBounds(std::vector<ValueType> const& weightVector, std::vector<ValueType>& weightedRewardVector) {
                // Allocate some memory so this does not need to happen for each time epoch
                std::vector<uint_fast64_t> optimalChoicesInCurrentEpoch(this->model.getNumberOfStates());
                std::vector<ValueType> choiceValues(weightedRewardVector.size());
                std::vector<ValueType> temporaryResult(this->model.getNumberOfStates());
                // Get for each occurring timeBound the indices of the objectives with that bound.
                std::map<uint_fast64_t, storm::storage::BitVector, std::greater<uint_fast64_t>> stepBounds;
                for (uint_fast64_t objIndex = 0; objIndex < this->objectives.size(); ++objIndex) {
                    if (this->objectives[objIndex].formula->getSubformula().isCumulativeRewardFormula()) {
                        auto const& subformula = this->objectives[objIndex].formula->getSubformula().asCumulativeRewardFormula();
                        uint_fast64_t stepBound = subformula.template getBound<uint_fast64_t>();
                        if (subformula.isBoundStrict()) {
                            --stepBound;
                        }
                        auto stepBoundIt = stepBounds.insert(std::make_pair(stepBound, storm::storage::BitVector(this->objectives.size(), false))).first;
                        stepBoundIt->second.set(objIndex);
                        
                        // There is no error for the values of these objectives.
                        this->offsetsToUnderApproximation[objIndex] = storm::utility::zero<ValueType>();
                        this->offsetsToOverApproximation[objIndex] = storm::utility::zero<ValueType>();
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
                            ValueType factor = storm::solver::minimize(this->objectives[objIndex].formula->getOptimalityType()) ? -weightVector[objIndex] : weightVector[objIndex];
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
            
            
            template <class SparseMdpModelType>
            void SparseMdpPcaaWeightVectorChecker<SparseMdpModelType>::boundedPhaseWithRewardBounds(std::vector<ValueType> const& weightVector, std::vector<ValueType>& weightedRewardVector) {
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Multi-objective model checking with reward bounded objectives is not supported.");
            }
            
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
            
            
            
            template class SparseMdpPcaaWeightVectorChecker<storm::models::sparse::Mdp<double>>;
#ifdef STORM_HAVE_CARL
            template class SparseMdpPcaaWeightVectorChecker<storm::models::sparse::Mdp<storm::RationalNumber>>;
#endif
            
        }
    }
}
